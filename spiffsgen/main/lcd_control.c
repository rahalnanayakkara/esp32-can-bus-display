#include "libraries.h"

#define IMAGE_W 336
#define IMAGE_H 256

static const char *TAG = "LCD_CONTROL";

uint16_t **pixels;
char *file;

esp_err_t init_pixels()
{
    pixels = NULL;
    esp_err_t ret = ESP_OK;

    //Alocate pixel memory. Each line is an array of IMAGE_W 16-bit pixels; the `*pixels` array itself contains pointers to these lines.
    pixels = calloc(IMAGE_H, sizeof(uint16_t *));
    if (pixels == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for lines");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    for (int i = 0; i < IMAGE_H; i++) {
        (pixels)[i] = malloc(IMAGE_W * sizeof(uint16_t));
        if ((pixels)[i] == NULL) {
            ESP_LOGE(TAG, "Error allocating memory for line %d", i);
            ret = ESP_ERR_NO_MEM;
            goto err;
        }
    }

    file = malloc(IMAGE_W*2*sizeof(char));

    return ret;
err:
    //Something went wrong! Exit cleanly, de-allocating everything we allocated.
    if (pixels != NULL) {
        for (int i = 0; i < IMAGE_H; i++) {
            free((pixels)[i]);
        }
        free(pixels);
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
    for (int y=0; y<IMAGE_H; y++) 
    {
        for (int x=0; x<IMAGE_W; x++) 
        {
            pixels[y][x] = 0;
        }
    }
}

void load_image(char* path)
{
    FILE* f = fopen(path, "r");
    if (f == NULL) 
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    
    for(int i=0; i<IMAGE_H; i++)
    {
        fread(file, 1, IMAGE_W*2, f);

        for(int j=0; j<IMAGE_W*2; j+=2)
        {
            pixels[i][j/2] = (file[j+1]<<8) + file[j];
        }
    }

    fclose(f);   
}

void fade_out()
{
    // for(int i=0; i<5; i++)
    // {
    //     for (int y=0; y<IMAGE_H; y++) 
    //         {
    //             for (int x=0; x<IMAGE_W; x++) 
    //             {
    //                 pixels[y][x] = ((pixels[y][x] & 0xF000)>>1) + 
    //                         ((pixels[y][x] & 0x07C0)>>1) + ((pixels[y][x] & 0x001E));
    //             }
    //         } 
    //     update_display();
    // }  

    for(int i=0; i<5; i++)
    {
        for (int y=0; y<IMAGE_H; y++) 
        {
            for (int x=0; x<IMAGE_W; x++) 
            {
                pixels[y][x] = ((pixels[y][x] & 0xF000)>>1) + 
                        ((pixels[y][x] & 0x07C0)>>1) + ((pixels[y][x] & 0x001E)>>1);
            }
        } 
        update_display();  
    }
}

