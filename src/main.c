/**
 * @file main.c
 * @brief Entry point and high-level flow.
 */
#include <gint/display.h>
#include <gint/keyboard.h>
#include <string.h>
#include "menu.h"
#include "game.h"
#include "levels.h"

/**
 * @brief Program entry point.
 *
 * Displays the main menu, then reacts to the user's choice. The game returns
 * to this menu when the player quits in-game. The only definitive exit path is
 * choosing the Quit item in the menu (or pressing EXIT while in the menu).
 *
 * @return Status code (1 to keep OS menu from exiting immediately).
 */
int main(void)
{
    while (1)
    {
        /** Choice returned by the main menu.
         *  0 = Plus, 1 = Jouer, 2 = Quitter, -1 = EXIT from menu. */
        int menu_choice = run_menu();

        if (menu_choice == 0)
        {
            // Placeholder for future content in "Plus"
            dclear(C_WHITE);
            dtext(10, 28, C_BLACK, "Plus...");
            dupdate();
            getkey();
        }
        else if (menu_choice == 1)
        {
            // Let the player choose a level then start the game loop
            level_selector();
            // When game_loop() returns (EXIT or game over), control comes back
            // here and we show the menu again.
            game_loop();
        }
        else if (menu_choice == 2 || menu_choice == -1)
        {
            // Quit has been chosen from the menu, exit the program
            return 1;
        }
    }
}
