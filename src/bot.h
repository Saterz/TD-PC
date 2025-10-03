/**
 * @file bot.h
 * @brief Bot types, instances and bot-related API.
 */
#pragma once
#include <gint/display.h>

/**
 * @brief Static definition of a bot archetype (speed, HP, sprite).
 */
typedef struct {
    const char *name;              /**< Display name of the bot type. */
    int max_hp;                    /**< Maximum health points. */
    int speed;                     /**< Speed in pixels per update tick. */
    const bopti_image_t *sprite;   /**< Sprite used to render the bot. */
} BotType;

// --- Bots following waypoints ---
/**
 * @brief Runtime bot instance following a path of waypoints.
 */
typedef struct
{
    const BotType *type;       /**< Pointer to bot type definition. */
    int active;                 /**< 0 = free slot, 1 = in use. */
    int pixel_x, pixel_y;       /**< Pixel position (top-left or center). */
    int next_waypoint_index;    /**< Index of the next waypoint to reach. */
} Bot;

/**
 * @brief Allocate a bot from the pool and place it at the spawn.
 */
void spawn_bot(void);

/**
 * @brief Update all active bots (movement and goal checks).
 */
void update_bots(void);

/**
 * @brief Render all active bots.
 */
void render_bots(void);
