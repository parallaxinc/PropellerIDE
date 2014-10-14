#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "osint.h"
#include "p1load.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

/* port prefix */
#if defined(CYGWIN) || defined(WIN32) || defined(MINGW)
  #define PORT_PREFIX ""
#elif defined(LINUX)
  #define PORT_PREFIX "ttyUSB"
#elif defined(MACOSX)
  #define PORT_PREFIX "cu.usbserial"
#else
  #define PORT_PREFIX ""
#endif

/* defaults */
#define BAUD_RATE   115200
#define ACK_TIMEOUT 50

/* CheckPort state structure */
typedef struct {
    int baud;
    int verbose;
    char *actualport;
} CheckPortInfo;

#if 0
/* CheckPort result codes */
enum {
    CHECK_PORT_OK,
    CHECK_PORT_OPEN_FAILED,
    CHECK_PORT_NO_PROPELLER
};

#define LOAD_TYPE_SHUTDOWN      0
#define LOAD_TYPE_RUN           1
#define LOAD_TYPE_EEPROM        2
#define LOAD_TYPE_EEPROM_RUN    3
#endif

//static int LoadSpinBinary(int loadType, uint8_t *image, int size);
static uint8_t *ReadEntireFile(char *name, long *pSize);
static int ShowPort(const char* port, void* data);
static void ShowPorts(char *prefix);
static int CheckPort(const char* port, void* data);
//static int InitPort(char *prefix, char *port, int baud, int verbose, char *actualport);
//static int OpenPort(const char* port, int baud);
//static int HardwareFound(int *pVersion);

static int version;

static uint8_t txbuf[1024];
static int txcnt;
static uint8_t rxbuf[1024];
static int rxnext, rxcnt;
static uint8_t lfsr;

static void SerialInit(void);
static void TByte(uint8_t x);
static void TLong(uint32_t x);
static void TComm(void);
static int RBit(int timeout);
static int IterateLFSR(void);

#ifdef STANDALONE
static void Usage(void);
int main(int argc, char *argv[])
{
    char actualPort[PATH_MAX], *port, *p;
    int baudRate, baudRate2, verbose, terminalMode, pstMode, i;
    char *file = NULL;
    int loadType = 0;
    long imageSize;
    uint8_t *image;
    int cat = 1;
    
    /* initialize */
    baudRate = baudRate2 = BAUD_RATE;
    verbose = terminalMode = pstMode = FALSE;
    port = NULL;
    
    /* process the position-independent arguments */
    for (i = 1; i < argc; ++i) {

        /* handle switches */
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'b':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                else
                    Usage();
                if (*p != ':')
                    baudRate = baudRate2 = atoi(p);
                if ((p = strchr(p, ':')) != NULL) {
                    if (*++p != ':' && *p != '\0')
                        baudRate2 = atoi(p);
                }
                break;
            case 'e':
                loadType |= LOAD_TYPE_EEPROM;
                break;
            case 'p':
                if (argv[i][2])
                    port = &argv[i][2];
                else if (++i < argc)
                    port = argv[i];
                else
                    Usage();
#if defined(CYGWIN) || defined(WIN32) || defined(LINUX)
                if (isdigit((int)port[0])) {
#if defined(CYGWIN) || defined(WIN32)
                    static char buf[10];
                    sprintf(buf, "COM%d", atoi(port));
                    port = buf;
#endif
#if defined(LINUX)
                    static char buf[64];
                    sprintf(buf, "/dev/ttyUSB%d", atoi(port));
                    port = buf;
#endif
                }
#endif
#if defined(MACOSX)
                if (port[0] != '/') {
                    static char buf[64];
                    sprintf(buf, "/dev/cu.usbserial-%s", port);
                    port = buf;
                }
#endif
                break;
            case 'P':
                ShowPorts(PORT_PREFIX);
                cat = 0;
                break;
            case 'r':
                loadType |= LOAD_TYPE_RUN;
                cat = 1;
                break;
            case 'T':
                pstMode = TRUE;
                // fall through
            case 't':
                terminalMode = TRUE;
                cat = 0;
                break;
            case 'v':
                verbose = TRUE;
                break;
            case '?':
                /* fall through */
            default:
                Usage();
                break;
            }
        }
    
        /* handle the input filename */
        else {
            if (file)
                Usage();
            file = argv[i];
        }
    }
    
    switch (InitPort(PORT_PREFIX, port, baudRate, verbose, actualPort)) {
    case CHECK_PORT_OK:
        printf("Found propeller version %d on %s\n", version, actualPort);
        break;
    case CHECK_PORT_OPEN_FAILED:
        printf("error: opening serial port '%s'\n", port);
        perror("Error is ");
        return 1;
    case CHECK_PORT_NO_PROPELLER:
        if (port)
            printf("error: no propeller chip on port '%s'\n", port);
        else
            printf("error: can't find a port with a propeller chip\n");
        return 1;
    }
    
    if (file) {
        if ((image = ReadEntireFile(file, &imageSize)) != NULL) {
            printf("Loading '%s' (%ld bytes)\n", file, imageSize);
            if (LoadSpinBinary(loadType, image, imageSize) != 0)
                printf("Load failed!\n");
        }
    }
    
    /* enter terminal mode if requested */
    if (terminalMode) {
        printf("[ Entering terminal mode. Type ESC or Control-C to exit. ]\n");
        fflush(stdout);
        if (baudRate2 != baudRate)
            serial_baud(baudRate2);
        terminal_mode(FALSE, pstMode);
    }

#if 1
    if(cat) {
        uint8_t buf[20];
        int count = 0;
        count = rx_timeout(buf, 1, 500);
        if(count != SERIAL_TIMEOUT) {
            puts("RECEIVED");
        }
        while(count != SERIAL_TIMEOUT) {
            fputc(*buf,stdout);
            count = rx_timeout(buf, 1, 50);
        }
        fflush(stdout);
        serial_done();
    }
#endif

    return 0;
}

/* Usage - display a usage message and exit */
static void Usage(void)
{
printf("\
p2load - a loader for the propeller 2 - version 0.008, 2013-04-23\n\
usage: p2load\n\
         [ -b baud ]               baud rate (default is %d)\n\
         [ -p port ]               serial port (default is to auto-detect the port)\n\
         [ -e ]                    write a bootable image to EEPROM\n\
         [ -P ]                    list available serial ports\n\
         [ -r ]                    run the program after loading\n\
         [ -t ]                    enter terminal mode after running the program\n\
         [ -T ]                    enter PST-compatible terminal mode\n\
         [ -v ]                    verbose output\n\
         [ -? ]                    display a usage message and exit\n\
         file[,addr]...            files to load\n", BAUD_RATE);
    exit(1);
}
#endif

/* LoadSpinBinary - load a spin binary using the rom loader */
int LoadSpinBinary(int loadType, char *bytes, int size)
{
    int retries = 100;
    uint8_t buf[1];
    uint8_t *image = (uint8_t*) bytes;
    int i;
        
    TLong(loadType);
    TLong(size / sizeof(uint32_t));
    
    /* download the spin binary */
    for (i = 0; i < size; i += 4) {
        uint32_t data = image[i] | (image[i + 1] << 8) | (image[i + 2] << 16) | (image[i + 3] << 24);
        TLong(data);
    }
    TComm();
    usleep(1000);
    
    /* wait for an ACK */
    while (--retries >= 0) {
        TByte(0xf9);
        TComm();
        //fprintf(stderr,"Bit %02x\n", buf[0]);
        if (rx_timeout(buf, 1, ACK_TIMEOUT) <= 0)
            continue;
        //printf("Bit %02x\n", buf[0]);
        if (buf[0] == 0xfe)
            break;
    }
    
    return retries >= 0 ? 0 : -1;
}

/* ReadEntireFile - read an entire file into an allocated buffer */
static uint8_t *ReadEntireFile(char *name, long *pSize)
{
    uint8_t *buf;
    long size;
    FILE *fp;

    /* open the file in binary mode */
    if (!(fp = fopen(name, "rb")))
        return NULL;

    /* get the file size */
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    
    /* allocate a buffer for the file contents */
    if (!(buf = (uint8_t *)malloc(size))) {
        fclose(fp);
        return NULL;
    }
    
    /* read the contents of the file into the buffer */
    if (fread(buf, 1, size, fp) != size) {
        free(buf);
        fclose(fp);
        return NULL;
    }
    
    /* close the file ad return the buffer containing the file contents */
    *pSize = size;
    fclose(fp);
    return buf;
}

static int ShowPort(const char* port, void* data)
{
    printf("%s\n", port);
    return 1;
}

static void ShowPorts(char *prefix)
{
    serial_find(prefix, ShowPort, NULL);
}

static int CheckPort(const char* port, void* data)
{
    CheckPortInfo* info = (CheckPortInfo*)data;
    int rc;
    if (info->verbose)
        printf("Trying %s                    \n", port); fflush(stdout);
    if ((rc = OpenPort(port, info->baud)) != CHECK_PORT_OK)
        return rc;
    if (info->actualport) {
        strncpy(info->actualport, port, PATH_MAX - 1);
        info->actualport[PATH_MAX - 1] = '\0';
    }
    return 0;
}

int InitPort(char *prefix, char *port, int baud, int verbose, char *actualport)
{
    int result;
    
    if (port) {
        if (actualport) {
            strncpy(actualport, port, PATH_MAX - 1);
            actualport[PATH_MAX - 1] = '\0';
        }
        result = OpenPort(port, baud);
    }
    else {
        CheckPortInfo info;
        info.baud = baud;
        info.verbose = verbose;
        info.actualport = actualport;
        if (serial_find(prefix, CheckPort, &info) == 0)
            result = CHECK_PORT_OK;
        else
            result = CHECK_PORT_NO_PROPELLER;
    }
        
    return result;
}

int OpenPort(char* port, int baud)
{
    /* open the port */
    if (serial_init(port, baud) == 0)
        return CHECK_PORT_OPEN_FAILED;
        
    /* check for a propeller on this port */
    if (!HardwareFound(&version)) {
        serial_done();
        return CHECK_PORT_NO_PROPELLER;
    }

    return CHECK_PORT_OK;
}

void ClosePort()
{
    serial_done();
}

/* this code is adapted from Chip Gracey's PNut IDE */

int HardwareFound(int *pVersion)
{
    int version, i;
    
    /* reset the propeller */
    hwreset();
    
    /* initialize the serial buffers */
    SerialInit();
    
    /* send the connect string + blanks for echoes */
    TByte(0xf9);
    lfsr = 'P';
    for (i = 0; i < 250; ++i)
        TByte(IterateLFSR() | 0xfe);
    for (i = 0; i < 250 + 8; ++i)
        TByte(0xf9);
    TComm();
    
    /* receive the connect string */
    for (i = 0; i < 250; ++i)
        if (RBit(100) != IterateLFSR()) {
            fprintf(stderr,"error: hardware lost\n");
            return FALSE;
        }
        
    /* receive the chip version */
    for (version = i = 0; i < 8; ++i) {
        int bit = RBit(50);
        if (bit < 0) {
            fprintf(stderr,"error: hardware lost\n");
            return FALSE;
        }
        version = ((version >> 1) & 0x7f) | (bit << 7);
    }
    *pVersion = version;
        
    /* return successfully */
    return TRUE;
}

/* SerialInit - initialize the serial buffers */
static void SerialInit(void)
{
    txcnt = rxnext = rxcnt = 0;
}

/* TByte - add a byte to the transmit buffer */
static void TByte(uint8_t x)
{
    txbuf[txcnt++] = x;
    if (txcnt >= sizeof(txbuf))
        TComm();
}

/* TLong - add a long to the transmit buffer */
static void TLong(uint32_t x)
{
    int i;
    for (i = 0; i < 11; ++i) {

#if 0
        // p2 code
        uint8_t byte = 0x92
                     | ((i == 10 ? -1 : 0) & 0x60)
                     | ((x >> 31) & 1)
                     | (((x >> 30) & 1) << 3)
                     | (((x >> 29) & 1) << 6);
        TByte(byte);
        x <<= 3;
#else
        // p1 code
        uint8_t byte = 0x92
                     | ((i == 10 ? -1 : 0) & 0x60)
                     |  (x & 1)
                     | ((x & 2) << 2)
                     | ((x & 4) << 4);
        TByte(byte);
        x >>= 3;
#endif

    }
}

/* TComm - write the transmit buffer to the port */
static void TComm(void)
{
    tx(txbuf, txcnt);
    txcnt = 0;
}

/* RBit - receive a bit with a timeout */
static int RBit(int timeout)
{
    int result;
    for (;;) {
        if (rxnext >= rxcnt) {
            rxcnt = rx_timeout(rxbuf, sizeof(rxbuf), timeout);
            if (rxcnt <= 0) {
                /* hardware lost */
                return -1;
            }
            rxnext = 0;
        }
        result = rxbuf[rxnext++] - 0xfe;
        if ((result & 0xfe) == 0)
            return result;
        /* checksum error */
    }
}

/* IterateLFSR - get the next bit in the lfsr sequence */
static int IterateLFSR(void)
{
    int result = lfsr & 1;
    lfsr = ((lfsr << 1) & 0xfe) | (((lfsr >> 7) ^ (lfsr >> 5) ^ (lfsr >> 4) ^ (lfsr >> 1)) & 1);
    return result;
}

/* end of code adapted from Chip Gracey's PNut IDE */

#if defined(__cplusplus)
}
#endif
