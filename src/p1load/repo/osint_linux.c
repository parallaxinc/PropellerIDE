/**
 * @file osint_linux.c
 *
 * Serial I/O functions used by PLoadLib.c
 *
 * Copyright (c) 2009 by John Steven Denson
 * Modified in 2011 by David Michael Betz
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>

#include "osint.h"
#ifdef RASPBERRY_PI
#include "gpio_sysfs.h"
#endif

typedef int HANDLE;
static HANDLE hSerial = -1;
static struct termios old_sparm;
static int continue_terminal = 1;

#ifdef RASPBERRY_PI
static int propellerResetGpioPin = 17;
static int propellerResetGpioLevel = 0;
#endif

/* Normally we use DTR for reset */
static reset_method_t reset_method = RESET_WITH_DTR;

int use_reset_method(char* method)
{
    if (strcasecmp(method, "dtr") == 0)
        reset_method = RESET_WITH_DTR;
    else if (strcasecmp(method, "rts") == 0)
       reset_method = RESET_WITH_RTS;
#ifdef RASPBERRY_PI
    else if (strncasecmp(method, "gpio", 4) == 0)
    {
        reset_method = RESET_WITH_GPIO;

        char *token;
        token = strtok(method, ",");
        if (token)
        {
            token = strtok(NULL, ",");
            if (token)
            {
                propellerResetGpioPin = atoi(token);
            }
            token = strtok(NULL, ",");
            if (token)
            {
                propellerResetGpioLevel = atoi(token); 
            }
        }

        printf ("Using GPIO pin %d as Propeller reset ", propellerResetGpioPin);
        if (propellerResetGpioLevel)
        {
            printf ("(HIGH).\n");
        }
        else
        {
            printf ("(LOW).\n");
        }
        gpio_export(propellerResetGpioPin);
        gpio_write(propellerResetGpioPin, propellerResetGpioLevel ^ 1);
        gpio_direction(propellerResetGpioPin, 1);
    }
#endif
    else {
        return -1;
    }
    return 0;
}

static void chk(char *fun, int sts)
{
    if (sts != 0)
        printf("%s failed\n", fun);
}

int serial_find(const char* prefix, int (*check)(const char* port, void* data), void* data)
{
    char path[PATH_MAX];
    int prefixlen = strlen(prefix);
    struct dirent *entry;
    DIR *dirp;
    
    if (!(dirp = opendir("/dev")))
        return -1;
    
    while ((entry = readdir(dirp)) != NULL) {
        if (strncmp(entry->d_name, prefix, prefixlen) == 0) {
            sprintf(path, "/dev/%s", entry->d_name);
            if ((*check)(path, data) == 0) {
                closedir(dirp);
                return 0;
            }
        }
    }
    
    closedir(dirp);
    return -1;
}

static void sigint_handler(int signum)
{
        serial_done();
        continue_terminal = 0;
}

/**
 * open serial port
 * @param port - COMn port name
 * @returns hSerial - file handle to serial port
 */
int serial_init(const char* port, unsigned long baud)
{
    struct termios sparm;
    int tbaud = 0;

    /* open the port */
#ifdef MACOSX
    hSerial = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
#else
    hSerial = open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
#endif
    if(hSerial == -1) {
        //printf("error: opening '%s' -- %s\n", port, strerror(errno));
        return 0;
    }
    
    signal(SIGINT, sigint_handler);
    
    /* set the terminal to exclusive mode */
    if (ioctl(hSerial, TIOCEXCL) != 0) {
        //printf("error: can't open terminal for exclusive access\n");
        close(hSerial);
        return 0;
    }

    fcntl(hSerial, F_SETFL, 0);

    /* set the baud rate */
    if (!serial_baud(baud)) {
        close(hSerial);
        return 0;
    }

    fcntl(hSerial, F_SETFL, 0);

    /* get the current options */
    chk("tcgetattr", tcgetattr(hSerial, &old_sparm));
    sparm = old_sparm;
    
    /* set raw input */
#ifdef MACOSX
    cfmakeraw(&sparm);
    sparm.c_cc[VTIME] = 0;
    sparm.c_cc[VMIN] = 1;
    chk("cfsetspeed", cfsetspeed(&sparm, tbaud));
#else
    memset(&sparm, 0, sizeof(sparm));
    sparm.c_cflag     = CS8 | CLOCAL | CREAD;
    sparm.c_lflag     = 0; // &= ~(ICANON | ECHO | ECHOE | ISIG);
    sparm.c_oflag     = 0; // &= ~OPOST;

    sparm.c_iflag     = IGNPAR | IGNBRK;
    sparm.c_cc[VTIME] = 0;
    sparm.c_cc[VMIN] = 1;
    chk("cfsetispeed", cfsetispeed(&sparm, tbaud));
    chk("cfsetospeed", cfsetospeed(&sparm, tbaud));
#endif

    /* set the options */
    chk("tcflush", tcflush(hSerial, TCIFLUSH));
    chk("tcsetattr", tcsetattr(hSerial, TCSANOW, &sparm));

    return 1;
}

/**
 * change the baud rate of the serial port
 * @param baud - baud rate
 * @returns 1 for success and 0 for failure
 */
int serial_baud(unsigned long baud)
{
    struct termios sparm;
    int tbaud = 0;

    switch(baud) {
        case 0: // default
            tbaud = B115200;
            break;
#ifdef B921600
        case 921600:
            tbaud = B921600;
            break;
#endif
#ifdef B576000
        case 576000:
            tbaud = B576000;
            break;
#endif
#ifdef B500000
        case 500000:
            tbaud = B500000;
            break;
#endif
#ifdef B460800
        case 460800:
            tbaud = B460800;
            break;
#endif
#ifdef B230400
        case 230400:
            tbaud = B230400;
            break;
#endif
        case 115200:
            tbaud = B115200;
            break;
        case 57600:
            tbaud = B57600;
            break;
        case 38400:
            tbaud = B38400;
            break;
        case 19200:
            tbaud = B19200;
            break;
        case 9600:
            tbaud = B9600;
            break;
        default:
            printf("Unsupported baudrate. Use ");
            tbaud = baud; break;
#ifdef B921600
            printf("921600, ");
#endif
#ifdef B576000
            printf("576000, ");
#endif
#ifdef B500000
            printf("500000, ");
#endif
#ifdef B460800
            printf("460800, ");
#endif
#ifdef B230400
            printf("230400, ");
#endif
            printf("115200, 57600, 38400, 19200, or 9600\n");
            serial_done();
            exit(2);
            break;
    }
    
    /* get the current options */
    chk("tcgetattr", tcgetattr(hSerial, &sparm));
    
    /* set raw input */
#ifdef MACOSX
    chk("cfsetspeed", cfsetspeed(&sparm, tbaud));
#else
    chk("cfsetispeed", cfsetispeed(&sparm, tbaud));
    chk("cfsetospeed", cfsetospeed(&sparm, tbaud));
#endif

    /* set the options */
    chk("tcflush", tcflush(hSerial, TCIFLUSH));
    chk("tcsetattr", tcsetattr(hSerial, TCSANOW, &sparm));
    
    return 1;
}

/**
 * close serial port
 */
void serial_done(void)
{
    if (hSerial != -1) {
        tcflush(hSerial, TCIOFLUSH);
        tcsetattr(hSerial, TCSANOW, &old_sparm);
        ioctl(hSerial, TIOCNXCL);
        close(hSerial);
        hSerial = -1;
    }
#ifdef RASPBERRY_PI
    if (reset_method == RESET_WITH_GPIO)
    {
//        gpio_unexport(propellerResetGpioPin);
    }
#endif
}

/**
 * receive a buffer
 * @param buff - char pointer to buffer
 * @param n - number of bytes in buffer to read
 * @returns number of bytes read
 */
int rx(uint8_t* buff, int n)
{
    ssize_t bytes = read(hSerial, buff, n);
    if(bytes < 1) {
        printf("Error reading port: %d\n", (int)bytes);
        return 0;
    }
    return (int)bytes;
}

/**
 * transmit a buffer
 * @param buff - char pointer to buffer
 * @param n - number of bytes in buffer to send
 * @returns zero on failure
 */
int tx(uint8_t* buff, int n)
{
    ssize_t bytes;
#if 0
    int j = 0;
    while(j < n) {
        printf("%02x ",buff[j++]);
    }
    printf("tx %d byte(s)\n",n);
#endif
    bytes = write(hSerial, buff, n);
    if(bytes != n) {
        printf("Error writing port\n");
        return 0;
    }
    return (int)bytes;
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
    ssize_t bytes = 0;
    struct timeval toval;
    fd_set set;

    FD_ZERO(&set);
    FD_SET(hSerial, &set);

    toval.tv_sec = timeout / 1000;
    toval.tv_usec = (timeout % 1000) * 1000;

    if (select(hSerial + 1, &set, NULL, NULL, &toval) > 0) {
        if (FD_ISSET(hSerial, &set))
            bytes = read(hSerial, buff, n);
    }

    return (int)(bytes > 0 ? bytes : SERIAL_TIMEOUT);
}

/**
 * assert_reset ... Asserts the Propellers reset signal via DTR, RTS or GPIO pin.
 * @returns void
 */
static void assert_reset(void)
{
    int cmd;

    switch (reset_method)
    {
    case RESET_WITH_DTR:
        cmd = TIOCM_DTR;
        ioctl(hSerial, TIOCMBIS, &cmd); /* assert bit */
        break;
    case RESET_WITH_RTS:
        cmd = TIOCM_RTS;
        ioctl(hSerial, TIOCMBIS, &cmd); /* assert bit */
        break;
#ifdef RASPBERRY_PI
    case RESET_WITH_GPIO:
        gpio_write(propellerResetGpioPin, propellerResetGpioLevel);
        break;
#endif
    default:
        // should be reached
        break;
    }
}

/**
 * deassert_reset ... Deasserts the Propellers reset signal via DTR, RTS or GPIO pin.
 * @returns void
 */
static void deassert_reset(void)
{
    int cmd;

    switch (reset_method)
    {
    case RESET_WITH_DTR:
        cmd = TIOCM_DTR;
        ioctl(hSerial, TIOCMBIC, &cmd); /* assert bit */
        break;
    case RESET_WITH_RTS:
        cmd = TIOCM_RTS;
        ioctl(hSerial, TIOCMBIC, &cmd); /* assert bit */
        break;
#ifdef RASPBERRY_PI
    case RESET_WITH_GPIO:
        gpio_write(propellerResetGpioPin, propellerResetGpioLevel ^ 1);
        break;
#endif
    default:
        // should be reached
        break;
    }
}

/**
 * hwreset ... resets Propeller hardware.
 * @returns void
 */
void hwreset(void)
{
    assert_reset();
    msleep(10);
    deassert_reset();
    msleep(100);
    tcflush(hSerial, TCIFLUSH);
}

/**
 * sleep for ms milliseconds
 * @param ms - time to wait in milliseconds
 */
void msleep(int ms)
{
#if 0
    volatile struct timeb t0, t1;
    do {
        ftime((struct timeb*)&t0);
        do {
            ftime((struct timeb*)&t1);
        } while (t1.millitm == t0.millitm);
    } while(ms-- > 0);
#else
    usleep(ms * 1000);
#endif
}

#define ESC     0x1b    /* escape from terminal mode */

/**
 * simple terminal emulator
 */
void terminal_mode(int check_for_exit, int pst_mode)
{
    struct termios oldt, newt;
    char buf[128], realbuf[256];
    ssize_t cnt;
    fd_set set;
    int exit_char = 0xdead; /* not a valid character */
    int sawexit_char = 0;
    int sawexit_valid = 0; 
    int exitcode = 0;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_iflag &= ~(ICRNL | INLCR);
    newt.c_oflag &= ~OPOST;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    if (check_for_exit)
      {
        exit_char = 0xff;
      }

#if 0
    /* make it possible to detect breaks */
    tcgetattr(hSerial, &newt);
    newt.c_iflag &= ~IGNBRK;
    newt.c_iflag |= PARMRK;
    tcsetattr(hSerial, TCSANOW, &newt);
#endif

    do {
        FD_ZERO(&set);
        FD_SET(hSerial, &set);
        FD_SET(STDIN_FILENO, &set);
        if (select(hSerial + 1, &set, NULL, NULL, NULL) > 0) {
            if (FD_ISSET(hSerial, &set)) {
                if ((cnt = read(hSerial, buf, sizeof(buf))) > 0) {
                    int i;
                    // check for breaks
                    ssize_t realbytes = 0;
                    for (i = 0; i < cnt; i++) {
                      if (sawexit_valid)
                        {
                          exitcode = buf[i];
                          continue_terminal = 0;
                        }
                      else if (sawexit_char) {
                        if (buf[i] == 0) {
                          sawexit_valid = 1;
                        } else {
                          realbuf[realbytes++] = exit_char;
                          realbuf[realbytes++] = buf[i];
                          sawexit_char = 0;
                        }
                      } else if (((int)buf[i] & 0xff) == exit_char) {
                        sawexit_char = 1;
                      } else {
                        realbuf[realbytes++] = buf[i];
                        if (pst_mode && buf[i] == '\r')
                            realbuf[realbytes++] = '\n';
                      }
                    }
                    write(fileno(stdout), realbuf, realbytes);
                }
            }
            if (FD_ISSET(STDIN_FILENO, &set)) {
                if ((cnt = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
                    int i;
                    for (i = 0; i < cnt; ++i) {
                        //printf("%02x\n", buf[i]);
                        if (buf[i] == ESC)
                            goto done;
                    }
                    write(hSerial, buf, cnt);
                }
            }
        }
    } while (continue_terminal);

done:
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    if (sawexit_valid)
      {
        exit(exitcode);
      }
    
}

