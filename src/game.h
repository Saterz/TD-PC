/**
 * @file game.h
 * @brief Core game constants and API.
 */
#pragma once
// Standard integer types
#include <stdint.h>

/** Logical screen width in pixels. */
#define SCREEN_W 128
/** Logical screen height in pixels. */
#define SCREEN_H 64

/** Size of a square map tile in pixels. */
#define TILE_SIZE 9

/** Map width in tiles (14 * 9 = 126 px). */
#define MAP_W 14
/** Map height in tiles (7 * 9 = 63 px). */
#define MAP_H 7

/**
 * @brief Remaining player lives.
 *
 * Decreases when a bot reaches the goal.
 */
extern int LIFES;

/**
 * @brief Read-only level layout, expressed as Tile IDs.
 */
extern const uint8_t level_tiles[MAP_H][MAP_W];

/**
 * @brief Check whether grid coordinates are inside the map bounds.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return 1 if in bounds, 0 otherwise.
 */
static inline int in_bounds(int grid_x, int grid_y)
{
    return grid_x >= 0 && grid_x < MAP_W && grid_y >= 0 && grid_y < MAP_H;
}

/**
 * @brief Enter the main game loop.
 * @return 0 when exiting the game.
 */
int game_loop();
