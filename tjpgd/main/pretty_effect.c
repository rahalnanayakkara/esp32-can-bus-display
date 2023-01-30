/*
   This code generates an effect that should pass the 'fancy graphics' qualification
   as set in the comment in the spi_master code.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "pretty_effect.h"
// #include "sdkconfig.h"
#include "esp_log.h"
// #include "decode_image.h"

#define IMAGE_W 336
#define IMAGE_H 256

const char *TAG = "ImageDec";

uint16_t **pixels;

esp_err_t decode_image(uint16_t ***pixels)
{
    *pixels = NULL;
    esp_err_t ret = ESP_OK;

    //Alocate pixel memory. Each line is an array of IMAGE_W 16-bit pixels; the `*pixels` array itself contains pointers to these lines.
    *pixels = calloc(IMAGE_H, sizeof(uint16_t *));
    if (*pixels == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for lines");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    for (int i = 0; i < IMAGE_H; i++) {
        (*pixels)[i] = malloc(IMAGE_W * sizeof(uint16_t));
        if ((*pixels)[i] == NULL) {
            ESP_LOGE(TAG, "Error allocating memory for line %d", i);
            ret = ESP_ERR_NO_MEM;
            goto err;
        }
    }

    return ret;
err:
    //Something went wrong! Exit cleanly, de-allocating everything we allocated.
    if (*pixels != NULL) {
        for (int i = 0; i < IMAGE_H; i++) {
            free((*pixels)[i]);
        }
        free(*pixels);
    }
    return ret;
}

//Grab a rgb16 pixel from the esp32_tiles image
static inline uint16_t get_bgnd_pixel(int x, int y)
{
    //Image has an 8x8 pixel margin, so we can also resolve e.g. [-3, 243]
    x+=8;
    y+=8;
    return pixels[y][x];
}


//Calculate the pixel data for a set of lines (with implied line size of 320). Pixels go in dest, line is the Y-coordinate of the
//first line to be calculated, linect is the amount of lines to calculate. 
void load_lines(uint16_t *dest, int line, int linect)
{
    for (int y=line; y<line+linect; y++) 
    {
        for (int x=0; x<320; x++) 
        {
            *dest++=get_bgnd_pixel(x, y);
        }
    }
}

void clear_all()
{
    for (int y=0; y<248; y++) 
    {
        for (int x=0; x<328; x++) 
        {
            pixels[y][x] = 0;
        }
    }
}


esp_err_t load_logo(void)
{
    return decode_image(&pixels);
}
