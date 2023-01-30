#include "libraries.h"

static const char *TAG = "INIT";

esp_lcd_panel_handle_t panel_handle = NULL;

// Simple routine to generate some patterns and send them to the LCD. Because the
// SPI driver handles transactions in the background, we can calculate the next line
// while the previous one is being sent.
static uint16_t *s_lines[2];

void update_display()
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

void initialize_lcd()
{
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    // Initialize the GPIO of backlight
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_PCLK,
        .mosi_io_num = PIN_NUM_DATA0,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = PARALLEL_LINES * LCD_H_RES * 2 + 8
    };

    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };

    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .color_space = ESP_LCD_COLOR_SPACE_BGR,
        .bits_per_pixel = 16,
    };
    // Initialize the LCD configuration
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // Turn off backlight to avoid unpredictable display on the LCD screen while initializing
    // the LCD panel driver. (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_OFF_LEVEL));

    // Reset the display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Swap x and y axis (Different LCD screens may need different options)
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));

    // Turn on backlight (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL));

    // "Rotate or not" flag
    bool is_rotated = true;

    // Allocate memory for the pixel buffers
    for (int i = 0; i < 2; i++) {
        s_lines[i] = heap_caps_malloc(LCD_H_RES * PARALLEL_LINES * sizeof(uint16_t), MALLOC_CAP_DMA);
        assert(s_lines[i] != NULL);
    }

    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, is_rotated, is_rotated));
    
    // Initialize image of display in RAM
    init_pixels();
    update_display();
}

void initialize_spiffs()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    // Configuration of spiffs
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}
