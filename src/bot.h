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
typedef struct Bot
{
    const BotType *type;         /**< Pointer to bot type definition. */
    int active;                  /**< 0 = free slot, 1 = in use. */
    int pixel_x, pixel_y;        /**< Pixel position (top-left or center). */
    int next_waypoint_index;     /**< Index of the next waypoint to reach. */
    int hp;                      /**< Current health points. */
    int tile_x, tile_y;          /**< Cached tile coordinates. */
    struct Bot *next_in_tile;    /**< Next bot in the same tile bucket. */
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

/**
 * @brief Retrieve the head of the bot list for a tile bucket.
 * @param tile_x Tile X coordinate.
 * @param tile_y Tile Y coordinate.
 * @return Pointer to the first bot in the bucket, or NULL.
 */
Bot *bot_first_in_tile(int tile_x, int tile_y);

/**
 * @brief Apply raw damage to a bot, despawning it when HP reaches zero.
 * @param bot Bot instance to damage.
 * @param damage Damage amount to subtract from bot HP.
 */
void bot_apply_damage(Bot *bot, int damage);
