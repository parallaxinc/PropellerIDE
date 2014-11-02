#ifndef __P1LOAD_H
#define __P1LOAD_H

#if defined(__cplusplus)
extern "C" {
#endif

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

int  OpenPort(char *port, int baud);
void ClosePort();
int  InitPort(char *prefix, char *port, int baud, int verbose, char *actualport);
int  HardwareFound(int *pVersion);
int  LoadSpinBinary(int loadType, char *bytes, int size);

#if defined(__cplusplus)
}
#endif

#endif
//__P1LOAD_H
