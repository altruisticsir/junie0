#ifndef UART
#define UART

#include "common.h"

/* Only run UART on actual hardware, obviously */
#ifndef EMULATED
    /**
     * @brief Sets up UART.
     * @note Pin 22 to RX, Pin 21 to TX
     */
    void uart_initialize() {
        SYSKEY = 0x0;

        /* Unlock modification of system
        registers by applying these two
        keys */
        SYSKEY = 0xAA996655;
        SYSKEY = 0x556699AA;

        /* Unlock PPS */
        CFGCONbits.IOLOCK = 0;

        U2RXRbits.U2RXR = 0b0011;   /* U2RX to Pin 22 */
        RPB10Rbits.RPB10R = 0b0010; /* U2TX to Pin 21 */

        CFGCONbits.IOLOCK = 1;

        /* Lock PPS */
        SYSKEY = 0x0;

        TRISBbits.TRISB10 = 0; /* U2TX: output */
        TRISBbits.TRISB11 = 1; /* U2RX: input  */

        /* Configure the UART while it is disabled. */
        U2MODE = 0;
        U2STA = 0;

        /* Applying baud rate */
        U2BRG = (SYSCLK / (16 * BAUD)) - 1;
        
        U2MODEbits.ON = 1;

        U2STAbits.UTXEN = 1; /* Enable U2 TX */
        U2STAbits.URXEN = 1; /* Enable U2 RX */
    }

    /**
     * @brief Checks whether a byte is available via UART.
     * @return Nonzero when U2RXREG contains a byte
     */
    bool uart_byte_available() {
        if (U2STAbits.OERR) {
            /* If an overrun error is
            encountered, resume the read */
            U2STAbits.OERR = 0;
        }
        return U2STAbits.URXDA;
    }

    /**
     * @brief Reads an incoming byte via UART.
     * @return U2RXREG
     * @note U2STAbits.URXDA shouldn't be empty,
     * or no valid byte will be returned
     */
    uint8_t uart_read_byte() {
        return U2RXREG;
    }

    /**
     * @brief Writes a byte via UART.
     * @param byte Byte
     * @note Yields
     */
    void uart_write_byte(uint8_t byte) {
        while (U2STAbits.UTXBF);
        U2TXREG = byte;
    }

    /**
     * @brief Writes a string via UART.
     * @param string String
     * @see uart_write_byte
     */
    void uart_write_string(const char *string) {
        while (*string) {
            uart_write_byte(*string++);
        }
    }
#endif

#endif