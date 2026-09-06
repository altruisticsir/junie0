#ifndef HARDWARE
#define HARDWARE

#if !defined(_WIN32) && !defined(__APPLE__)
    #include <xc.h>

    #include "lookup.h"

    #pragma config POSCMOD = OFF
    #pragma config OSCIOFNC = OFF
    #pragma config FSOSCEN = OFF
    #pragma config FNOSC = FRCPLL
    #pragma config FPLLIDIV = DIV_2
    #pragma config FPLLMUL = MUL_20
    #pragma config FPLLODIV = DIV_2
    #pragma config FPBDIV = DIV_1
    #pragma config FWDTEN = OFF
    #pragma config JTAGEN = OFF

    #define SYS_FREQ 40000000UL

    enum mode {
        in = 0,
        out
    };

    enum state {
        off = 0,
        on,
        toggle
    };

    /**
     * @brief Sets the direction of a specified pin;
     * either input or output.
     * @param pin_number Pin number
     * @param mode Mode
     */
    static void digital_mode(int pin_number, int mode) {
        const pin_registers *pin = &pin_map[pin_number];

        *(pin->ansel_clr) = pin->mask;

        if (mode == in) {
            *(pin->tris_set) = pin->mask;
        } else if (mode == out) {
            *(pin->tris_clr) = pin->mask;
        }
    }

    /**
     * @brief Sets the state of a pin to off, on, or
     * toggle.
     * @param pin_number Pin number
     * @param state State
     */
    static void digital_flip(int pin_number, int state) {
        const pin_registers *pin = &pin_map[pin_number];
        if (state == off) {
            *(pin->lat_clr) = pin->mask;
        } else if (state == on) {
            *(pin->lat_set) = pin->mask;
        } else if (state == toggle) {
            *(pin->lat_inv) = pin->mask;
        }
    }
#endif

#endif