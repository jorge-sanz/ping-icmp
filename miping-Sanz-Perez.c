// Practica tema 8, Sanz Perez Jorge

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int v_mode = 0; /* 1 for v mode active, which means that the program will print its trace in standard output. */

/* error: wrapper for perror */
void error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* checkProgramAguments */
void checkProgramArguments(int argc, char *argv[])
{
    if (argc == 3)
    {
        /* if there are 3 arguments and the 2nd one is -v then v mode is active */
        if (strcmp(argv[2], "-v") == 0)
        {
            v_mode = 1;
            printf("V mode is active: trace will be shown during the execution.\n");
        }
    }
    /* exit failure. program should have 2 arguments at least. */
    else if (argc < 2)
    {
        fprintf(stderr,
                "Usage: %s ip-address [-v]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    int udp_socket; /* udp socket */

    printf("-- Welcome to TFTP client --\n");
    checkProgramArguments(argc, argv);

    /* create UDP socket */
    if ((udp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        error("ERROR opening UDP socket");
    }

    /* close socket */
    if (close(udp_socket) < 0)
    {
        error("ERROR closing UDP socket");
    }
    exit(EXIT_SUCCESS);
}
