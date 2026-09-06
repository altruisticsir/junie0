#ifndef LOOKUP
#define LOOKUP

/* auto-generated */
#define NUMBER 0x01U
#define STRING 0x02U
#define STRING_END 0x03U
#define NIL 0x04U
#define ADD 0x05U
#define SUB 0x06U
#define MUL 0x07U
#define DIV 0x08U
#define MOD 0x09U
#define CONDITION 0x0AU
#define SET 0x0BU
#define FETCH 0x0CU
#define FUNCTION 0x0DU
#define RETURN_SAVE 0x0EU
#define RETURN 0x0FU
#define POP 0x10U
#define FOR 0x11U
#define WHILE 0x12U
#define IF 0x13U
#define TICK 0x14U
#define JUMP 0x15U
#define SAVE 0x16U
#define RESTORE 0x17U
#define START_FILE 0x18U
#define END_FILE 0x19U
#define DELAY_MS 0x1AU
#define DIGITAL_MODE 0x1BU
#define DIGITAL_FLIP 0x1CU
#if !defined(_WIN32) && !defined(__APPLE__)

typedef struct {
    volatile uint32_t *const lat_set;
    volatile uint32_t *const lat_clr;
    volatile uint32_t *const lat_inv;
    volatile uint32_t *const tris_set;
    volatile uint32_t *const tris_clr;
    volatile uint32_t *const port;
    volatile uint32_t *const ansel_clr;
    uint32_t mask;
} pin_registers;

static const pin_registers pin_map[] = {
	{
		.lat_set = &LATASET,
		.lat_clr = &LATACLR,
		.lat_inv = &LATAINV,
		.tris_set = &TRISASET,
		.tris_clr = &TRISACLR,
		.port = &PORTA,
		.ansel_clr = &ANSELACLR,
		.mask = (1 << 0),
	},
	{
		.lat_set = &LATASET,
		.lat_clr = &LATACLR,
		.lat_inv = &LATAINV,
		.tris_set = &TRISASET,
		.tris_clr = &TRISACLR,
		.port = &PORTA,
		.ansel_clr = &ANSELACLR,
		.mask = (1 << 1),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 0),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 1),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 2),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 3),
	},
	{
		.lat_set = &LATASET,
		.lat_clr = &LATACLR,
		.lat_inv = &LATAINV,
		.tris_set = &TRISASET,
		.tris_clr = &TRISACLR,
		.port = &PORTA,
		.ansel_clr = &ANSELACLR,
		.mask = (1 << 2),
	},
	{
		.lat_set = &LATASET,
		.lat_clr = &LATACLR,
		.lat_inv = &LATAINV,
		.tris_set = &TRISASET,
		.tris_clr = &TRISACLR,
		.port = &PORTA,
		.ansel_clr = &ANSELACLR,
		.mask = (1 << 3),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 4),
	},
	{
		.lat_set = &LATASET,
		.lat_clr = &LATACLR,
		.lat_inv = &LATAINV,
		.tris_set = &TRISASET,
		.tris_clr = &TRISACLR,
		.port = &PORTA,
		.ansel_clr = &ANSELACLR,
		.mask = (1 << 4),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 5),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 7),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 8),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 9),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 10),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 11),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 13),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 14),
	},
	{
		.lat_set = &LATBSET,
		.lat_clr = &LATBCLR,
		.lat_inv = &LATBINV,
		.tris_set = &TRISBSET,
		.tris_clr = &TRISBCLR,
		.port = &PORTB,
		.ansel_clr = &ANSELBCLR,
		.mask = (1 << 15),
	},
};
#endif
#define ROW_REQUIRED 7

#endif