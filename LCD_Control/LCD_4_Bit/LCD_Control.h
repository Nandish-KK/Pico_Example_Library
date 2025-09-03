#define RETURN_HOME 0x2
#define CLR_DP 0x2
#define DP_SHIFT_LEFT 0x18
#define DP_SHIFT_RIGHT 0x1C
#define CURSOR_SHIFT_LEFT 0x10
#define CURSOR_SHIFT_RIGHT 0x14
#define SET_RAM_ADDR(n) (0x80 | ((n) & 0x7F)) //parse address to set the cursor position

static inline uint32_t map_to_pins(uint8_t val);
void EN_toggle();
void bit_parse(uint8_t Data_bit, bool reg);