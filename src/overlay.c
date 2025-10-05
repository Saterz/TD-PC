/**
 * @file overlay.c
 * @brief In‑game overlay (HUD) rendering.
 */
#include <gint/display.h>
#include <gint/defs/types.h>
#include <stdio.h>
#include "game.h"
#include "overlay.h"

/**
 * @brief Draw a compact text label at the given position.
 *
 * Utility wrapper to centralize color and avoid duplicated flags.
 *
 * @param pixel_x X coordinate in pixels.
 * @param pixel_y Y coordinate in pixels.
 * @param text    Null-terminated C string to render.
 */
static void draw_label(int pixel_x, int pixel_y, const char *text)
{
    dtext(pixel_x, pixel_y, C_BLACK, text);
}

/**
 * @brief Render the heads-up display on top of the game scene.
 *
 * Shows remaining lives and the current selector grid coordinates. Also draws
 * a thin background rectangle for readability and a one-line controls hint.
 *
 * @param selector_pixel_x Selector X in pixels (top-left of tile).
 * @param selector_pixel_y Selector Y in pixels (top-left of tile).
 */
void overlay_draw()
{
    /* Panel background for readability (width fits screen, small height). */
    const int panel_height = 10; /* Enough for one text row */
    drect(0, 0, SCREEN_W - 1, panel_height, C_WHITE);
    dline(0, panel_height, SCREEN_W - 1, panel_height, C_BLACK);

    /* Assemble and draw status line: Lives and selector coordinates. */
    char status_buffer[48];
    snprintf(status_buffer, sizeof(status_buffer), "Lives: %d", LIFES);
    draw_label(2, 1, status_buffer);
}
