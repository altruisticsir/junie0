#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

/* Running on a PC */
#if defined(_WIN32) || defined(__APPLE__)
    #include <stdint.h>
    #include <stdlib.h>

    /* When this is defined,
    the program knows its being
    emulated. */
    #define EMULATED

    /* ANSI color codes, to make the program
    look nice! */
    #define RST     "\033[0m"
    #define RED     "\033[31m"
    #define GREEN   "\033[32m"
    #define YELLOW  "\033[33m"
    #define BLUE    "\033[34m"
    #define MAGENTA "\033[35m"
    #define CYAN    "\033[36m"
    #define WHITE   "\033[37m"
    #define BOLD    "\033[1m"
    #define DIM     "\033[2m"
    #define DIM_RST "\033[22m"
#else
    #include <xc.h>
    #define REAL_HARDWARE

    #define SYSCLK 40000000UL
    #define BAUD 9600

    /**
     * ANSEL: Analog Select
     *     0 => Digital
     *     1 => Analog
     * 
     * TRIS: Tri-State (Direction)
     *     0 => Output
     *     1 => Input
     * 
     * LAT: Data Write
     *     0 => LOW
     *     1 => HIGH
     * 
     * PORT: Data Read
     *     Returns a pin's state.
     */
#endif

#define MAX_PROGRAM_SIZE 1024
#define MAX_STRING_LENGTH 64
#define MAX_STACK_SIZE 128

#define EQUAL_TO 0
#define NOT_EQUAL_TO 1
#define LTOET 2 /* Less than or equal to */
#define GTOET 3 /* Greater than or equal to */
#define LT 4 /* Less than */
#define GT 5 /* Greater than */
#define EXISTS 6

#include "lookup.h"
#include "hardware.h"

#endif