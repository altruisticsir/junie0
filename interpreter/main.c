#include "lib/common.h"
#include "lib/stack.h"
#include "lib/env.h"
#include "lib/uart.h"
#include "lib/hardware.h"

int main() {
    env_init();
    #ifdef EMULATED
        printf("%sWelcome to the Junie emulator!%s\n", GREEN, RST);

        /* Try to read bytecode from a specified file */
        /* Path to the bytecode */
        const char *path = "bin/jn.o";
        
        printf("Attempting read from %s\"%s\"%s\n", YELLOW, path, RST);
        FILE *file = fopen(path, "rb"); /* Read as binary */

        if (file == NULL) {
            /* File couldn't open */
            printf("%sFailed to open file \"%s\"%s", RED, path, RST);
        }

        printf("%s\nBytecode ===> ", YELLOW);

            int c; /* Character; int to check for EOF */
            while ((c = fgetc(file)) != EOF) {
                env_write((uint8_t)c);
            }

        printf("%s\n", RST);

        printf("Successfully read from %s\"%s\"%s (%d bytes)\n\n", GREEN, path, RST, byte_count + 1);
        printf("%sNow running program%s\n", BLUE, RST);

        /* Close the file after reading */
        fclose(file);
    #else
        uart_initialize();
        digital_mode(12, 1);
        digital_flip(12, 0);
    #endif

    for (;;) {
        #ifndef EMULATED
            /* Check if there are any incoming bytes
            through UART */
            while (uart_byte_available()) {
                /* There is an available byte;
                write to environment (program) */
                uint8_t byte = uart_read_byte();
                env_write(byte);
            }
        #endif
        env_step();
    }

    return 0;
}