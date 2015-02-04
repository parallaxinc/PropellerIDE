/*
 * gpio_sysfs.c
 *
 * Raspberry Pi GPIO handling using sysfs interface.
 * Guillermo A. Amaral B. <g@maral.me>
 *
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpio_sysfs.h"

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */


/**
 * Export GPIO pin for use. 
 * @param pin - The pin number to be used.
 * @returns Zero on success -1 on failure.  
 */
int gpio_export(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

/**
 * Unexport GPIO pin for use. 
 * @param pin - The pin number to be used.
 * @returns Zero on success -1 on failure.
 */
int gpio_unexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

/**
 * Set GPIO pin direction. 
 * @param pin - The pin number to be used.
 * @param dir - The desired pin directio, 1 = out, 0 = in.
 * @returns Zero on success -1 on failure.
 */
int gpio_direction(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio%d direction for writing!\n", pin);
		return(-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set gpio%d direction!\n", pin);
		return(-1);
	}

	close(fd);
	return(0);
}

/**
 * Read GPIO pin state. 
 * @param pin - The pin number to be read.
 * @returns The pin state 0 or 1.
 */
int gpio_read(int pin)
{
#define VALUE_MAX 30
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio%d for reading!\n", pin);
		return(-1);
	}

	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read gpio%d!\n", pin);
		return(-1);
	}

	close(fd);

	return(atoi(value_str));
}

/**
 * Write GPIO pin state.
 * @param pin - The pin number to be written to.
 * @returns The desired pin state 0 or 1.
 */
int gpio_write(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio%d for writing!\n", pin);
		return(-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write gpio%d!\n", pin);
		return(-1);
	}

	close(fd);
	return(0);
}

#ifdef TEST_GPIO_SYSFS

/* This test blinks GPIO 4 (P1-07) while reading GPIO 24 (P1_18) */
int main(int argc, char *argv[])
{
	int repeat = 10;

	// Enable GPIO pins
	if (-1 == gpio_export(POUT) || -1 == gpio_export(PIN))
		return(1);

	// Set GPIO directions
	if (-1 == gpio_direction(POUT, OUT) || -1 == gpio_direction(PIN, IN))
		return(2);

	do {
		// Write GPIO value
		if (-1 == gpio_write(POUT, repeat % 2))
			return(3);

		// Read GPIO value
		printf("I'm reading %d in GPIO %d\n", gpio_read(PIN), PIN);

		usleep(500 * 1000);
	}
	while (repeat--);

	// Disable GPIO pins
	if (-1 == gpio_unexport(POUT) || -1 == gpio_unexport(PIN))
		return(4);

	return(0);
}

#endif // TEST_GPIO_SYSFS

