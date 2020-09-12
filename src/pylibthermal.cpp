#include "pylibthermal.h"

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
open_serial(std::string& path, int baud)
{
    termios* tty = new termios;

    int fd = open(path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

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

PyObject*
thermal_open_serial(PyObject* self, PyObject* args)
{
    const char* path;
    int baud;

    if (!PyArg_ParseTuple(args, "si", &path, &baud))
        return NULL;
    std::string path_str(path);
    int fd = open_serial(path_str, baud);
    if (fd == -1)
        return PyErr_SetFromErrnoWithFilename(PyExc_OSError, path);
    return PyLong_FromLong((long)fd);
}

PyObject*
thermal_close_serial(PyObject* self, PyObject* args)
{
    long fd;

    if (!PyArg_ParseTuple(args, "l", &fd)) {
        return NULL;
    }
    if (close_serial((int)fd) != 0) {
        return PyErr_SetFromErrno(PyExc_OSError);
    }
    Py_RETURN_NONE;
}

PyObject*
thermal_write_serial(PyObject* self, PyObject* args)
{
    long fd;
    const char* msg;
    long msg_length;

    if (!PyArg_ParseTuple(args, "ls#", &fd, &msg, &msg_length)) {
        return NULL;
    }
    int bytes = write(fd, msg, msg_length);
    if (bytes == -1) {
        return PyErr_SetFromErrno(PyExc_OSError);
    }
    return PyLong_FromLong((long)bytes);
}

PyObject*
thermal_write_serial_bytes(PyObject* self, PyObject* args)
{
    long fd;
    const char* bytes;
    long bytes_length;

    if (!PyArg_ParseTuple(args, "ly#", &fd, &bytes, &bytes_length)) {
        return NULL;
    }
    int byte_count = 0;
    for (long i = 0; i < bytes_length; i++) {
        int n_byte_count = write_serial(fd, bytes[i]);
        if (n_byte_count == -1)
            return PyErr_SetFromErrno(PyExc_OSError);
        byte_count += n_byte_count;
    }
    return PyLong_FromLong((long)byte_count);
}

static PyMethodDef ThermalMethods[] = {
    {"open_serial",
     thermal_open_serial,
     METH_VARARGS,
     "Open the serial port to a thermal printer."},
    {"close_serial",
     thermal_close_serial,
     METH_VARARGS,
     "Close the serial port of a given file descriptor."},
    {"write_serial",
     thermal_write_serial,
     METH_VARARGS,
     "Write a string to the thermal printer."},
    {"write_serial_bytes",
     thermal_write_serial_bytes,
     METH_VARARGS,
     "Write raw bytes to the thermal printer."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef thermalmodule =
    {PyModuleDef_HEAD_INIT, "thermal", NULL, -1, ThermalMethods};

PyMODINIT_FUNC
PyInit__pylibthermal(void)
{
    return PyModule_Create(&thermalmodule);
}
