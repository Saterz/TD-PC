/**
 * @file main.c
 * @brief Entry point and high-level flow.
 */
#include <gint/display.h>
#include <gint/keyboard.h>
#include <string.h>
#include "menu.h"
#include "game.h"

/**
 * @brief Program entry point.
 * @return Status code (1 to keep OS menu from exiting immediately).
 */
int main(void)
{
    int choice = run_menu();

    dclear(C_WHITE);
    switch (choice)
    {
    case 0:
        dtext(10, 28, C_BLACK, "Plus...");
        break;
    case 1:
        game_loop();
        break;
    case 2:
        dtext(10, 28, C_BLACK, "Quit...");
        break;
    default:
        dtext(10, 28, C_BLACK, "Exit...");
        break;
    }
    dupdate();
    getkey();
    return 1;
}
