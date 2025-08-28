#define DIR_CW 0x10
#define DIR_CCW 0x20


static uint32_t last_interrupt_time;
static const uint32_t debounce_delay_ms = 25; // Debounce time in milliseconds


// Transition states for rotary encoder
typedef enum
{
    IDLE_LOW  = 0x0, 
    CCW1      = 0x1, 
    CW1       = 0x2, 
    IDLE_HIGH = 0x3, 
    CCW2      = 0x4, 
    CW2       = 0x5  

} encoder_state_t;

static const encoder_state_t transition_table[6][4] = {
//IDLE_LOW 00
    {IDLE_LOW,          CCW1,         CW1,          IDLE_LOW},

//CCW1 01
    {IDLE_LOW,          CCW1,         IDLE_LOW,     IDLE_HIGH|DIR_CCW },

//CW1 10
    {IDLE_LOW,          IDLE_LOW,     CW1,          IDLE_HIGH|DIR_CW  },

//IDLE_HIGH 11
    {IDLE_HIGH,         CW2,          CCW2,         IDLE_HIGH},

//CCW2 10 <- DIR_CHANGE 
    {IDLE_LOW|DIR_CCW,  IDLE_HIGH,    CCW2,         IDLE_HIGH},
    
//CW2  01 <- DIR_CHANGE
    {IDLE_LOW|DIR_CW,   CW2,          IDLE_HIGH,    IDLE_HIGH},
};

 encoder_state_t state = IDLE_LOW;
 encoder_state_t read_encoder_state(void);
 uint Encoder_Event(uint gpio); 
 void gpio_callback(uint gpio, uint32_t events);


