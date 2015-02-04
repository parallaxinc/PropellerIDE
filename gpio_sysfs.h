#ifndef GPIO_SYSFS_H
#define GPIO_SYSFS_H

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

int gpio_export (int pin);

int gpio_unexport (int pin);

int gpio_direction (int pin, int dir);

int gpio_read (int pin);

int gpio_write (int pin, int value);

#endif // GPIO_SYSFS_H

