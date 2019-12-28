#include "connmgr.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
        printf("starting main \n \n");
        connmgr_listen(1234);
        connmgr_free();
        return 0;
}
//// gcc -Wall -Werror main.c connmgr.h connmgr.c config.h lib/dplist.h lib/dplist.c lib/tcpsock.c  lib/tcpsock.h -D TIMEOUT=5
//valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./a.out