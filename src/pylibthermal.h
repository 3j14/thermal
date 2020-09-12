#ifndef LIBTHERMAL_H
#define LIBTHERMAL_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

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

#endif
