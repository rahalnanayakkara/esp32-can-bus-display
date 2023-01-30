/* LCD tjpgd example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_heap_caps.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "pretty_effect.h"

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

#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  0
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_DATA0          23  /*!< for 1-line SPI, this also refered as MOSI */
#define EXAMPLE_PIN_NUM_PCLK           18
#define EXAMPLE_PIN_NUM_CS             5
#define EXAMPLE_PIN_NUM_DC             22
#define EXAMPLE_PIN_NUM_RST            15
#define EXAMPLE_PIN_NUM_BK_LIGHT       13

// The pixel number in horizontal and vertical
#define LCD_H_RES              320
#define LCD_V_RES              240

// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

esp_lcd_panel_handle_t panel_handle = NULL;

// Simple routine to generate some patterns and send them to the LCD. Because the
// SPI driver handles transactions in the background, we can calculate the next line
// while the previous one is being sent.
static uint16_t *s_lines[2];

static void update_display()
{
    // Indexes of the line currently being sent to the LCD and the line we're calculating
    int sending_line = 0;
    int calc_line = 0;

    for (int y = 0; y < LCD_V_RES; y += PARALLEL_LINES) 
    {
        // Loads a line for sending
        load_lines(s_lines[calc_line], y, PARALLEL_LINES);
        sending_line = calc_line;
        calc_line = !calc_line;

        // Send the loaded data
        esp_lcd_panel_draw_bitmap(panel_handle, 0, y, 0 + LCD_H_RES, y + PARALLEL_LINES, s_lines[sending_line]);
    }
}

void initialize()
{
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    // Initialize the GPIO of backlight
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_PCLK,
        .mosi_io_num = EXAMPLE_PIN_NUM_DATA0,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = PARALLEL_LINES * LCD_H_RES * 2 + 8
    };

    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };

    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        .color_space = ESP_LCD_COLOR_SPACE_BGR,
        .bits_per_pixel = 16,
    };
    // Initialize the LCD configuration
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // Turn off backlight to avoid unpredictable display on the LCD screen while initializing
    // the LCD panel driver. (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL));

    // Reset the display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Swap x and y axis (Different LCD screens may need different options)
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));

    // Turn on backlight (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL));

    // "Rotate or not" flag
    bool is_rotated = true;

    // Allocate memory for the pixel buffers
    for (int i = 0; i < 2; i++) {
        s_lines[i] = heap_caps_malloc(LCD_H_RES * PARALLEL_LINES * sizeof(uint16_t), MALLOC_CAP_DMA);
        assert(s_lines[i] != NULL);
    }

    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, is_rotated, is_rotated));
}

void app_main(void)
{
    // Start
    initialize();
    
    // Display Logo
    load_logo();
    update_display();

    // Wait 10 seconds and clear
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    clear_all();
    update_display();

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    uint16_t test_data[] = {8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170};
    esp_lcd_panel_draw_bitmap(panel_handle, 10, 10, 10 + 1, 10 + 4, test_data);
}
