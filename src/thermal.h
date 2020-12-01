#ifndef LIBTHERMAL_H
#define LIBTHERMAL_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <errno.h> // Error number definitions
#include <fcntl.h> // File control definitions
#include <iostream>
#include <sstream>
#include <stdio.h>  // Standard input/output definitions
#include <string.h> // String function definitions
#include <string>
#include <termios.h> // POSIX calls for tty I/O control
#include <unistd.h>  // UNIX standard function definitions

int
open_serial(std::string& path, int baud);
int
close_serial(int fd);

int
write_serial(int fd, const std::string& msg);
int
write_serial(int fd, const char* msg);

int
write_serial(int fd, char byte);
int
write_serial(int fd, char b1, char b2, char b3);
/*
static PyObject*
thermal_open_serial(PyObject* self, PyObject* args);
static PyObject*
thermal_close_serial(PyObject* self, PyObject* args);

static PyObject*
thermal_write_serial(PyObject* self, PyObject* args);
static PyObject*
thermal_write_serial_bytes(PyObject* self, PyObject* args);

PyMODINIT_FUNC
PyInit_thermal(void);
*/
typedef struct {
    PyObject_HEAD
    PyObject *port;
    int baudrate;
    int _fd;
} PrinterObject;

static void
Printer_dealloc(PrinterObject *self);

static PyObject *
Printer_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

static int
Printer_init(PrinterObject *self, PyObject *args, PyObject *kwds);

static PyMemberDef Printer_members[] = {
    {"port",
     T_OBJECT_EX, offsetof(PrinterObject, port),
     0,
     "Serial port used to communicate with the thermal printer."},
    {"baudrate",
     T_INT, offsetof(PrinterObject, baudrate),
     0,
     "Serial baudrate used for communication."},
    {NULL}  /* Sentinel */
};

static PyObject *
Printer_open(PrinterObject *self, PyObject *Py_UNUSED(ignored));
static PyObject *
Printer_close(PrinterObject *self, PyObject *Py_UNUSED(ignored));

static PrinterObject *
Printer__enter__(PrinterObject *self, PyObject *Py_UNUSED(ignored));
static PyObject *
Printer__exit__(PrinterObject *self, PyObject* args);

static PyObject*
Printer_write(PrinterObject *self, PyObject* args);
static PyObject*
Printer_write_bytes(PrinterObject *self, PyObject* args);

static PyMethodDef Printer_methods[] = {
    {"open",
     (PyCFunction) Printer_open,
     METH_NOARGS,
     "Open the serial port to a thermal printer."},
    {"close",
     (PyCFunction) Printer_close,
     METH_NOARGS,
     "Close the serial port of a given file descriptor."},
    {"__enter__",
     (PyCFunction) Printer__enter__,
     METH_NOARGS,
     "Used for the context manager"},
    {"__exit__",
     (PyCFunction) Printer__exit__,
     METH_VARARGS,
     "Used for the context manager"},
    {"write",
     (PyCFunction) Printer_write,
     METH_VARARGS,
     "Write a string to the thermal printer"},
    {"write_bytes",
     (PyCFunction) Printer_write_bytes,
     METH_VARARGS,
     "Write raw bytes to the thermal printer"},
    {NULL}  /* Sentinel */
};

static PyTypeObject PrinterType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "thermal.Printer",
    .tp_doc = "Thermal Printer",
    .tp_basicsize = sizeof(PrinterObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Printer_new,
    .tp_init = (initproc) Printer_init,
    .tp_dealloc = (destructor) Printer_dealloc,
    .tp_members = Printer_members,
    .tp_methods = Printer_methods
};

static PyModuleDef thermalmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "thermal",
    .m_doc = "Thermal Printer Module",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_thermal(void);

#endif