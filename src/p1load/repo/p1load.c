#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "ploader.h"
#include "osint.h"

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

/* port prefix */
#if defined(CYGWIN) || defined(WIN32) || defined(MINGW)
  #define PORT_PREFIX ""
#elif defined(LINUX)
  #ifdef RASPBERRY_PI
    #define PORT_PREFIX "ttyAMA"
  #else
    #define PORT_PREFIX "ttyUSB"
  #endif
#elif defined(MACOSX)
  #define PORT_PREFIX "cu.usbserial-"
#else
  #define PORT_PREFIX ""
#endif

/* defaults */
#define BAUD_RATE   115200

/* CheckPort state structure */
typedef struct {
    int baud;
    int verbose;
    char *actualport;
} CheckPortInfo;

/* CheckPort result codes */
enum {
    CHECK_PORT_OK,
    CHECK_PORT_OPEN_FAILED,
    CHECK_PORT_NO_PROPELLER
};

#define UNUSED(x)

static void cb_reset(void *data)
{
    UNUSED(data); // osint keeps its state in globals
    hwreset();
}

static int cb_tx(void *data, uint8_t* buf, int n)
{
    UNUSED(data); // osint keeps its state in globals
    return tx(buf, n);
}

static int cb_rx_timeout(void *data, uint8_t* buf, int n, int timeout)
{
    UNUSED(data); // osint keeps its state in globals
    return rx_timeout(buf, n, timeout);
}

static PL_serial serial = { cb_reset, cb_tx, cb_rx_timeout };
static PL_state state;
static int version;

static void Usage(void);
static uint8_t *ReadEntireFile(char *name, long *pSize);
static int ShowPort(const char* port, void* data);
static void ShowPorts(char *prefix);
static int CheckPort(const char* port, void* data);
static int InitPort(char *prefix, char *port, int baud, int verbose, char *actualport);
static int OpenPort(const char* port, int baud);

int main(int argc, char *argv[])
{
    char actualPort[PATH_MAX], *var, *val, *port, *p;
    int baudRate, baudRate2, verbose, terminalMode, pstMode, i;
    char *file = NULL;
    int loadType = 0;
    long imageSize;
    uint8_t *image;
    
    /* initialize */
    baudRate = baudRate2 = BAUD_RATE;
    verbose = terminalMode = pstMode = FALSE;
    port = NULL;
    
    /* initialize the loader */
    PL_Init(&state, &serial, NULL);
    
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
            case 'D':
                if (argv[i][2])
                    p = &argv[i][2];
                else if (++i < argc)
                    p = argv[i];
                else
                    Usage();
                if ((var = strtok(p, "=")) != NULL) {
                    if ((val = strtok(NULL, "")) != NULL) {
                        if (strcmp(var, "reset") == 0)
                            use_reset_method(val);
                        else
                            Usage();
                    }
                    else
                        Usage();
                }
                else
                    Usage();
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
                    sprintf(buf, "/dev/%s%d", PORT_PREFIX, atoi(port));
                    port = buf;
#endif
                }
#endif
#if defined(MACOSX)
                if (port[0] != '/') {
                    static char buf[64];
                    sprintf(buf, "/dev/%s-%s", PORT_PREFIX, port);
                    port = buf;
                }
#endif
                break;
            case 'P':
                ShowPorts(PORT_PREFIX);
                break;
            case 'r':
                loadType |= LOAD_TYPE_RUN;
                break;
            case 'T':
                pstMode = TRUE;
                // fall through
            case 't':
                terminalMode = TRUE;
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
            if (PL_LoadSpinBinary(&state, loadType, image, imageSize) != 0)
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

    return 0;
}

/* Usage - display a usage message and exit */
static void Usage(void)
{
printf("\
p1load - a simple loader for the propeller - version 0.009, 2014-08-10\n\
usage: p1load\n\
         [ -b baud ]               baud rate (default is %d)\n\
         [ -D var=val ]            set variable value\n\
         [ -e ]                    write a bootable image to EEPROM\n\
         [ -p port ]               serial port (default is to auto-detect the port)\n\
         [ -P ]                    list available serial ports\n\
         [ -r ]                    run the program after loading\n\
         [ -t ]                    enter terminal mode after running the program\n\
         [ -T ]                    enter PST-compatible terminal mode\n\
         [ -v ]                    verbose output\n\
         [ -? ]                    display a usage message and exit\n\
         file                      file to load\n", BAUD_RATE);
#ifdef RASPBERRY_PI
printf("\
\n\
This version supports resetting the Propeller with a GPIO pin with option: -Dreset=gpio,pin,level\n\
where \"pin\" is the GPIO number to use and \"level\" is the logic level, 0 or 1. \n\
");
#endif
    exit(1);
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

static int InitPort(char *prefix, char *port, int baud, int verbose, char *actualport)
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

static int OpenPort(const char* port, int baud)
{
    /* open the port */
    if (serial_init(port, baud) == 0)
        return CHECK_PORT_OPEN_FAILED;
        
    /* check for a propeller on this port */
    if (!PL_HardwareFound(&state, &version)) {
        serial_done();
        return CHECK_PORT_NO_PROPELLER;
    }

    return CHECK_PORT_OK;
}
