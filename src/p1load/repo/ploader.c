#ifdef MINGW
#include <unistd.h>
#else
#include <time.h>
#endif
#include "ploader.h"

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#define ACK_TIMEOUT 20

static int us_sleep(unsigned long usecs);
static void SerialInit(PL_state *state);
static void TByte(PL_state *state, uint8_t x);
static void TLong(PL_state *state, uint32_t x);
static void TComm(PL_state *state);
static int RBit(PL_state *state, int timeout);
static int IterateLFSR(PL_state *state);

void PL_Init(PL_state *state, PL_serial *serial, void *data)
{
    state->serial = serial;
    state->serialData = data;
}

/* PL_LoadSpinBinary - load a spin binary using the rom loader */
int PL_LoadSpinBinary(PL_state *state, int loadType, uint8_t *image, int size)
{
    int retries = 100;
    uint8_t buf[1];
    int i;
    
    TLong(state, loadType);
    TLong(state, size / sizeof(uint32_t));
    
    /* download the spin binary */
    for (i = 0; i < size; i += 4) {
        uint32_t data = image[i] | (image[i + 1] << 8) | (image[i + 2] << 16) | (image[i + 3] << 24);
        TLong(state, data);
    }
    TComm(state);
    us_sleep(1000);
    
    /* wait for an ACK */
    while (--retries >= 0) {
        TByte(state, 0xf9);
        TComm(state);
        if ((*state->serial->rx_timeout)(state->serialData, buf, 1, ACK_TIMEOUT) <= 0)
            continue;
        if (buf[0] == 0xfe)
            break;
    }
    
    /* wait for eeprom programming and verification */
    if (loadType == LOAD_TYPE_EEPROM || loadType == LOAD_TYPE_EEPROM_RUN) {
        /* BUG: need to add handling of ACK/NAK from EEPROM programming */
        /* BUG: need to add handling of ACK/NAK from EEPROM verification */
    }
    
    return retries >= 0 ? 0 : -1;
}

static int us_sleep(unsigned long usecs)
{
    struct timespec req;
    req.tv_sec = (int)(usecs / 1000000L);
    req.tv_nsec = (usecs - (req.tv_sec * 1000000L)) * 1000L;
    while (nanosleep(&req, &req) < 0)
        ;
    return 1;
}

/* this code is adapted from Chip Gracey's PNut IDE */

int PL_HardwareFound(PL_state *state, int *pVersion)
{
    int version, i;
        
    /* initialize the serial buffers */
    SerialInit(state);
    
    /* reset the propeller (includes post-reset delay of 100ms) */
    (*state->serial->reset)(state->serialData);
    
    /* transmit the calibration pulses */
    TByte(state, 0xf9);
    
    /* transmit the handshake pattern */
    state->lfsr = 'P';
    for (i = 0; i < 250; ++i)
        TByte(state, IterateLFSR(state) | 0xfe);

    /* transmit calibration pulses to clock out the connection response and the version byte */
    for (i = 0; i < 250 + 8; ++i)
        TByte(state, 0xf9);
        
    /* flush the transmit buffer */
    TComm(state);
    
    /* receive the connection response */
    for (i = 0; i < 250; ++i)
        if (RBit(state, 100) != IterateLFSR(state))
            return FALSE;
        
    /* receive the chip version */
    for (version = i = 0; i < 8; ++i) {
        int bit = RBit(state, 50);
        if (bit < 0)
            return FALSE;
        version = ((version >> 1) & 0x7f) | (bit << 7);
    }
    *pVersion = version;
        
    /* return successfully */
    return TRUE;
}

/* SerialInit - initialize the serial buffers */
static void SerialInit(PL_state *state)
{
    state->txcnt = 0;
    state->rxnext = 0;
    state->rxcnt = 0;
}

/* TByte - add a byte to the transmit buffer */
static void TByte(PL_state *state, uint8_t x)
{
    state->txbuf[state->txcnt++] = x;
    if (state->txcnt >= sizeof(state->txbuf))
        TComm(state);
}

/* TLong - add a long to the transmit buffer */
static void TLong(PL_state *state, uint32_t x)
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
        TByte(state, byte);
        x <<= 3;
#else
        // p1 code
        uint8_t byte = 0x92
                     | ((i == 10 ? -1 : 0) & 0x60)
                     |  (x & 1)
                     | ((x & 2) << 2)
                     | ((x & 4) << 4);
        TByte(state, byte);
        x >>= 3;
#endif

    }
}

/* TComm - write the transmit buffer to the port */
static void TComm(PL_state *state)
{
    (*state->serial->tx)(state->serialData, state->txbuf, state->txcnt);
    state->txcnt = 0;
}

/* RBit - receive a bit with a timeout */
static int RBit(PL_state *state, int timeout)
{
    int result;
    for (;;) {
        if (state->rxnext >= state->rxcnt) {
            state->rxcnt = (*state->serial->rx_timeout)(state->serialData, state->rxbuf, sizeof(state->rxbuf), timeout);
            if (state->rxcnt <= 0) {
                /* hardware lost */
                return -1;
            }
            state->rxnext = 0;
        }
        result = state->rxbuf[state->rxnext++] - 0xfe;
        if ((result & 0xfe) == 0)
            return result;
        /* checksum error */
    }
}

/* IterateLFSR - get the next bit in the lfsr sequence */
static int IterateLFSR(PL_state *state)
{
    uint8_t lfsr = state->lfsr;
    int result = lfsr & 1;
    state->lfsr = ((lfsr << 1) & 0xfe) | (((lfsr >> 7) ^ (lfsr >> 5) ^ (lfsr >> 4) ^ (lfsr >> 1)) & 1);
    return result;
}

/* end of code adapted from Chip Gracey's PNut IDE */
