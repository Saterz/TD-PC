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

/** Per-tile bot buckets for fast spatial queries. */
static Bot *g_bots_in_tile[MAP_H][MAP_W];

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
 * @brief Convert a pixel coordinate to its tile index.
 * @param px Pixel coordinate.
 * @return Tile coordinate obtained by integer division.
 */
static inline int px_to_tile(int px)
{
    return px / TILE_SIZE;
}

/**
 * @brief Check whether tile coordinates are within map bounds.
 * @param tx Tile X coordinate.
 * @param ty Tile Y coordinate.
 * @return 1 if coordinates are valid, 0 otherwise.
 */
static inline int tile_in_bounds(int tx, int ty)
{
    return tx >= 0 && tx < MAP_W && ty >= 0 && ty < MAP_H;
}

/**
 * @brief Link a bot into the bucket corresponding to tile coordinates.
 * @param b Bot instance to insert.
 * @param tx Tile X coordinate where the bot resides.
 * @param ty Tile Y coordinate where the bot resides.
 */
static void add_bot_to_tile(Bot *b, int tx, int ty)
{
    if (!tile_in_bounds(tx, ty))
    {
        b->tile_x = tx;
        b->tile_y = ty;
        b->next_in_tile = NULL;
        return;
    }
    b->tile_x = tx;
    b->tile_y = ty;
    b->next_in_tile = g_bots_in_tile[ty][tx];
    g_bots_in_tile[ty][tx] = b;
}

/**
 * @brief Remove a bot from the bucket stored in its cached tile coordinates.
 * @param b Bot instance to unlink.
 */
static void remove_bot_from_tile(Bot *b)
{
    int tx = b->tile_x;
    int ty = b->tile_y;
    if (!tile_in_bounds(tx, ty))
    {
        b->next_in_tile = NULL;
        return;
    }

    Bot *prev = NULL;
    for (Bot *cur = g_bots_in_tile[ty][tx]; cur; cur = cur->next_in_tile)
    {
        if (cur == b)
        {
            if (prev)
                prev->next_in_tile = cur->next_in_tile;
            else
                g_bots_in_tile[ty][tx] = cur->next_in_tile;
            b->next_in_tile = NULL;
            return;
        }
        prev = cur;
    }
}

/**
 * @brief Update cached tile coordinates when a bot moves in pixel space.
 * @param b Bot to update.
 * @param new_x_px New pixel X coordinate.
 * @param new_y_px New pixel Y coordinate.
 */
static void update_bot_tile_if_moved(Bot *b, int new_x_px, int new_y_px)
{
    int ntx = px_to_tile(new_x_px);
    int nty = px_to_tile(new_y_px);
    if (ntx == b->tile_x && nty == b->tile_y)
    {
        b->pixel_x = new_x_px;
        b->pixel_y = new_y_px;
        return;
    }

    remove_bot_from_tile(b);
    add_bot_to_tile(b, ntx, nty);
    b->pixel_x = new_x_px;
    b->pixel_y = new_y_px;
}

/**
 * @brief Despawn a bot, removing it from buckets and marking it inactive.
 * @param b Bot instance to deactivate.
 */
static void destroy_bot(Bot *b)
{
    if (!b || !b->active)
        return;
    remove_bot_from_tile(b);
    b->active = 0;
    b->hp = 0;
    b->type = NULL;
    b->next_in_tile = NULL;
    b->tile_x = -1;
    b->tile_y = -1;
}

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
            bots[i].hp = bots[i].type->max_hp;
            bots[i].tile_x = -1;
            bots[i].tile_y = -1;
            bots[i].next_in_tile = NULL;
            bots[i].next_waypoint_index = (WAYPOINT_COUNT > 1) ? 1 : 0;
            add_bot_to_tile(&bots[i], px_to_tile(spawn_pixel_x), px_to_tile(spawn_pixel_y));
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
        destroy_bot(b);
        return;
    }

    int target_pixel_x, target_pixel_y;
    waypoint_index_to_pixel(b->next_waypoint_index, &target_pixel_x, &target_pixel_y);

    int delta_x_px = target_pixel_x - b->pixel_x;
    int delta_y_px = target_pixel_y - b->pixel_y;

    // If close enough, snap and advance to next waypoint
    if (delta_x_px * delta_x_px + delta_y_px * delta_y_px <= 1)
    {
        update_bot_tile_if_moved(b, target_pixel_x, target_pixel_y);
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

    int new_x = b->pixel_x + step_x_px * b->type->speed;
    int new_y = b->pixel_y + step_y_px * b->type->speed;
    update_bot_tile_if_moved(b, new_x, new_y);
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

/**
 * @brief Retrieve the first bot stored in a tile bucket.
 * @param tile_x Tile X coordinate.
 * @param tile_y Tile Y coordinate.
 * @return Pointer to the first bot, or NULL when empty or out of bounds.
 */
Bot *bot_first_in_tile(int tile_x, int tile_y)
{
    if (!tile_in_bounds(tile_x, tile_y))
        return NULL;
    return g_bots_in_tile[tile_y][tile_x];
}

/**
 * @brief Apply damage to a bot and despawn it on zero HP.
 * @param bot Bot instance to damage.
 * @param damage Hit points to subtract.
 */
void bot_apply_damage(Bot *bot, int damage)
{
    if (!bot || !bot->active || damage <= 0)
        return;
    bot->hp -= damage;
    if (bot->hp <= 0)
        destroy_bot(bot);
}
