#ifndef ENV
#define ENV

#include "common.h"
#include "uart.h"
#include "stack.h"

/* The program is stored in this array. */
uint8_t byte_set[MAX_PROGRAM_SIZE] = {0};

volatile int byte_count = 0; /* Keeps track of the program's current size */
volatile int byte_index = 0; /* Keeps track of where we are in the program */

volatile bool writing = false; /* Determines whether or not the program
is being written to */

volatile int start_row = 0; /* How many times in a row the START_FILE byte showed up */
volatile int end_row = 0; /* How many times in a row the END_FILE byte showed up */

stack s;

symbol variables[32] = {0};

/* A list of indexes to return to when
a RETURN instruction is hit. A LIFO stack
of integers, if you will */
int return_indexes[32] = {0};
int return_index_count = 0;

/**
 * @brief Saves a variable provided
 * an ID and value.
 * @param id ID
 * @param value Value
 */
void save_variable(symbol id, symbol value) {
    variables[(int)id.number] = value;
    #ifdef EMULATED
        if (value.type == NUMBER) {
            printf("%sSet: %s%g%s\n", GREEN, BOLD, value.number, RST);
        } else if (value.type == STRING) {
            printf("%sSet: %s\"%s\"%s\n", MAGENTA, BOLD, value.string, RST);
        } else if (value.type == NIL) {
            printf("%s%sSet: %s%snil%s\n", RED, DIM, DIM_RST, BOLD, RST);
        }
    #endif
}

/**
 * @brief Retrieves the value of a variable
 * by ID.
 * @param id ID
 */
symbol fetch_variable(symbol id) {
    symbol value = variables[(int)id.number];
    #ifdef EMULATED
        if (value.type == NUMBER) {
            printf("%sFetched: %s%g%s ", MAGENTA, BOLD, value.number, RST);
        } else if (value.type == STRING) {
            printf("%sFetched: %s\"%s\"%s ", GREEN, BOLD, value.string, RST);
        } else if (value.type == NIL) {
            printf("%s%sFetched: %s%snil%s ", RED, DIM, DIM_RST, BOLD, RST);
        }
    #endif
    return value;
}

float extract_number() {
    /* Extract the next four bytes */
    uint8_t bytes[4];
    for (int i = 0; i < 4; i++) {
        uint8_t b = byte_set[byte_index++];
        bytes[i] = b;

        #ifdef EMULATED
            printf("%s%02X%s ", YELLOW, b, RST);
        #endif
    }

    /* Convert the bytes into floats
    from little endian */
    union {
        uint8_t b[4];
        float f;
    } converter;

    for (int i = 0; i < 4; i++) {
        converter.b[i] = bytes[i];
    }

    return converter.f;
}

void env_init() {
    stack_init(&s);
}

/**
 * @brief Polls UART to check for
 * an incoming byte. If there is one,
 * then it is written to the program.
 * @note Mandatory for yielding
 * functions.
 */
void uart_poll();

#ifndef EMULATED
void env_write(uint8_t byte);

/**
 * @brief Delay; waits
 * a specified amount of time in
 * microseconds.
 * Additionally, checks to see
 * if there are any incoming bytes.
 * @param us Milliseconds
 * @note Yielding
 */
void delay_us(uint32_t us) {
    uint32_t ticks = us * (SYS_FREQ / 2000000UL);
    uint32_t start = _CP0_GET_COUNT();

    while ((_CP0_GET_COUNT() - start) < ticks) {
        uart_poll();
    }
}
void delay_ms(uint32_t ms) {
    uint32_t ticks = ms * (SYS_FREQ / 2000UL);
    uint32_t start = _CP0_GET_COUNT();

    while ((_CP0_GET_COUNT() - start) < ticks) {
        while (uart_byte_available()) {
            /* Checks if there's an incoming
            byte. This is necessary, so that
            delays do not disrupt program
            uploads. */
            env_write(uart_read_byte());
        }
    }
}
#endif
/*
 *
 */

void env_write(uint8_t byte) {
    if (byte == START_FILE) {
        end_row = 0;
        if (start_row >= ROW_REQUIRED) {
            /* Enough START_FILE bytes in a row;
            begin writing to the program. */
            writing = true;

            #ifndef EMULATED
                digital_flip(12, 1);
            #endif

            start_row = 0;

            byte_count = 0; /* Reset program size */
            byte_index = 0;
            
            return_index_count = 0;
            
            memset(return_indexes, 0, sizeof(return_indexes));
            memset(variables, 0, sizeof(variables));

            stack_flush(&s); /* Empty the stack */

            return;
        } else {
            start_row++;
        }
    } else if (byte == END_FILE) {
        start_row = 0;
        if (end_row >= ROW_REQUIRED) {
            /* Enough END_FILE bytes in a row;
            stop writing to the program, remove
            excess END_FILE bytes, and run. */
            writing = false;

            #ifndef EMULATED
                digital_flip(12, 0);
            #endif

            end_row = 0;
            
            byte_count -= (ROW_REQUIRED + 1);
            byte_index = 0; /* Set to beginning */

            #ifdef EMULATED
                /* Print each written byte */
                for (int i = 0; i <= byte_count; i++) {
                    printf("%02X ", byte_set[i]);
                }
            #endif

            return;
        } else {
            end_row++;
        }
    } else {
        /* Something other than START_FILE or END_FILE
        showed up, reset the row counts */
        start_row = 0;
        end_row = 0;
    }

    if (writing == true) {
        byte_set[byte_count++] = byte;
    }
}

void uart_poll() {
    while (uart_byte_available()) {
        env_write(uart_read_byte());
    }
}

void env_step() {
    if (writing == true) return; /* Still writing, not running */
    if (byte_index > byte_count) {
        /* Loop the program back to the
        start; flush stack as well */
        byte_index = 0;
        stack_flush(&s);
        #ifdef EMULATED
            printf("\t%s%sRestart%s\n", YELLOW, BOLD, RST);
        #endif
    }
    uint8_t byte = byte_set[byte_index++];
    #ifdef EMULATED
        printf("%s%s> 0x%s%02X%s ", DIM, BLUE, DIM_RST, byte, RST);
    #endif

    switch(byte) {
        case NUMBER: {
            /* Extract and push a number
            to the stack (reads next 4 bytes) */
            float number = extract_number();

            #ifdef EMULATED
                printf("%s%s[ %g ]%s ", BLUE, BOLD, number, RST);
            #endif

            symbol sym = {0};
            sym.number = number;
            sym.type = NUMBER;

            stack_push(&s, sym);

            break;
        }
        case STRING: {
            char str[MAX_STRING_LENGTH];
            int length = 0;

            /* Keep reading bytes and placing
            them inside of the string until
            STRING_END is reached */
            for (;;) {
                if (byte_set[byte_index] == STRING_END) {
                    byte_index++;
                     break;
                }
                uint8_t byte = byte_set[byte_index++];
                #ifdef EMULATED
                    printf("%s%02X%s ", YELLOW, byte, RST);
                #endif
                str[length++] = (char)byte;
            }
            str[length++] = '\0'; /* Terminator */

            #ifdef EMULATED
                printf("%s%s\"%s\"%s ", BLUE, BOLD, str, RST);
            #endif

            symbol sym = {0};
            strcpy(sym.string, str);
            sym.type = STRING;
            
            stack_push(&s, sym);
            break;
        }
        case NIL: {
            symbol sym = {0};
            sym.type = NIL;
            
            stack_push(&s, sym);
            break;
        }
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case MOD: {
            /* Take the left and right
            symbols, perform an operation,
            and push the result */
            
            symbol right = stack_pop(&s);
            symbol  left = stack_pop(&s);

            symbol sym = {0};
            sym.type = NUMBER;

            if (byte == ADD) {
                if (left.type == STRING || right.type == STRING) {
                    if (left.type == NUMBER) {
                        /* Convert number to string in order
                        to combine */
                        snprintf(left.string, MAX_STRING_LENGTH, "%g", left.number);
                    } else if (left.type == NIL) {
                        /* Nil value; empty string as a
                        fallback */
                        snprintf(left.string, MAX_STRING_LENGTH, "");
                    }
                    if (right.type == NUMBER) {
                        /* Convert number to string in order
                        to combine */
                        snprintf(right.string, MAX_STRING_LENGTH, "%g", right.number);
                    } else if (right.type == NIL) {
                        /* Nil value; empty string as a
                        fallback */
                        snprintf(left.string, MAX_STRING_LENGTH, "");
                    }
                    /* Combine strings and apply to the symbol to be pushed */
                    snprintf(sym.string, MAX_STRING_LENGTH, "%s%s", left.string, right.string);
                    sym.type = STRING;
                } else {
                    sym.number = left.number + right.number;
                }
            }
            if (byte == SUB) {
                if (left.type == STRING || right.type == STRING) {
                    /* Subtraction of strings is not allowed */
                    snprintf(sym.string, MAX_STRING_LENGTH, "nil");
                    sym.type = NIL;
                } else {
                    sym.number = left.number - right.number;
                }
            }
            if (byte == MUL) {
                if (left.type == STRING || right.type == STRING) {
                    /* Multiplication of strings is not allowed */
                    snprintf(sym.string, MAX_STRING_LENGTH, "nil");
                    sym.type = NIL;
                } else {
                    sym.number = left.number * right.number;
                }
            }
            if (byte == DIV) {
                if (left.type == STRING || right.type == STRING) {
                    /* Division of strings is not allowed */
                    snprintf(sym.string, MAX_STRING_LENGTH, "nil");
                    sym.type = NIL;
                } else {
                    sym.number = left.number / right.number;
                }
            }
            if (byte == MOD) {
                if (left.type == STRING || right.type == STRING) {
                    /* Modulo of strings is not allowed */
                    snprintf(sym.string, MAX_STRING_LENGTH, "nil");
                    sym.type = NIL;
                } else {
                    /* Integer operation only, convert */
                    float result = (int)left.number % (int)right.number;
                    sym.number = result;
                }
            }

            stack_push(&s, sym);
            break;
        }
        case SET: {
            /* Pop ID and value;
            save to memory */
            symbol id = stack_pop(&s);
            symbol value = stack_pop(&s);
            save_variable(id, value);
            break;
        }
        case FETCH: {
            /* Push variable via ID
            onto stack */
            symbol id = stack_pop(&s);
            stack_push(&s, fetch_variable(id));
            break;
        }
        case RETURN: {
            byte_index = return_indexes[--return_index_count];
            #ifdef EMULATED
                printf("%sReturned to index %s%d%s\n", GREEN, BOLD, byte_index, RST);
            #endif
            break;
        }
        case RETURN_SAVE: {
            /* This is only placed before NUMBER ^x>y JUMP, so we
            can safely assume that the offset is always six */
            return_indexes[return_index_count++] = byte_index + 6; /* Save current index
            to jump back to later when RETURN is hit */
            break;
        }
        case JUMP: {
            symbol index = stack_pop(&s);
            byte_index = (int)index.number;

            #ifdef EMULATED
                printf("%sJumped to index %s%d%s\n", MAGENTA, BOLD, byte_index, RST);
            #endif
            break;
        }
        case POP: {
            stack_pop(&s);
            #ifdef EMULATED
                printf("%s%sPopped and tossed item from stack%s\n", DIM, MAGENTA, RST);
            #endif
            break;
        }
        case FUNCTION: {
            /* argc */
            symbol ac = stack_pop(&s);
            symbol args[8] = {0};
            int arg_count = (int)ac.number;

            /* -1 to correct index starts */
            /* Based off the arg_count that was
            extracted above, symbols are popped
            and added to the args list. These
            symbols include the IDs of arguments. */
            for (int i = arg_count; i > 0; i--) {
                args[i] = stack_pop(&s);
            }

            /* In the place of the saved IDs, real
            argument values are installed, and can
            then be used like variables. */
            for (int i = arg_count; i > 0; i--) {
                symbol arg = stack_pop(&s);
                save_variable(args[i], arg);
            }
            stack_pop(&s);
            break;
        }
        case FOR: {
            #ifdef EMULATED
                printf("%sFOR%s\n", MAGENTA, RST);
            #endif

            /* Pop all necessary information */
            symbol end   = stack_pop(&s);
            symbol mode  = stack_pop(&s);
            symbol start = stack_pop(&s);
            symbol id    = stack_pop(&s);
            symbol exit_index = stack_pop(&s);

            /* Loop variable; begins at start */
            symbol loop_value = start;
            loop_value.type = NUMBER;
            save_variable(id, loop_value);

            /* Ternary; if mode is zero then the mode
            is "to". If not, then it's "with", which is
            inclusive. */
            while ((int)mode.number == 0
                ? (int)loop_value.number < (int)end.number
                : (int)loop_value.number <= (int)end.number) {
                uart_poll();
                /* Step */
                env_step();
                /* Keep stepping until a tick
                marker is reached */
                if (byte_set[byte_index] == TICK) {
                    symbol tick_id = stack_pop(&s);
                    /* If the tick id isn't equal to the loop
                    variable's id, then it's not this for loop's
                    tick, and we ignore it. */
                    if ((int)tick_id.number != (int)id.number) {
                        continue;
                    }

                    /* Add to the loop variable's value, as
                    a tick for this loop was reached. */
                    loop_value.number += 1;
                    /* Save the new value to the variable */
                    save_variable(id, loop_value);
                }
            }

            /* Skip the for loop once complete */
            byte_index = (int)exit_index.number;
            break;
        }
        case WHILE: {
            #ifdef EMULATED
                printf("%sWHILE%s\n", MAGENTA, RST);
            #endif

            symbol ei = stack_pop(&s);
            symbol c = stack_pop(&s); /* If the number value of
            this condition symbol is 0, then it's false. Otherwise, it's
            true. */

            int condition = (int)c.number;
            int exit_index = (int)ei.number;

            /* Check that the condition equals zero.
            If it does, then exit the loop. Otherwise,
            do nothing */
            if (condition == 0) byte_index = exit_index;

            break;
        }
        case CONDITION: {
            symbol o = stack_pop(&s);
            int op = (int)o.number;

            int result = 0; /* 0 is false, 1 is true */

            switch (op)
            {
                case EXISTS: {
                    /* Simple check for existence, check that the
                    symbol popped isn't nil and does not have a zero
                    value. */
                    symbol sym = stack_pop(&s);
                    result = (sym.type != NIL && (int)sym.number != 0) ? 1 : 0;
                    break;
                }
                default: {  
                    symbol  r = stack_pop(&s);
                    symbol  l = stack_pop(&s);

                    float  left = l.number;
                    float right = r.number;

                    /* Due to floating point errors, comparing two
                    floats directly often leads to unintended behavior.
                    So, we'll approximate the equality. */
                    float difference = fabs(left - right);
                    float epsilon = 0.0001f;

                    /* Return the "boolean" based on the comparison operator
                    provided. */
                    if (op == EQUAL_TO)     result =  (difference < epsilon) ? 1 : 0;
                    if (op == NOT_EQUAL_TO) result = !(difference < epsilon) ? 1 : 0;
                    if (op == LTOET)        result =  (left <= right) ? 1 : 0;
                    if (op == GTOET)        result =  (left >= right) ? 1 : 0;
                    if (op == LT)           result =  (left <  right) ? 1 : 0;
                    if (op == GT)           result =  (left >  right) ? 1 : 0;

                    break;
                }
            }

            /* Pushes the "boolean" result of the
            comparison. Either 0 or 1. */
            symbol sym = {0};
            sym.number = result;
            sym.type = NUMBER;
            stack_push(&s, sym);

            break;
        }
        case DELAY_MS: {
            #ifndef EMULATED
                symbol time = stack_pop(&s);
                uint32_t ms = (uint32_t)time.number;
                delay_ms(ms);
            #endif
            break;
        }
        case DIGITAL_MODE: {
            symbol m = stack_pop(&s); /* Mode */
            symbol p = stack_pop(&s); /* Pin */
            
            int pin = (int)p.number;
            int mode = (int)m.number;
            #ifndef EMULATED
                digital_mode(pin, mode);
            #else
                printf("%d, %d\n", pin, mode);
            #endif
            break;
        }
        case DIGITAL_FLIP: {
            symbol st = stack_pop(&s); /* State */
            symbol p = stack_pop(&s); /* Pin */

            int pin = (int)p.number;
            int state = (int)st.number;
            #ifndef EMULATED
                digital_flip(pin, state);
            #else
                printf("%d, %d\n", pin, state);
            #endif
            break;
        }
        case ADC_READ: {
            /*Reads the analog => digital value of a
            specified channel/pin and pushes the result
            to the stack. */
            symbol channel = stack_pop(&s); /* Like pin */
            symbol result = {0};
            #ifndef EMULATED
                /* Get the ADC value of the specified channel/pin */
                result.number = (float)adc_read((int)channel.number);
            #else
                result.number = 0.0f;
            #endif
            result.type = NUMBER;

            /* Push the result */
            stack_push(&s, result);
            break;
        }
        default: { break; }
    }
}

#endif