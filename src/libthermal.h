#ifndef LIBTHERMAL_H
#define LIBTHERMAL_H

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>    // Standard input/output definitions
#include <string.h>   // String function definitions
#include <unistd.h>   // UNIX standard function definitions
#include <fcntl.h>    // File control definitions
#include <errno.h>    // Error number definitions
#include <termios.h>  // POSIX calls for tty I/O control


/**
 * @author    Jonas Drotleff <jonas@drtlf.de>
 * @version   0.0.1
 * @since     0.0.1
 */
class Connection {
    /**
     * Everything is a file. Thus, the serial port is specified as a
     * file path.
     * 
     * @see ::fd for the file descriptor
     */
    std::string path;

    /**
     * The termios struct tty provides an easy interface for POSIX
     * I/O calls. It is used to change flags on the file descriptor.
     */
    struct termios tty;
    int fd;

    /** 
     * Configure TTY
     * Use the termios struct to configure the file descriptor
     */
    void configureTTY(void);

    public:
        Connection(std::string path);
        ~Connection();

        int open(void);
        int close(void);
        
        /**
         * Write message. If the message does not wrap the entire length
         * of the paper, it is probably not going to be printed as the
         * print will be performed once the line wraps.
         *
         * @param msg: Message to be printed
         */
        int write(const std::string msg);
        int write(const char *msg);

        /**
         * Write byte to serial printer
         *
         * @param byte: Single byte send to the printer
         */
        int write(char byte);
        /**
         * Write 3 bytes to serial printer. Many commands (e.g. line
         * feed) require three bytes.
         *
         * @param b1: First byte
         * @param b2: Second byte
         * @param b3: Third byte
         */
        int write(char b1, char b2, char b3);

        /**
         * Print line. Will feed one line after printing the message.
         *
         * @param msg: Message to be printed
         */
        int println(const std::string msg);
        int println(const char *msg);
        /**
         * Feed print paper by n rows. Defaults to one row
         *
         * @param rows: Number of rows
         */
        int feed(int rows = 1);
};

#endif
