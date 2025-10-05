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
    int center_x_px;                /**< Precomputed center X in pixels. */
    int center_y_px;                /**< Precomputed center Y in pixels. */
    int range_tiles;                /**< Detection radius in tiles. */
    int cooldown;                   /**< Remaining cooldown ticks. */
} Tower;

/**
 * @brief Check if a tower can be built at the given grid cell.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return 1 if the cell is buildable, 0 otherwise.
 */
int can_build(int grid_x, int grid_y);

/**
 * @brief Attempt to place a tower at the given grid coordinates.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 * @return 1 on success, 0 otherwise.
 */
int build_turret(int grid_x, int grid_y);

/**
 * @brief Remove an existing tower from the map, if present.
 * @param grid_x X coordinate in grid units.
 * @param grid_y Y coordinate in grid units.
 */
void remove_turret(int grid_x, int grid_y);

/**
 * @brief Update a single tower, acquiring and firing at nearby bots.
 * @param tower Tower instance to update.
 */
void update_turret(Tower *tower);

/**
 * @brief Update all active towers, making them acquire and shoot targets.
 */
void update_all_turrets(void);
