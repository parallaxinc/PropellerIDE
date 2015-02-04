#ifndef __PLOADER_H__
#define __PLOADER_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdint.h>

#define LOAD_TYPE_SHUTDOWN      0
#define LOAD_TYPE_RUN           1
#define LOAD_TYPE_EEPROM        2
#define LOAD_TYPE_EEPROM_RUN    3

/* serial port interface - filled in by the user prior to calling PL_Init */
typedef struct {
    void (*reset)(void *data);
    int (*tx)(void *data, uint8_t* buf, int n);
    int (*rx_timeout)(void *data, uint8_t* buf, int n, int timeout);
} PL_serial;

/* loader state structure - filled in by the loader functions */
typedef struct {
    PL_serial *serial;
    void *serialData;
    uint8_t txbuf[1024];
    int txcnt;
    uint8_t rxbuf[1024];
    int rxnext;
    int rxcnt;
    uint8_t lfsr;
} PL_state;

/* PL_Init - Initializes the loader state structure. */
void PL_Init(PL_state *state, PL_serial *serial, void *data);

/* PL_HardwareFound - Sends the handshake sequence and returns non-zero if a Propeller
   chip is found on the serial interface and also sets the version parameter to the
   chip version.
*/
int PL_HardwareFound(PL_state *state, int *pVersion);

/* PL_LoadSpinBinary - Loads a Spin binary image. Must be called immediatel following a
   successful call to PL_HardwareFound.
*/
int PL_LoadSpinBinary(PL_state *state, int loadType, uint8_t *image, int size);

#ifdef __cplusplus
}
#endif

#endif
