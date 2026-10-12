/************************************************************************

 $Id: IPX.H 1.8 1996/06/24 17:33:04 MaxWolf Exp MaxWolf $

************************************************************************/

#ifndef __IPX_H__
#define __IPX_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__TURBOC__) || defined(__BORLANDC__)
typedef unsigned long DWORD;
typedef unsigned int WORD;
typedef unsigned char BYTE;
#define FAR far
#define CDECL
#define GET_SEG FP_SEG
#define GET_OFF FP_OFF
#define TO_IPX_ADDR(p) p
#define IPXMemAlloc malloc
#define IPXMemFree free

#elif defined(__WATCOMC__)
#pragma pack(1);
#ifdef __386__
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
#define int86 int386
#define int86x call_RMode
#define FAR
#define CDECL cdecl
#define GET_SEG(p) ((((unsigned int)(p)) >> 4) & 0xffff)
#define GET_OFF(p) ((unsigned int)(p) & 0xf)

#ifdef IPX_DEBUG
#define TO_IPX_ADDR(p)  ( (int)p < 0x100000 ?  ((void*)((GET_SEG(p) << 16) | GET_OFF(p))) : \
        ((void*)printf("Wrong IPX address in %s(%d)\n", __FILE__, __LINE__), (void*)fflush(stdout), exit(1), (void*)p) )
#else
#define TO_IPX_ADDR(p) ((void*)((GET_SEG(p) << 16) | GET_OFF(p)))
#endif

void* IPXMemAlloc(size_t size);
void IPXMemFree(void* ptr);

#else // WATCOM 16-bit
typedef unsigned long DWORD;
typedef unsigned int WORD;
typedef unsigned char BYTE;
#define FAR far
#define CDECL cdecl
#define GET_SEG FP_SEG
#define GET_OFF FP_OFF
#define TO_IPX_ADDR(p) p
#define IPXMemAlloc malloc
#define IPXMemFree free

#endif

#else
#error "Unknown compiler"
#endif

/*
 * Be carefull with 'a' !!! There will be no warnings if it is not an address !!!
 * f.e. getWORD(data[i]) instead of getWORD(&data[i])
 */
#define getWORD(a)      (*(WORD*)(a))
#define	getDWORD(a)		(*(DWORD*)(a))
#define putWORD(a,n)	*(WORD*)(a)=n
#define putDWORD(a,n)	*(DWORD*)(a)=n

typedef struct {	/* everything high-low */
	BYTE network[4];
	BYTE node[6];
	WORD socket;
} IPXAddress;

typedef struct {	/* everything high-low */
	WORD		checkSum;
	WORD		length;
	BYTE		transportControl;
	BYTE		packetType;
	IPXAddress	destination;	/* user fills on Send */
	IPXAddress	source;		/* user fills on Send */
} IPXHeader;


typedef struct {	/* describe a fragment buffer */
    void FAR    *address;
	WORD		size;
} ECBFragment;

typedef struct ecb_s {
    void FAR    *linkAddress;
    void (CDECL FAR *ESRAddress)(struct ecb_s FAR *);   /* user fills on Send,Listen,Timer Event */
	volatile BYTE inUseFlag;
	BYTE		completionCode;
	WORD		socketNumber;			/* hi-lo, user fills on Send */
	BYTE		IPXWorkspace[4];
	BYTE		driverWorkspace[12];
	BYTE		immediateAddress[6];	/* hi-lo, user fills on Send */
	WORD		fragmentCount;			/* user fills on Send,Listen */
	ECBFragment	fragmentDescriptor[4];	/* user fills on Send,Listen */
} ECB;

WORD CDECL wordswap(WORD n);
DWORD CDECL dwordswap(DWORD n);

//
// Every ESR handler should conform the following prototype
//
typedef void FAR CDECL ESRFUNC(ECB FAR *);

//void CDECL FreeCallBack(void (CDECL FAR *Esr)());
//void CDECL FAR * CDECL RegisterCallBack(void (CDECL FAR *Esr)());

void CDECL FreeCallBack(ESRFUNC *Esr);
ESRFUNC * CDECL RegisterCallBack(ESRFUNC *Esr, DWORD size);

/* Also declared (as functions or as macros):
void* IPXMemAlloc(size_t size);
void IPXMemFree(void* ptr);
*/

/*
 * =========== IPX interface
 */

int CDECL IPXInstalled(void);                             /* non-zero -> OK */
void CDECL IPXGetInternetworkAddress(IPXAddress *addr);


#define LONG_LIVED  0xff
#define SHORT_LIVED 0x00
int CDECL IPXOpenSocket(WORD type, WORD *socket);


void CDECL IPXCloseSocket(WORD *socket);
void CDECL IPXListenForPacket(ECB *RxECB);
void CDECL IPXSendPacket(ECB *TxECB);
void CDECL IPXRelinquishControl(void);
int CDECL IPXGetLocalTarget(IPXAddress *addr, BYTE ImmAddr[6]);
WORD CDECL IPXGetIntervalMarker(void);

void CDECL IPXScheduleEvent(WORD delay, ECB *asrECB);
int CDECL IPXCancelEvent(ECB *asrECB);

/*
 * =========== SPX interface
 */

typedef struct {
    int         verMajor, verMinor;
    int         totalConnections;
    int         availConnections;
} SPXInfo;

typedef struct {	/* everything high-low */
	WORD		checkSum;
	WORD		length;
	BYTE		transportControl;
	BYTE		packetType;
	IPXAddress	destination;	/* user fills on Send */
	IPXAddress	source;		/* user fills on Send */
	BYTE		connControl;
	BYTE		dataStreamType;
	WORD		sourceConnID;
	WORD		destinationConnID;
	WORD		seqNumber;
	WORD		ackNumber;
	WORD		allocNumber;
} SPXHeader;


int CDECL SPXCheckInstallation(SPXInfo *iptr);            /* non-zero -> OK */
void CDECL SPXListenForConnection(ECB *ecb, int retryCnt, int watchDog);
int CDECL SPXEstablishConnection(ECB *ecb, WORD *connID, int retryCnt, int watchDog);
void CDECL SPXListenForPacket(ECB *ecb);
void CDECL SPXSendPacket(ECB *ecb, WORD connID);
void CDECL SPXTerminateConnection(ECB *ecb, WORD connID);
void CDECL SPXAbortConnection(WORD connID);


#ifdef __WATCOMC__
#pragma pack();
#endif

#ifdef __cplusplus
};
#endif

#endif