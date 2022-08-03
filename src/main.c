#include <morse_interpreter.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tcptools.h>
#include <battleship.h>

int main(void)
{
    player_t *player = (player_t *)malloc(sizeof(player_t));
    memset(player->playerPlacement, 0, sizeof(player->playerPlacement));
    phase_place_ships(player);

    return 0;
}