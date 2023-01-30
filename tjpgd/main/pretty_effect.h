/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once
#include <stdint.h>
#include "esp_err.h"

esp_err_t decode_image(uint16_t ***pixels);

/**
 * @brief Calculate the effect for a bunch of lines.
 *
 * @param dest Destination for the pixels. Assumed to be LINECT * 320 16-bit pixel values.
 * @param line Starting line of the chunk of lines.
 * @param frame Current frame, used for animation
 * @param linect Amount of lines to calculate
 */
void load_lines(uint16_t *dest, int line, int linect);


/**
 * @brief Initialize the effect
 *
 * @return ESP_OK on success, an error from the jpeg decoder otherwise.
 */
esp_err_t load_logo(void);

void clear_all();
