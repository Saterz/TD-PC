/**
 * @file game.c
 * @brief Core game loop, input handling and rendering order.
 */
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "bot.h"
#include "turret.h"
#include "map.h"

/** Player starts with 10 lives. */
int LIFES = 10;

/**
 * @brief Main game loop.
 *
 * Processes input for selector movement and actions, updates bots, renders the
 * frame, and throttles timing. Returns when lives reach zero or the user exits.
 *
 * @return 0 when exiting the loop.
 */
int game_loop(void)
{
    srand(time(NULL));

    // Selector position, clamped to map bounds
    int selector_pixel_x = 0;
    int selector_pixel_y = 0;

    while (LIFES > 0)
    {

        key_event_t ev = pollevent();

        // Move bots one step along the path
        update_bots();
        // Update turrets after bot movement to react this frame
        update_all_turrets();

        if (ev.type == KEYEV_DOWN)
        {
            switch (ev.key)
            {
            case KEY_RIGHT:
                selector_pixel_x += TILE_SIZE;
                break;
            case KEY_LEFT:
                selector_pixel_x -= TILE_SIZE;
                break;
            case KEY_UP:
                selector_pixel_y -= TILE_SIZE;
                break;
            case KEY_DOWN:
                selector_pixel_y += TILE_SIZE;
                break;

            case KEY_EXE:
            {
                int grid_x = selector_pixel_x / TILE_SIZE;
                int grid_y = selector_pixel_y / TILE_SIZE;
                build_turret(grid_x, grid_y);
            }
            break;

            case KEY_F1:
                spawn_bot();
                break;

            case KEY_DEL:
            {
                int grid_x = selector_pixel_x / TILE_SIZE;
                int grid_y = selector_pixel_y / TILE_SIZE;
                if (in_bounds(grid_x, grid_y))
                    remove_turret(grid_x, grid_y);
            }
            break;

            case KEY_EXIT:
                return 0;
            }
        }

        // Clamp selector to map bounds
        if (selector_pixel_x < 0)
            selector_pixel_x = 0;
        if (selector_pixel_y < 0)
            selector_pixel_y = 0;
        if (selector_pixel_x > MAP_W * TILE_SIZE - TILE_SIZE)
            selector_pixel_x = MAP_W * TILE_SIZE - TILE_SIZE;
        if (selector_pixel_y > MAP_H * TILE_SIZE - TILE_SIZE)
            selector_pixel_y = MAP_H * TILE_SIZE - TILE_SIZE;

        render_map(selector_pixel_x, selector_pixel_y);

        sleep_ms(16);
    }

    return 0;
}
