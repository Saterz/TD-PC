/**
 * @file turret.h
 * @brief Tower types, tower grid and building rules.
 */
#pragma once
#include <stdint.h>
#include "bot.h"
#include "game.h"

/** Grid of player-built towers; non-zero means a tower is present. */
extern uint8_t towers[MAP_H][MAP_W];

/**
 * @brief Definition of a tower archetype.
 */
typedef struct
{
    const char *name;               /**< Display name. */
    int range_px;                   /**< Attack range in pixels. */
    int fire_cooldown_ticks;        /**< Cooldown between shots in ticks. */
    int damage;                     /**< Damage per shot. */
    const bopti_image_t *sprite;    /**< Sprite used to render the tower. */
} TowerType;

/**
 * @brief Runtime tower instance.
 */
typedef struct
{
    const TowerType *type;          /**< Pointer to tower type. */
    int x, y;                       /**< Grid coordinates. */
    int cd;                         /**< Remaining cooldown ticks. */
} Tower;

/**
 * @brief Check if a tower can be built at the given grid cell.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return 1 if the cell is buildable, 0 otherwise.
 */
int can_build(int grid_x, int grid_y);
