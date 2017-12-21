
// ------------------------------------------------------
// Fichero de cabecera ip-icmp-ping.h
// Autor: Diego Llanos, diego@infor.uva.es.
// V2.0, fecha: Fri Jan 16 12:30:23 CET 2015
// ------------------------------------------------------
// Este fichero contiene las estructuras de datos que definen:
//    - La cabecera IP.
//    - La cabecera ICMP.
//    - El datagrama ICMP de solicitud para hacer un ping.
//    - El datagrama ICMP para recibir la respuesta del ping.
// ------------------------------------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define REQ_DATASIZE 64 // Tamaño del payload.

// ------------------------------------------------------
// IP Header
typedef struct tagIPHDR
{
	unsigned char VIHL;		 // Ver, Hdr length
	unsigned char TOS;		 // Type of service
	short TotLen;			 // Total length
	short ID;				 // Identification
	short FlagOff;			 // Flags, Frag off
	unsigned char TTL;		 // Time - to - live
	unsigned char Protocol;  // Protocol
	unsigned short Checksum; // Checksum
	struct in_addr iaSrc;	// Source IP addr
	struct in_addr iaDst;	// Dest IP addr
} IPHeader;

// ------------------------------------------------------
// ICMP Header
typedef struct tagICMPHeader
{
	unsigned char Type;
	unsigned char Code;
	unsigned short int Checksum;
} ICMPHeader;

// ------------------------------------------------------
// ICMP PING request segment
typedef struct tagEchoRequest
{
	ICMPHeader icmpHeader;
	unsigned short ID;
	unsigned short SeqNumber;
	char payload[REQ_DATASIZE];
} ECHORequest;

// ------------------------------------------------------
// ICMP ping response segment
typedef struct tagEchoResponse
{
	IPHeader ipHeader;
	ICMPHeader icmpHeader;
	unsigned short ID;
	unsigned short SeqNumber;
	char payload[REQ_DATASIZE];
} ECHOResponse;

// Para utilizarlos, declarar las siguientes variables dentro
// del main():
//    ECHORequest echoRequest;
//    ECHOResponse echoResponse;
