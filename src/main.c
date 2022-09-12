#include <battleship.h>
#include <playertools.h>
#include <utils.h>

#ifdef GUI_ENABLE
#include <dedicatedGUI.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{

#ifdef GUI_ENABLE
    gtk_app_init(argc, argv);
    return 0;
#endif

    printf("battleship-tty\n");
    printf("Menu\n");
    printf("1) Start couch multiplayer\n");
    printf("2) Start local game server\n");
    printf("Specify choice: ");
    char c, choice;
    while ((c = getchar()) != '\n' && c != EOF)
        choice = c;
    if (choice == '1')
        couch_multiplayer();
    else
        lan_game();
    return 0;
}