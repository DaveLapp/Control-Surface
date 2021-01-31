#pragma once

#ifdef ARDUINO

#if defined(TEENSYDUINO)

#if defined(__AVR__) /* Teensy 2.x */
#include "USBMIDI_Teensy2.hpp"

#elif defined(__MK20DX128__)  /* Teensy 3.0 */                                 \
    || defined(__MK20DX256__) /* Teensy 3.1/3.2 */                             \
    || defined(__MK64FX512__) /* Teensy 3.5 */                                 \
    || defined(__MK66FX1M0__) /* Teensy 3.6 */
#include "USBMIDI_Teensy3.hpp"

#elif defined(__IMXRT1062__) || defined(__IMXRT1052__) /* Teensy 4.0 */
#include "USBMIDI_Teensy4.hpp"

#elif defined(__MKL26Z64__) /* Teensy LC */
#include "USBMIDI_TeensyLC.hpp"

#else
#warning "Unknown Teensy board, please open an issue on GitHub" \
         "https://github.com/tttapa/Arduino-Helpers"
// Fall back to the MIDIUSB library and hope for the best
#include "USBMIDI_MIDIUSB.hpp"
#endif

#elif defined(ARDUINO_ARCH_MBED)

#include "USBMIDI_Arduino_mbed.hpp"

#else

#include <AH/Arduino-Wrapper.h>
#ifdef USBCON
#include "USBMIDI_MIDIUSB.hpp"
#else
#define CS_USB_MIDI_NOT_SUPPORTED
#endif

#endif

#else

#include "USBMIDI_Mock.hpp"

#endif
