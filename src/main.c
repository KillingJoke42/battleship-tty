#include <battleship.h>
#include <playertools.h>
#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    // player_t *player = (player_t *)malloc(sizeof(player_t));
    // player_init(player);
    // phase_place_ships(player);
    // while(1)
    // {
    //     phase_fire(player);
    //     if (all_ships_sunk(player))
    //     {
    //         printf("All ships sunk for %s.\n", player->playerName);
    //         break;
    //     }
    // }
    // server_t *server = (server_t *)malloc(sizeof(server_t));
    // create_server(server);
    // start_server(server);
    // printf("Server Name: %s\n", server->server_name);
    // printf("Server Max players: %d\n", server->playercnt);
    // printf("Server joined players: %d\n", server->joined_players);
    // for (int i = 0; i < server->joined_players; i++)
    // {
    //     printf("Player %d name: %s\n", (i+1), server->player_list[i]->playerName);
    // }

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