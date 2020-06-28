#include "libthermal.h"



Connection::Connection(std::string path)
{
    this->path = path;
    // Allocate memory for termios struct
	memset(&tty, 0, sizeof tty);
}

Connection::~Connection()
{
    this->close();
}

int Connection::open(void)
{
	fd = ::open(path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        fprintf(stderr, "Error %i: %s (%s)\n", errno, strerror(errno), path.c_str());
        return errno;
    }
    // Take over existing settings from open serial port
    if (tcgetattr(fd, &tty) != 0) {
		fprintf(stderr, "Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return errno;
    }
    configureTTY();
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return errno;
    }
    return 0;
}

int Connection::close(void)
{
    return ::close(fd);
}

int Connection::write(const std::string msg)
{
    return ::write(fd, msg.c_str(), msg.size());
}

int Connection::write(const char *msg)
{
    std::string str(msg);
    // It is way easier to handle the size std::string than it is for
    // char buffers. Thus, we convert the message to a string.
    return this->write(str);
}

int Connection::write(char byte)
{
    char buf[1] = {byte};
    return ::write(fd, buf, 1);
}

int Connection::write(char b1, char b2, char b3)
{
    char buf[3] = {b1, b2, b3};
    return ::write(fd, buf, 3);
}

int Connection::feed(int rows)
{
    return this->write(27, 'd', rows);
}

int Connection::println(const std::string msg)
{
    int res = this->write(msg);
    if (res == -1) return res;
    return this->feed();
}

int Connection::println(const char *msg)
{
    std::string str(msg);
    return this->println(str);
}

void Connection::configureTTY(void)
{
    // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~PARENB;
    // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSTOPB;
    // Use 8 bits per byte
    tty.c_cflag |= CS8;
    // Disable RTS/CTS hardware flow control
    tty.c_cflag &= ~CRTSCTS;
    // Disables software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty.c_lflag &= ~ICANON;
    cfsetispeed(&tty, B9600);
}
