/**
 * @file bot.c
 * @brief Bot pathing, update and rendering.
 */
#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "bot.h"

extern bopti_image_t const spr_bot_1;

/** Size of the static bot pool. */
#define MAX_BOTS 16
static Bot bots[MAX_BOTS];

/** Default bot archetype. */
static const BotType BOT_BASIC = {
    .name = "Basic",
    .max_hp = 1,
    .speed = 1,
    .sprite = &spr_bot_1,
};

/**
 * @brief Path waypoints (grid coordinates) matching the road, S -> G.
 */
static const struct
{
    uint8_t grid_x, grid_y;
} waypoints[] = {
    {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, // across the top row
    {5, 1},
    {5, 2},
    {5, 3},
    {5, 4}, // straight down
    {6, 4},
    {7, 4},
    {8, 4},
    {9, 4},
    {10, 4}, // right turn
    {10, 5}, // down one
    {10, 6},
    {11, 6},
    {12, 6},
    {13, 6} // final right to the Goal
};
/** Number of points in the path. */
static const int WAYPOINT_COUNT = sizeof(waypoints) / sizeof(waypoints[0]);

/**
 * @brief Convert a waypoint index to tile-aligned pixel coordinates.
 * @param waypoint_index Index in the waypoint array.
 * @param out_pixel_x Output pointer for pixel X.
 * @param out_pixel_y Output pointer for pixel Y.
 */
static inline void waypoint_index_to_pixel(int waypoint_index, int *out_pixel_x, int *out_pixel_y)
{
    int grid_x = waypoints[waypoint_index].grid_x;
    int grid_y = waypoints[waypoint_index].grid_y;
    *out_pixel_x = grid_x * TILE_SIZE;
    *out_pixel_y = grid_y * TILE_SIZE;
}

/**
 * @brief Spawn a bot at the first waypoint, if a slot is available.
 */
void spawn_bot(void)
{
    for (int i = 0; i < MAX_BOTS; ++i)
    {
        if (!bots[i].active)
        {
            int spawn_pixel_x, spawn_pixel_y;
            if (WAYPOINT_COUNT == 0)
                return; // no path defined
            waypoint_index_to_pixel(0, &spawn_pixel_x, &spawn_pixel_y);
            bots[i].active = 1;
            bots[i].type = &BOT_BASIC;
            bots[i].pixel_x = spawn_pixel_x;
            bots[i].pixel_y = spawn_pixel_y;
            bots[i].next_waypoint_index = (WAYPOINT_COUNT > 1) ? 1 : 0;
            return;
        }
    }
}

/**
 * @brief Move a single bot toward its current waypoint and handle goal reach.
 * @param b Bot instance to update.
 */
static void update_bot(Bot *b)
{
    if (!b->active)
        return;
    if (b->next_waypoint_index >= WAYPOINT_COUNT)
    {
        // Reached the end (goal). Apply life penalty and despawn.
        if (LIFES > 0)
            LIFES -= 1;
        b->active = 0;
        return;
    }

    int target_pixel_x, target_pixel_y;
    waypoint_index_to_pixel(b->next_waypoint_index, &target_pixel_x, &target_pixel_y);

    int delta_x_px = target_pixel_x - b->pixel_x;
    int delta_y_px = target_pixel_y - b->pixel_y;

    // If close enough, snap and advance to next waypoint
    if (delta_x_px * delta_x_px + delta_y_px * delta_y_px <= 1)
    {
        b->pixel_x = target_pixel_x;
        b->pixel_y = target_pixel_y;
        b->next_waypoint_index++;
        return;
    }

    // Take a small step toward the target (unit grid step)
    int step_x_px = 0, step_y_px = 0;
    if (delta_x_px > 0)
        step_x_px = 1;
    else if (delta_x_px < 0)
        step_x_px = -1;
    if (delta_y_px > 0)
        step_y_px = 1;
    else if (delta_y_px < 0)
        step_y_px = -1;

    b->pixel_x += step_x_px * b->type->speed;
    b->pixel_y += step_y_px * b->type->speed;
}

void update_bots(void)
{
    for (int i = 0; i < MAX_BOTS; ++i)
        if (bots[i].active)
            update_bot(&bots[i]);
}

/**
 * @brief Draw a sprite for each active bot.
 */
void render_bots(void)
{
    for (int i = 0; i < MAX_BOTS; ++i)
    {
        if (!bots[i].active)
            continue;
        int pixel_x = bots[i].pixel_x;
        int pixel_y = bots[i].pixel_y;
        const bopti_image_t *sprite = bots[i].type ? bots[i].type->sprite : &spr_bot_1;
        dimage(pixel_x, pixel_y, sprite);
    }
}
