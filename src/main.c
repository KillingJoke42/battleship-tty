#include <morse_interpreter.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tcptools.h>
#include <battleship.h>

int main(void)
{
    player_t *player = (player_t *)malloc(sizeof(player_t));
    player_init(player);
    phase_place_ships(player);
    while(1)
    {
        phase_fire(player);
        if (all_ships_sunk(player))
        {
            printf("All ships sunk for %s.\n", player->playerName);
            break;
        }
    }
    return 0;
}