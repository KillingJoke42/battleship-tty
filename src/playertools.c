#include <playertools.h>
#include <battleship.h>
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>

void create_server(server_t *server)
{
    char c;
    char* server_name;

    server_name = get_string("Enter server name: ");
    server->server_name = server_name;

    char* player_count = get_string("Number of players: ");
    // printf("Number of players: ");
    // while ((c = getchar()) != '\n' && c != EOF)
    
    server->playercnt = atoi(player_count);
    server->joined_players = 0;

    server->player_list = (player_t **)malloc(sizeof(player_t *) * server->playercnt);
    
    printf("Protect server? (Y/N): ");
    char choice;
    while ((c = getchar()) != '\n' && c != EOF)
        choice = c;
    if (choice == 'Y')
        protect_server(server);
}

void protect_server(server_t *server)
{
    struct termios t;
    tcgetattr(0, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &t);

    char* password;
    password = get_string("Enter server password: ");
    server->pass = password;

    printf("\n");
    tcgetattr(0, &t);
    t.c_lflag |= ECHO;
    tcsetattr(0, TCSANOW, &t);    
}

void player_init(player_t *player)
{
    char c;
    char* name;
    
    name = get_string("Enter player name: ");
    player->playerName = name;

    memset(player->playerPlacement, 0, sizeof(player->playerPlacement));

    player->player_ship_status.carrier = 0;
    player->player_ship_status.battleship = 0;
    player->player_ship_status.destroyer = 0;
    player->player_ship_status.submarine = 0;
    player->player_ship_status.patrol_boat = 0;
}

void start_server(server_t *server)
{
    char c;
    player_t *host = (player_t *)malloc(sizeof(player_t));
    player_init(host);

    server->player_list[0] = host;
    server->joined_players = 1;

    printf("Press ENTER to start.");
    while((c = getchar()) != '\n' && c != '\r');
    // Start looking for clients to join
    // Start the game now
    return;
}