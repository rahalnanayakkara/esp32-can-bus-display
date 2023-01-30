esp_err_t init_pixels();

/**
 * @brief Calculate the effect for a bunch of lines.
 *
 * @param dest Destination for the pixels. Assumed to be LINECT * 320 16-bit pixel values.
 * @param line Starting line of the chunk of lines.
 * @param frame Current frame, used for animation
 * @param linect Amount of lines to calculate
 */
void load_lines(uint16_t *dest, int line, int linect);

void clear_all();

void load_image(char* path);

void fade_out();
