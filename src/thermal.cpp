#include <iostream>
#include "libthermal.h"


int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <port> <message>\n", argv[0]);
        return EINVAL;
    }
    Connection con(argv[1]);
    int res = con.open();
    if (res != 0) {
        exit(res);
    }
    con.println(argv[2]);
}
