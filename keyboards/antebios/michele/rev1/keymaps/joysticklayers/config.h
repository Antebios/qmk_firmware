/* Copyright 2024 Richard Nunez - Antebios
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

/**======================
 **    Joystick
 *========================**/
                // {"matrix": [3, 6], "x": 6, "y": 3.375},
                // {"matrix": [8, 6], "x": 8, "y": 3.375},


#define POINTING_DEVICE_DEBUG
#define ANALOG_JOYSTICK_X_AXIS_PIN F4
#define ANALOG_JOYSTICK_Y_AXIS_PIN F5
// #define SECOND_ANALOG_JOYSTICK_X_AXIS_PIN F4
// #define SECOND_ANALOG_JOYSTICK_Y_AXIS_PIN F5
// #define POINTING_DEVICE_LEFT
// #define POINTING_DEVICE_RIGHT
#define POINTING_DEVICE_COMBINED

// #define SPLIT_KEYBOARD
#define POINTING_DEVICE_DRIVER_ANALOG_JOYSTICK
#define POINTING_DEVICE_COUNT 2

#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_INVERT_X
#define POINTING_DEVICE_INVERT_Y
#define POINTING_DEVICE_INVERT_X_LEFT
#define POINTING_DEVICE_INVERT_Y_LEFT
#define POINTING_DEVICE_INVERT_X_RIGHT
#define POINTING_DEVICE_INVERT_Y_RIGHT


// #include "analog.h"
// #include "second_analog_joystick.h"

// Enabling this option changes the startup behavior to listen for an
// active USB communication to delegate which part is master and which
// is slave. With this option enabled and theres’s USB communication,
// then that half assumes it is the master, otherwise it assumes it
// is the slave.
//
// I've found this helps with some ProMicros where the slave does not boot
#define SPLIT_USB_DETECT

#define RGB_DISABLE_WHEN_USB_SUSPENDED     // turn off effects when suspended
#define RGBLIGHT_SPLIT
#define RGBLIGHT_SLEEP

#define SPLIT_TRANSPORT_MIRROR             // If LED_MATRIX_KEYPRESSES or LED_MATRIX_KEYRELEASES is enabled, you also will want to enable SPLIT_TRANSPORT_MIRROR
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 100  // limits maximum brightness of LEDs (max 255). Higher may cause the controller to crash.
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 400U // Timeout window in ms in which the double tap can occur.

// #define NOP_FUDGE 0.4


/**======================
 **      SERIAL Driver
 *========================**/
// #define SERIAL_USART_RX_PIN GP1


/**======================
 **    I2C Driver
 *========================**/
// #define I2C_DRIVER I2CD1
// #define I2C1_SDA_PIN GP2
// #define I2C1_SCL_PIN GP3


/* Serial communication settings */
// #define USE_SERIAL
// #define SOFT_SERIAL_PIN GP1


