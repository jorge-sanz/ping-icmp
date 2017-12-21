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
#include "ip-icmp-ping.h"

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
    int udp_socket;                    /* udp socket */
    struct sockaddr_in remote_address; /* external machine information */
    int response;                      /* ping response */
    ECHORequest echoRequest;
    ECHOResponse echoResponse;

    printf("-- Welcome to TFTP client --\n");
    checkProgramArguments(argc, argv);

    /* create UDP socket */
    if ((udp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        error("ERROR opening UDP socket");
    }

    /* create remote address structure */
    remote_address.sin_addr.s_addr = inet_addr(argv[1]);
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = 0;

    /* fill ICMP datagram */
    bzero(&echoRequest, sizeof(echoRequest)); /* erase data */
    echoRequest.ID = getpid();
    echoRequest.SeqNumber = 0;
    strcpy(echoRequest.payload, "Payload.");
    echoRequest.icmpHeader.Type = 8;
    echoRequest.icmpHeader.Code = 0;
    echoRequest.icmpHeader.Checksum = 0;

    /* calculate checksum */
    int numShorts = sizeof(echoRequest);
    unsigned short int *pointer;
    pointer = (unsigned short int *)&echoRequest;
    unsigned int accumulator = 0;
    int i = 0;

    for (i = 0; i <= (numShorts - 1); i = i + 2)
    {
        accumulator = accumulator + (unsigned int)*pointer;
        pointer++;
    }

    accumulator = (accumulator >> 16) + (accumulator & 0x0000ffff);
    accumulator = (accumulator >> 16) + (accumulator & 0x0000ffff);
    accumulator = ~accumulator;

    /* assign the checksum */
    echoRequest.icmpHeader.Checksum = (unsigned short int)accumulator;

    /* print echoRequest */
    if (v_mode)
    {
        printf("-> Generating ICMP header\n");
        printf("-> Type: %d\n", echoRequest.icmpHeader.Type);
        printf("-> Code: %d\n", echoRequest.icmpHeader.Code);
        printf("-> Identifier (pid): %d\n", echoRequest.ID);
        printf("-> Seq. number: %d\n", echoRequest.SeqNumber);
        printf("-> String to be sent:  %s\n", echoRequest.payload);
        printf("-> Checksum 0x%x\n", echoRequest.icmpHeader.Checksum);
        printf("-> ICMP package size: %ld\n", sizeof(echoRequest));
    }

    /* send */
    if ((sendto(udp_socket, &echoRequest, sizeof(echoRequest), 0, (struct sockaddr *)&remote_address, sizeof(struct sockaddr_in))) < 0)
    {
        error("ERROR in sendto");
    }

    printf("ICMP package sent to %s\n", argv[1]);

    int size = sizeof(struct sockaddr_in);

    /* receive response */
    if ((response = recvfrom(udp_socket, &echoResponse, sizeof(echoResponse), 0, (struct sockaddr *)&remote_address, (socklen_t *)&size)) < 0)
    {
        error("ERROR in recvfrom");
    }

    printf("Received response from %s\n", argv[1]);

    /* print response information */
    if (v_mode)
    {
        printf("-> Response size: %d\n", response);
        printf("-> Received string: %s\n", echoResponse.payload);
        printf("-> Identifier (pid): %d\n", echoResponse.ID);
        printf("-> TTL: %d\n", echoResponse.ipHeader.TTL);
    }

    /* print response description */
    printf("Response description: ");

    switch (echoResponse.icmpHeader.Type)
    {
    case 0:
        printf("Echo reply(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 1 ... 2:
        printf("Reserved(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 3:
        switch (echoResponse.icmpHeader.Code)
        {
        case 0:
            printf("Destination network unreachable(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 1:
            printf("Destination host unreachable(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 2:
            printf("Destination protocol unreachable(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 3:
            printf("Destination port unreachable(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 4:
            printf("Fragmentation required, and DF flag set(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 5:
            printf("Source route failed(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 6:
            printf("Destination network unknown(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 7:
            printf("Destination host unknown(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 8:
            printf("Source host isolated(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 9:
            printf("Network administratively prohibited(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 10:
            printf("Host administratively prohibited(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 11:
            printf("Network unreachable for TOS(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 12:
            printf("Host unreachable for TOS(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 13:
            printf("Communication administratively prohibited(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 14:
            printf("Host Precedence Violation(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 15:
            printf("Precedence cutoff in effect(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        default:
            break;
        }
        break;
    case 4:
        printf("Source quench (congestion control)(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 5:
        switch (echoResponse.icmpHeader.Code)
        {
        case 0:
            printf("Redirect Datagram for the Network(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 1:
            printf("Redirect Datagram for the Host(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 2:
            printf("Redirect Datagram for the TOS & network(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 3:
            printf("Redirect Datagram for the TOS & host(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        default:
            break;
        }
        break;
    case 6:
        printf("Alternate Host Address(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 7:
        printf("Reserved(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 8:
        printf("Echo request(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 9:
        printf("Router Advertisement(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 10:
        printf("Router discovery/selection/solicitation(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 11:
        switch (echoResponse.icmpHeader.Code)
        {
        case 0:
            printf("TTL expired in transit(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 1:
            printf("Fragment reassembly time exceeded(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        default:
            break;
        }
        break;
    case 12:
        switch (echoResponse.icmpHeader.Code)
        {
        case 0:
            printf("Pointer indicates the error(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 1:
            printf("Missing a required option(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        case 2:
            printf("Bad length(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
            break;
        default:
            break;
        }
        break;
    case 13:
        printf("Timestamp(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 14:
        printf("Timestamp reply(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 15:
        printf("Information Request(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 16:
        printf("Information Reply(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 17:
        printf("Address Mask Request(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 18:
        printf("Address Mask Reply(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 19:
        printf("Reserved for security(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 20 ... 29:
        printf("Reserved for robustness experiment(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 30:
        printf("information Request(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 31:
        printf("Datagram Conversion Error(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 32:
        printf("Mobile Host Redirect(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 33:
        printf("Where-Are-You (originally meant for IPv6)(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 34:
        printf("Here-I-Am (originally meant for IPv6)(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 35:
        printf("Mobile Registration Request(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 36:
        printf("Mobile Registration Reply(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 37:
        printf("Domain Name Request(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 38:
        printf("Domain Name Reply(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 39:
        printf("SKIP Algorithm Discovery Protocol, Simple Key-Management for Internet Protocol(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 40:
        printf("Photuris, Security failures(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 41:
        printf("ICMP for experimental mobility protocols such as Seamoby [RFC4065](type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    case 42 ... 255:
        printf("Reserved(type %d, code %d).\n", echoResponse.icmpHeader.Type, echoResponse.icmpHeader.Code);
        break;
    default:
        break;
    }

    /* close socket */
    if (close(udp_socket) < 0)
    {
        error("ERROR closing UDP socket");
    }
    /* finish program */
    exit(EXIT_SUCCESS);
}
