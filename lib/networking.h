/*
 THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF OUTRAGE
 ENTERTAINMENT, INC. ("OUTRAGE").  OUTRAGE, IN DISTRIBUTING THE CODE TO
 END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
 ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
 IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
 SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
 FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
 CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
 AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
 COPYRIGHT 1996-2000 OUTRAGE ENTERTAINMENT, INC.  ALL RIGHTS RESERVED.
 */
 
#ifndef NETWORKING_H
#define NETWORKING_H

#include "pstypes.h"

#if defined(WIN32)
//Windows includes
#include <winsock.h>

#ifdef __cplusplus
//helper macros for working with SOCKADDR_IN to make it look nicer between windows and Linux
inline void INADDR_SET_SUN_SADDR(struct in_addr* st,unsigned int value)
{
	st->S_un.S_addr = value;
}
inline void INADDR_GET_SUN_SADDR(struct in_addr* st,unsigned int *value)
{
	*value = st->S_un.S_addr;
}
inline void INADDR_SET_SUN_SUNW(struct in_addr* st,unsigned short s_w1,unsigned short s_w2)
{
	st->S_un.S_un_w.s_w1 = s_w1;
	st->S_un.S_un_w.s_w2 = s_w2;
}
inline void INADDR_GET_SUN_SUNW(struct in_addr* st,unsigned short *s_w1,unsigned short *s_w2)
{
	*s_w1 = st->S_un.S_un_w.s_w1;
	*s_w2 = st->S_un.S_un_w.s_w2;
}
inline void INADDR_SET_SUN_SUNB(struct in_addr* st,unsigned char s_b1,unsigned char s_b2,unsigned char s_b3,unsigned char s_b4)
{
	st->S_un.S_un_b.s_b1 = s_b1;
	st->S_un.S_un_b.s_b2 = s_b2;
	st->S_un.S_un_b.s_b3 = s_b3;
	st->S_un.S_un_b.s_b4 = s_b4;
}
inline void INADDR_GET_SUN_SUNB(struct in_addr* st,unsigned char *s_b1,unsigned char *s_b2,unsigned char *s_b3,unsigned char *s_b4)
{
	*s_b1 = st->S_un.S_un_b.s_b1;
	*s_b2 = st->S_un.S_un_b.s_b2;
	*s_b3 = st->S_un.S_un_b.s_b3;
	*s_b4 = st->S_un.S_un_b.s_b4;
}
#endif

#elif defined(__LINUX__)
//Linux includes/defines
//#include <linux/types.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef __EMSCRIPTEN__
//#include <linux/ipx.h>
#include <linux/if.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>

#include "linux_lib.h"

#define BOOL bool
#define SOCKET unsigned int
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR_IPX sockaddr_ipx
#define SOCKADDR sockaddr
#define INVALID_SOCKET -1
#define NSPROTO_IPX AF_IPX
#define TRUE true
#define FALSE false
#define HOSTENT struct hostent
#define SOCKET_ERROR -1
#define HANDLE void *

//Winsock = sockets error translation
#define WSAEWOULDBLOCK	EWOULDBLOCK
#define WSAEINVAL			EINVAL
#define WSAENOPROTOOPT	ENOPROTOOPT

inline int WSAGetLastError(){return errno;}
extern bool Use_DirectPlay;

//helper macros for working with SOCKADDR_IN to make it look nicer between windows and Linux
inline void INADDR_SET_SUN_SADDR(struct in_addr* st,unsigned int value)
{
	st->s_addr = value;
}
inline void INADDR_GET_SUN_SADDR(struct in_addr* st,unsigned int *value)
{
	*value = st->s_addr;
}
inline void INADDR_SET_SUN_SUNW(struct in_addr* st,unsigned short s_w1,unsigned short s_w2)
{
	union{
		struct{ unsigned char s_b1,s_b2,s_b3,s_b4;}S_un_b;
		struct{	unsigned short s_w1,s_w2;} S_un_w;
		unsigned int S_addr;
	}S_un;

	S_un.S_un_w.s_w1 = s_w1;
	S_un.S_un_w.s_w2 = s_w2;
	st->s_addr = S_un.S_addr;
}
inline void INADDR_GET_SUN_SUNW(struct in_addr* st,unsigned short *s_w1,unsigned short *s_w2)
{
	union{
		struct{ unsigned char s_b1,s_b2,s_b3,s_b4;}S_un_b;
		struct{	unsigned short s_w1,s_w2;} S_un_w;
		unsigned int S_addr;
	}S_un;

	S_un.S_addr = st->s_addr;
	*s_w1 = S_un.S_un_w.s_w1;
	*s_w2 = S_un.S_un_w.s_w2;
}
inline void INADDR_SET_SUN_SUNB(struct in_addr* st,unsigned char s_b1,unsigned char s_b2,unsigned char s_b3,unsigned char s_b4)
{
	union{
		struct{ unsigned char s_b1,s_b2,s_b3,s_b4;}S_un_b;
		struct{	unsigned short s_w1,s_w2;} S_un_w;
		unsigned int S_addr;
	}S_un;

	S_un.S_un_b.s_b1 = s_b1;
	S_un.S_un_b.s_b2 = s_b2;
	S_un.S_un_b.s_b3 = s_b3;
	S_un.S_un_b.s_b4 = s_b4;
	st->s_addr = S_un.S_addr;
}
inline void INADDR_GET_SUN_SUNB(struct in_addr* st,unsigned char *s_b1,unsigned char *s_b2,unsigned char *s_b3,unsigned char *s_b4)
{
	union{
		struct{ unsigned char s_b1,s_b2,s_b3,s_b4;}S_un_b;
		struct{	unsigned short s_w1,s_w2;} S_un_w;
		unsigned int S_addr;
	}S_un;

	S_un.S_addr = st->s_addr;
	*s_b1 = S_un.S_un_b.s_b1;
	*s_b2 = S_un.S_un_b.s_b2;
	*s_b3 = S_un.S_un_b.s_b3;
	*s_b4 = S_un.S_un_b.s_b4;
}
#elif defined(MACINTOSH)

#include <OpenTransport.h>
#include <OpenTptXti.h>
#include <OpenTptInternet.h>
#include "otsockets.h"

#include "macsock.h"

#define BOOL bool
#define SOCKET unsigned int
#define SOCKADDR_IN sockaddr_in
//#define SOCKADDR_IPX sockaddr_ipx
#define SOCKADDR sockaddr
//#define INVALID_SOCKET -1
//#define NSPROTO_IPX AF_IPX
#define TRUE true
#define FALSE false
#define HOSTENT struct hostent
//#define SOCKET_ERROR -1

//Winsock = sockets error translation
//#define WSAEWOULDBLOCK	EWOULDBLOCK
//#define WSAEINVAL			EINVAL
//#define WSAENOPROTOOPT	ENOPROTOOPT

extern bool Use_DirectPlay;

//#ifdef FIXED
//inline int WSAGetLastError(){return errno;}

//helper macros for working with SOCKADDR_IN to make it look nicer between windows and Linux
inline void INADDR_SET_SUN_SADDR(struct in_addr* st,unsigned int value)
{
	st->S_un.S_addr = value;
}
inline void INADDR_GET_SUN_SADDR(struct in_addr* st,unsigned int *value)
{
	*value = st->S_un.S_addr;
}
inline void INADDR_SET_SUN_SUNW(struct in_addr* st,unsigned short s_w1,unsigned short s_w2)
{
	st->S_un.S_un_w.s_w1 = s_w1;
	st->S_un.S_un_w.s_w2 = s_w2;
}
inline void INADDR_GET_SUN_SUNW(struct in_addr* st,unsigned short *s_w1,unsigned short *s_w2)
{
	*s_w1 = st->S_un.S_un_w.s_w1;
	*s_w2 = st->S_un.S_un_w.s_w2;
}
inline void INADDR_SET_SUN_SUNB(struct in_addr* st,unsigned char s_b1,unsigned char s_b2,unsigned char s_b3,unsigned char s_b4)
{
	st->S_un.S_un_b.s_b1 = s_b1;
	st->S_un.S_un_b.s_b2 = s_b2;
	st->S_un.S_un_b.s_b3 = s_b3;
	st->S_un.S_un_b.s_b4 = s_b4;
}
inline void INADDR_GET_SUN_SUNB(struct in_addr* st,unsigned char *s_b1,unsigned char *s_b2,unsigned char *s_b3,unsigned char *s_b4)
{
	*s_b1 = st->S_un.S_un_b.s_b1;
	*s_b2 = st->S_un.S_un_b.s_b2;
	*s_b3 = st->S_un.S_un_b.s_b3;
	*s_b4 = st->S_un.S_un_b.s_b4;
}
//#endif // FIXED
#else
typedef unsigned int SOCKET;
typedef void *HANDLE;
typedef int BOOL;
#endif	// OS


#define NWT_UNRELIABLE	1
#define NWT_RELIABLE	2


// This is the max size of a packet - DO NOT INCREASE THIS NUMBER ABOVE 512!
#define MAX_PACKET_SIZE	512
#if 1 //ndef DEMO
#define DEFAULT_GAME_PORT		D3_DEFAULT_PORT
#else
#define DEFAULT_GAME_PORT		6250
#endif
// Network flags
#define NF_CHECKSUM	1
#define NF_NOSEQINC	2

typedef enum
{
	NP_NONE,
	NP_TCP,
	NP_IPX,
	NP_DIRECTPLAY
} network_protocol;

typedef struct 
{
	ubyte address[6];
	ushort port;
	ubyte net_id[4];
	network_protocol connection_type;			// IPX, IP, modem, etc.
} network_address;

#ifdef __cplusplus
extern BOOL DP_active;	
extern BOOL TCP_active;
extern BOOL IPX_active;
#endif
//Get the info from RAS
unsigned int psnet_ras_status();

// function to shutdown and close the given socket.  It takes a couple of things into consideration
// when closing, such as possibly reiniting reliable sockets if they are closed here.
#ifdef __cplusplus
void nw_CloseSocket( SOCKET *sockp );
#endif

// Inits the sockets layer to activity
void nw_InitNetworking (int iReadBufSizeOverride
#ifdef __cplusplus
 = -1
#endif
);

// called by psnet_init to initialize the listen socket used by a host/server
int nw_InitReliableSocket();

// function which checks the Listen_socket for possibly incoming requests to be connected.
// returns 0 on error or nothing waiting.  1 if we should try to accept
int nw_CheckListenSocket(network_address *from_addr);

// Inits the sockets that the application will be using
void nw_InitSockets(ushort port);

// Connects a client to a server
void nw_ConnectToServer(uint *socket, network_address *server_addr);

// Returns internet address format from string address format...ie "204.243.217.14"
// turns into 1414829242
unsigned long nw_GetHostAddressFromNumbers (char *str);

// Fills in the string with the string address from the internet address
void nw_GetNumbersFromHostAddress(network_address * address,char *str);

// returns the ip address of this computer
unsigned int nw_GetThisIP();

// function which checks the Listen_socket for possibly incoming requests to be connected.
// returns 0 on error or nothing waiting.  1 if we should try to accept
int nw_CheckListenSocket(network_address *from_addr);


// Calculates a unique ushort checksum for a stream of data
ushort nw_CalculateChecksum( void * vptr, int len );

// Sends data on an unreliable socket
int nw_Send( network_address * who_to, void * data, int len, int flags );


// nw_ReceiveFromSocket will get data out of the socket and stuff it into the packet_buffers
// nw_Receive now calls this function, then determines which of the packet buffers
// to package up and use
void nw_ReceiveFromSocket();


// routine to "free" a packet buffer
void nw_FreePacket( int id );

// nw_Recieve will call the above function to read data out of the socket.  It will then determine
// which of the buffers we should use and pass to the routine which called us
int nw_Receive( void * data, network_address *from_addr );

// nw_SendReliable sends the given data through the given reliable socket.
int nw_SendReliable(unsigned int socketid, ubyte *data, int length,bool urgent
#ifdef __cplusplus
=false
#endif
 );

// function which reads data off of a reliable socket.  recv() should read the totaly amount of data
// available I believe.  (i.e. we shouldn't read only part of a message with one call....I may be wrong
// and this may be a source of bugs).
#ifdef __cplusplus
int nw_ReceiveReliable(SOCKET socket, ubyte *buffer, int max_len);
#endif

// Returns the current protocol in use
int nw_GetProtocolType ();

// Copies my address into the passed argument
void nw_GetMyAddress (network_address *addr);

//Sends a packet to the game tracker
int nw_SendGameTrackerPacker(void *packet);

//Checks for an incoming game tracker packet.
int nw_ReceiveGameTracker(void *packet);

//Send a packet to the pilot tracker
int nw_SendPilotTrackerPacket(void *packet);

//Checks for an incoming pilot tracker packet.
int nw_ReceivePilotTracker(void *packet);

int nw_PingCompare( const void *arg1, const void *arg2 );

// initialize the buffering system
void nw_psnet_buffer_init();

// buffer a packet (maintain order!)
void nw_psnet_buffer_packet(ubyte *data, int length, network_address *from);

// get the index of the next packet in order!
int nw_psnet_buffer_get_next(ubyte *data, int *length, network_address *from);

// get the index of the next packet in order!
int nw_psnet_buffer_get_next_by_dpid(ubyte *data, int *length, unsigned long dpid);

//This is all the reliable UDP stuff...
#define MAXNETBUFFERS			150		//Maximum network buffers (For between network and upper level functions, which is 
										//required in case of out of order packets
#define NETRETRYTIME				.75		//Time after sending before we resend
#define MIN_NET_RETRYTIME		.2
#define NETTIMEOUT				300		//Time after receiving the last packet before we drop that user
#define NETHEARTBEATTIME		10		//How often to send a heartbeat
#define MAXRELIABLESOCKETS		40		//Max reliable sockets to open at once...
#define NETBUFFERSIZE			600	//Max size of a network packet

//Network Types
#define RNT_ACK				1		//ACK Packet
#define RNT_DATA				2		//Data Packet
#define RNT_DATA_COMP		3		//Compressed Data Packet
#define RNT_REQ_CONN			4		//Requesting a connection
#define RNT_DISCONNECT		5		//Disconnecting a connection
#define RNT_HEARTBEAT		6		//Heartbeat -- send every NETHEARTBEATTIME
#define RNT_I_AM_HERE		7

//Reliable socket states
#define RNF_UNUSED			0		//Completely clean socket..
#define RNF_CONNECTED		1		//Connected and running fine
#define RNF_BROKEN			2		//Broken - disconnected abnormally
#define RNF_DISCONNECTED	3		//Disconnected cleanly
#define RNF_CONNECTING		4		//We received the connecting message, but haven't told the game yet.
#define RNF_LIMBO				5		//between connecting and connected

#ifdef __cplusplus
void nw_SendReliableAck(SOCKADDR *raddr,unsigned int sig, network_protocol link_type,float time_sent);
#endif
void nw_WorkReliable(ubyte * data,int len,network_address *naddr);
int nw_Compress(void *srcdata,void *destdata,int count);
int nw_Uncompress(void *compdata,void *uncompdata,int count);

#define NW_AGHBN_CANCEL		1
#define NW_AGHBN_LOOKUP		2
#define NW_AGHBN_READ		3

typedef struct _async_dns_lookup
{
	unsigned int ip;	//resolved host. Write only to worker thread.
	char * host;//host name to resolve. read only to worker thread
	bool done;	//write only to the worker thread. Signals that the operation is complete
	bool error; //write only to worker thread. Thread sets this if the name doesn't resolve
	bool abort;	//read only to worker thread. If this is set, don't fill in the struct.
}async_dns_lookup;

#ifdef WIN32
#define CDECLCALL	__cdecl
#else
#define CDECLCALL
#endif
#ifdef __LINUX__
void *CDECLCALL gethostbynameworker(void *parm);
#else
void CDECLCALL gethostbynameworker(void *parm);
#endif

int nw_Asyncgethostbyname(unsigned int *ip,int command, char *hostname);
int nw_ReccomendPPS();
void nw_DoNetworkIdle(void);

typedef void * ( * NetworkReceiveCallback ) (ubyte *data,int len, network_address *from);
int nw_RegisterCallback(NetworkReceiveCallback nfp, ubyte id);
NetworkReceiveCallback nw_UnRegisterCallback(ubyte id);
int nw_SendWithID(ubyte id,ubyte *data,int len,network_address *who_to);
int nw_DoReceiveCallbacks(void);
void nw_HandleConnectResponse(ubyte *data,int len,network_address *server_addr);
int nw_RegisterCallback(NetworkReceiveCallback nfp, ubyte id);
void nw_HandleUnreliableData(ubyte *data,int len,network_address *from_addr);
void nw_ReliableResend(void);
int nw_CheckReliableSocket(int socknum);

typedef struct
{
	// TCP/IP Status lines
	int udp_total_packets_sent;              // total number of packets sent out on the network (unreliable)
	int udp_total_packets_rec;               // total number of packets recieved on the network (unrealiable)
	int udp_total_bytes_sent;                // total number of bytes sent (unreliable)
	int udp_total_bytes_rec;                 // total number of bytes recieved (unreliable)
	
	int tcp_total_packets_sent;              // total number of packets sent out on the network (reliable)
	int tcp_total_packets_rec;               // total number of packets recieved on the network (reliable)
	int tcp_total_bytes_sent;                // total number of bytes sent (reliable)
	int tcp_total_bytes_rec;                 // total number of bytes recieved (reliable)
	int tcp_total_packets_resent;            // total number of packets resent (reliable)
	int tcp_total_bytes_resent;              // total number of bytes resent (reliable)

	// IPX Status lines
	int ipx_total_packets_sent;              // total number of packets sent out on the network (unreliable)
	int ipx_total_packets_rec;               // total number of packets recieved on the network (unrealiable)
	int ipx_total_bytes_sent;                // total number of bytes sent (unreliable)
	int ipx_total_bytes_rec;                 // total number of bytes recieved (unreliable)
	
	int spx_total_packets_sent;              // total number of packets sent out on the network (reliable)
	int spx_total_packets_rec;               // total number of packets recieved on the network (reliable)
	int spx_total_bytes_sent;                // total number of bytes sent (reliable)
	int spx_total_bytes_rec;                 // total number of bytes recieved (reliable)
	int spx_total_packets_resent;            // total number of packets resent (reliable)
	int spx_total_bytes_resent;              // total number of bytes resent (reliable)
}tNetworkStatus;	// network status information

// fills in the buffer with network stats
// pass NULL to reset the stats
void nw_GetNetworkStats(tNetworkStatus *stats);

#endif
