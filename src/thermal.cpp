#include "pylibthermal.h"
#include <iostream>

int
main(int argc, char* argv[])
{
  if (argc != 3) {
    fprintf(stderr, "usage: %s <port> <message>\n", argv[0]);
    return EINVAL;
  }
  std::string path(argv[1]);
  int fd = open_serial(path, 3900);
  std::cout << fd << std::endl;
  std::string text(argv[2]);
  write_serial(fd, text);
  write_serial(fd, 27, 'd', 2); // feed by 2
  close_serial(fd);
}
