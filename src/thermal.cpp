#include "thermal.h"

static int
baud_to_const(int baud)
{
// clang-format off
#define _B(x) case x: return B##x // clang-format on
    switch (baud) {
        _B(0);
        _B(50);
        _B(75);
        _B(110);
        _B(134);
        _B(150);
        _B(200);
        _B(300);
        _B(600);
        _B(1200);
        _B(1800);
        _B(2400);
        _B(4800);
        _B(9600);
        _B(19200);
        _B(38400);
        _B(57600);
        _B(115200);
        _B(230400);
        default:
            return 0;
    }
#undef _B
}

int
open_serial(const char* path, int baud)
{
    termios* tty = new termios;

    int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd < 0) {
        delete tty;
        return -1;
    }
    // Take over existing settings from open serial port
    if (tcgetattr(fd, tty) != 0) {
        delete tty;
        return -1;
    }

    // Clear parity bit, disabling parity (most common)
    tty->c_cflag &= ~PARENB;
    // Clear stop field, only one stop bit used in communication (most common)
    tty->c_cflag &= ~CSTOPB;
    // Use 8 bits per byte
    tty->c_cflag |= CS8;
    // Disable RTS/CTS hardware flow control
    tty->c_cflag &= ~CRTSCTS;
    // Disables software flow control
    tty->c_iflag &= ~(IXON | IXOFF | IXANY);

    tty->c_lflag &= ~ICANON;
    cfsetispeed(tty, baud_to_const(baud));

    if (tcsetattr(fd, TCSANOW, tty) != 0) {
        delete tty;
        return -1;
    }
    delete tty;
    return fd;
}

int
close_serial(int fd)
{
    return close(fd);
}

int
write_serial(int fd, const std::string& msg)
{
    return write(fd, msg.c_str(), msg.size());
}

int
write_serial(int fd, const char* msg)
{
    std::string str(msg);
    // It is way easier to handle the size std::string than it is for
    // char buffers. Thus, we convert the message to a string.
    return write_serial(fd, str);
}

int
write_serial(int fd, char byte)
{
    char buf[1] = {byte};
    return write(fd, buf, 1);
}

int
write_serial(int fd, char b1, char b2, char b3)
{
    char buf[3] = {b1, b2, b3};
    return write(fd, buf, 3);
}

static void
Printer_dealloc(PrinterObject *self)
{
    /*Py_XDECREF(self->port);
    std::cout << self->_fd << std::endl;
    if (self->_fd != 0) {
        close_serial(self->_fd);
        self->_fd = 0;
    } */
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Printer_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PrinterObject *self;
    self = (PrinterObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->port = PyUnicode_FromString("");
        if (self->port == NULL) {
            // Something failed here
            Py_DECREF(self);
            return NULL;
        }
        self->baudrate = 0;
        self->_fd = 0;
    }
    return (PyObject *) self;
}

static int
Printer_init(PrinterObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *port = NULL;
    if (!PyArg_ParseTuple(args, "Oi", &port, &self->baudrate))
        return -1;
    if (port) {
        PyObject *old_port = self->port;
        Py_INCREF(port);
        self->port = port;
        Py_DECREF(old_port);
    } else {
        // Well, this means that the port was not provided properly.
        // This should never happen.
        return -1;
    }
    return 0;
}

static PyObject *
Printer_open(PrinterObject *self, PyObject *Py_UNUSED(ignored))
{
    if (self->port == NULL || self->port == Py_None) {
        PyErr_SetString(PyExc_AttributeError, "'port' can not be None");
        return NULL;
    }
    // `port` will later be used
    char *port = NULL;
    PyObject *path_bytes = NULL;
    if (PyUnicode_Check(self->port)) {
        // `self->port` will be referenced py `path_bytes`
        Py_INCREF(self->port);
        path_bytes = PyUnicode_AsEncodedString(self->port, "UTF-8", "strict");
        if (path_bytes != NULL) {
            port = PyBytes_AsString(path_bytes);
        } else {
            // `path_bytes` is going to be `NULL`, so this has no effect
            Py_XDECREF(path_bytes);
            Py_DECREF(self->port);
            PyErr_SetString(PyExc_ValueError, "Error while parsing 'port' string");
            return NULL;
        }
    } else {
        // Reference counter of `self->port` has not yet been
        // incremented and `path_bytes` is going to be `NULL`
        Py_XDECREF(path_bytes);
        PyErr_SetString(PyExc_ValueError, "'port' is not a valid string");
        return NULL;
    }
    int fd = open_serial(port, self->baudrate);
    if (fd == -1) {
        // `open_serial` returned -1 indicating an error
        PyErr_SetFromErrnoWithFilename(PyExc_OSError, port);
        Py_DECREF(path_bytes);
        Py_DECREF(self->port);
        return NULL;
    }
    // `path_bytes` should by no chance be `NULL`, thus, there is should
    // be no need to use `Py_XDECREF`
    Py_DECREF(path_bytes);
    Py_DECREF(self->port);
    self->_fd = fd;
    Py_RETURN_NONE;
}

static PyObject *
Printer_close(PrinterObject *self, PyObject *Py_UNUSED(ignored))
{
    if (close_serial(self->_fd) != 0)
        return PyErr_SetFromErrno(PyExc_OSError);
    self->_fd = 0;
    Py_RETURN_NONE;
}

static PrinterObject *
Printer__enter__(PrinterObject *self, PyObject *Py_UNUSED(ignored))
{
    Printer_open(self, NULL);
    return self;
}

static PyObject *
Printer__exit__(PrinterObject *self, PyObject* args)
{
    Printer_close(self, NULL);
    Py_RETURN_NONE;
}

PyObject*
Printer_write(PrinterObject *self, PyObject* args)
{
    const char* msg;
    long msg_length;

    if (!PyArg_ParseTuple(args, "s#", &msg, &msg_length)) {
        return NULL;
    }
    int bytes = write(self->_fd, msg, msg_length);
    if (bytes == -1) {
        return PyErr_SetFromErrno(PyExc_OSError);
    }
    return PyLong_FromLong((long)bytes);
}

PyObject*
Printer_write_bytes(PrinterObject *self, PyObject* args)
{
    const char* bytes;
    long bytes_length;

    if (!PyArg_ParseTuple(args, "y#", &bytes, &bytes_length)) {
        return NULL;
    }
    int byte_count = 0;
    for (long i = 0; i < bytes_length; i++) {
        int n_byte_count = write_serial(self->_fd, bytes[i]);
        if (n_byte_count == -1)
            return PyErr_SetFromErrno(PyExc_OSError);
        byte_count += n_byte_count;
    }
    return PyLong_FromLong((long)byte_count);
}

PyMODINIT_FUNC
PyInit_thermal(void)
{
    PyObject *m;
    if (PyType_Ready(&PrinterType) < 0)
        return NULL;

    m = PyModule_Create(&thermalmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PrinterType);
    if (PyModule_AddObject(m, "Printer", (PyObject *) &PrinterType) < 0) {
        Py_DECREF(&PrinterType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}