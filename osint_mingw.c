/*
 * osint_mingw.c - serial i/o routines for win32api via mingw
 *
 * Copyright (c) 2011 by Steve Denson.
 *
 * MIT License                                                           
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */

#include <windows.h>

#include <conio.h>
#include <stdio.h>
#include <stdarg.h>
#include "osint.h"

static HANDLE hSerial = INVALID_HANDLE_VALUE;
static COMMTIMEOUTS original_timeouts;
static COMMTIMEOUTS timeouts;

static void ShowLastError(void);

/* Normally we use DTR for reset */
static reset_method_t reset_method = RESET_WITH_DTR;

int use_reset_method(char* method)
{
    if (strcasecmp(method, "dtr") == 0)
        reset_method = RESET_WITH_DTR;
    else if (strcasecmp(method, "rts") == 0)
       reset_method = RESET_WITH_RTS;
    else {
        return -1;
    }
    return 0;
}

int serial_init(const char *port, unsigned long baud)
{
    char fullPort[20];
    DCB state;

    sprintf(fullPort, "\\\\.\\%s", port);

    hSerial = CreateFile(
        fullPort,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
        return FALSE;

    GetCommState(hSerial, &state);
    switch (baud) {
    case 9600:
        state.BaudRate = CBR_9600;
        break;
    case 19200:
        state.BaudRate = CBR_19200;
        break;
    case 38400:
        state.BaudRate = CBR_38400;
        break;
    case 57600:
        state.BaudRate = CBR_57600;
        break;
    case 115200:
        state.BaudRate = CBR_115200;
        break;
    default:
        return FALSE;
    }
    state.ByteSize = 8;
    state.Parity = NOPARITY;
    state.StopBits = ONESTOPBIT;
    state.fOutxDsrFlow = FALSE;
    state.fDtrControl = DTR_CONTROL_DISABLE;
    state.fOutxCtsFlow = FALSE;
    state.fRtsControl = RTS_CONTROL_DISABLE;
    state.fInX = FALSE;
    state.fOutX = FALSE;
    state.fBinary = TRUE;
    state.fParity = FALSE;
    state.fDsrSensitivity = FALSE;
    state.fTXContinueOnXoff = TRUE;
    state.fNull = FALSE;
    state.fAbortOnError = FALSE;
    SetCommState(hSerial, &state);

    GetCommTimeouts(hSerial, &original_timeouts);
    timeouts = original_timeouts;
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;

    /* setup device buffers */
    SetupComm(hSerial, 10000, 10000);

    /* purge any information in the buffer */
    PurgeComm(hSerial, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    return TRUE;
}

void serial_done(void)
{
    if (hSerial != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(hSerial);
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}

/**
 * transmit a buffer
 * @param buff - char pointer to buffer
 * @param n - number of bytes in buffer to send
 * @returns zero on failure
 */
int tx(uint8_t* buff, int n)
{
    DWORD dwBytes = 0;
    if(!WriteFile(hSerial, buff, n, &dwBytes, NULL)){
        printf("Error writing port\n");
        ShowLastError();
        return 0;
    }
    return dwBytes;
}

/**
 * receive a buffer
 * @param buff - char pointer to buffer
 * @param n - number of bytes in buffer to read
 * @returns number of bytes read
 */
int rx(uint8_t* buff, int n)
{
    DWORD dwBytes = 0;
    SetCommTimeouts(hSerial, &original_timeouts);
    if(!ReadFile(hSerial, buff, n, &dwBytes, NULL)){
        printf("Error reading port\n");
        ShowLastError();
        return 0;
    }
    return dwBytes;
}

/**
 * receive a buffer with a timeout
 * @param buff - char pointer to buffer
 * @param n - number of bytes in buffer to read
 * @param timeout - timeout in milliseconds
 * @returns number of bytes read or SERIAL_TIMEOUT
 */
int rx_timeout(uint8_t* buff, int n, int timeout)
{
    DWORD dwBytes = 0;
    timeouts.ReadTotalTimeoutConstant = timeout;
    SetCommTimeouts(hSerial, &timeouts);
    if(!ReadFile(hSerial, buff, n, &dwBytes, NULL)){
        printf("Error reading port\n");
        ShowLastError();
        return 0;
    }
    return dwBytes > 0 ? dwBytes : SERIAL_TIMEOUT;
}

/**
 * hwreset ... resets Propeller hardware using DTR
 * @returns void
 */
void hwreset(void)
{
    EscapeCommFunction(hSerial, reset_method == RESET_WITH_RTS ? SETRTS : SETDTR);
    Sleep(25);
    EscapeCommFunction(hSerial, reset_method == RESET_WITH_RTS ? CLRRTS : CLRDTR);
    Sleep(90);
    // Purge here after reset helps to get rid of buffered data.
    PurgeComm(hSerial, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

static unsigned long getms()
{
    LARGE_INTEGER ticksPerSecond;
    LARGE_INTEGER tick;   // A point in time
    LARGE_INTEGER time;   // For converting tick into real time
    // get the high resolution counter's accuracy
    QueryPerformanceFrequency(&ticksPerSecond);
    if(ticksPerSecond.QuadPart < 1000) {
        printf("Your system does not meet timer requirement. Try another computer. Exiting program.\n");
        exit(1);
    }
    // what time is it?
    QueryPerformanceCounter(&tick);
    time.QuadPart = (tick.QuadPart*1000/ticksPerSecond.QuadPart);
    return (unsigned long)(time.QuadPart);
}

/**
 * sleep for ms milliseconds
 * @param ms - time to wait in milliseconds
 */
void msleep(int ms)
{
    unsigned long t = getms();
    while((t+ms+10) > getms())
        ;
}

static void ShowLastError(void)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);
    printf("    %s\n", (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
    exit(1); // exit on error
}

/* escape from terminal mode */
#define ESC         0x1b

/*
 * if "check_for_exit" is true, then
 * a sequence EXIT_CHAR 00 nn indicates that we should exit
 */
#define EXIT_CHAR   0xff

void terminal_mode(int check_for_exit, int pst_mode)
{
    int sawexit_char = 0;
    int sawexit_valid = 0;
    int exitcode = 0;
    int continue_terminal = 1;

    while (continue_terminal) {
        uint8_t buf[1];
        if (rx_timeout(buf, 1, 0) != SERIAL_TIMEOUT) {
            if (sawexit_valid) {
                exitcode = buf[0];
                continue_terminal = 0;
            }
            else if (sawexit_char) {
                if (buf[0] == 0) {
                    sawexit_valid = 1;
                } else {
                    putchar(EXIT_CHAR);
                    putchar(buf[0]);
                    fflush(stdout);
                }
            }
            else if (check_for_exit && buf[0] == EXIT_CHAR) {
                sawexit_char = 1;
            }
            else {
                putchar(buf[0]);
                if (pst_mode && buf[0] == '\r')
                    putchar('\n');
                fflush(stdout);
            }
        }
        else if (kbhit()) {
            if ((buf[0] = getch()) == ESC)
                break;
            tx(buf, 1);
/* this should be handled by the library */
#if 0
            if(buf[0] == '\r') {
                buf[0] = '\n';
                tx(buf, 1);
            }
#endif
        }
    }

    if (check_for_exit && sawexit_valid) {
        exit(exitcode);
    }
}

