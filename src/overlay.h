/**
 * @file overlay.h
 * @brief In‑game overlay (HUD) API.
 */
#pragma once

/**
 * @brief Render the overlay on top of the game scene.
 *
 * Draws player lives, selector coordinates and a short controls hint. Should
 * be called after world rendering and before `dupdate()`.
 */
void overlay_draw(void);

