#ifndef STACK
#define STACK

#include "common.h"

/* A symbol; contains both a float
and a string value. The "type"
property determines which of the two
values are actually used. */
typedef struct {
    float number;
    char string[MAX_STRING_LENGTH];
    uint8_t type;
} symbol;

typedef struct {
    symbol data[MAX_STACK_SIZE];
    int top;
} stack;

/**
 * @brief Initializes the stack
 * by preparing all necessary
 * values.
 * 
 * @param s Stack
 */
void stack_init(stack *s) {
    s->top = -1;
}

/**
 * @brief Returns whether the stack is empty
 * or not.
 * 
 * @param s Stack
 */
bool stack_is_empty(stack *s) {
    return s->top == -1;
}

/**
 * @brief Returns whether the stack is full
 * or not.
 * @param s Stack
 */
bool stack_is_full(stack *s) {
    return s->top == MAX_STACK_SIZE - 1;
}

/**
 * @brief Pushes an item onto the top of the
 * stack.
 * @param s Stack
 * @param item Item
 */
bool stack_push(stack *s, symbol item) {
    if (stack_is_full(s)) { return false; } /* Stack overflow */
    s->top++;

    s->data[s->top].number = item.number;
    strcpy(s->data[s->top].string, item.string);
    s->data[s->top].type = item.type;

    #ifdef EMULATED
        if (item.type == NUMBER) {
            printf("%sPushed to stack: %s%g%s\n", GREEN, BOLD, item.number, RST);
        } else if (item.type == STRING) {
            printf("%sPushed to stack: %s\"%s\"%s\n", MAGENTA, BOLD, item.string, RST);
        } else if (item.type == NIL) {
            printf("%s%sPushed to stack: %s%snil%s\n", RED, DIM, DIM_RST, BOLD, RST);
        }
    #endif

    return true;
}

symbol stack_pop(stack *s) {
    if (stack_is_empty(s)) {
        return (symbol){0};
    }
    symbol item =s->data[s->top--];
    return item;
}

/**
 * @brief Empties the stack.
 * @param s Stack
 */
void stack_flush(stack *s) { s->top = -1; }

#endif