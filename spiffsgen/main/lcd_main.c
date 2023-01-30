#include "libraries.h"

static const char *TAG = "LCD";

extern esp_lcd_panel_handle_t panel_handle;

void app_main(void)
{
    // Start
    initialize_lcd();
    initialize_spiffs();

    clear_all();
    update_display();

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // uint16_t test_data[] = {8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170, 8170};
    // esp_lcd_panel_draw_bitmap(panel_handle, 10, 10, 10 + 4, 10 + 4, test_data);

    load_image("/spiffs/mitsubishi.hex");
    update_display();

    ESP_LOGI(TAG,"Loaded Image");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    fade_out();

    vTaskDelay(100 / portTICK_PERIOD_MS);

    load_image("/spiffs/gto.hex");
    update_display();

    ESP_LOGI(TAG,"Loaded Image");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    fade_out();

    vTaskDelay(100 / portTICK_PERIOD_MS);

    load_image("/spiffs/dash.hex");
    update_display();
}
