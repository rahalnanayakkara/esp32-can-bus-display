// Using SPI2 in the example, as it aslo supports octal modes on some targets
#define LCD_HOST       SPI2_HOST

// To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many.
// More means more memory use, but less overhead for setting up / finishing transfers. Make sure 240
// is dividable by this.
#define PARALLEL_LINES 60

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LCD_PIXEL_CLOCK_HZ (30 * 1000 * 1000)

#define LCD_BK_LIGHT_ON_LEVEL  0
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL

#define PIN_NUM_DATA0          23  /*!< for 1-line SPI, this also refered as MOSI */
#define PIN_NUM_PCLK           18
#define PIN_NUM_CS             5
#define PIN_NUM_DC             22
#define PIN_NUM_RST            15
#define PIN_NUM_BK_LIGHT       13

// The pixel number in horizontal and vertical
#define LCD_H_RES              320
#define LCD_V_RES              240

// Bit number used to represent command and parameter
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8

void update_display();

void initialize_lcd();

void initialize_spiffs();
