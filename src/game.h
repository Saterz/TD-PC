/**
 * @file game.h
 * @brief Core game constants and API.
 */
#pragma once
// Standard integer types
#include <stdint.h>
#include "map.h"

/**
 * @brief Remaining player lives.
 *
 * Decreases when a bot reaches the goal.
 */
extern int LIFES;

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
