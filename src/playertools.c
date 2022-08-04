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

void couch_multiplayer(void)
{
    server_t *server = (server_t *)malloc(sizeof(server_t));
    create_server(server);

    for (int i = 0; i < server->playercnt; i++)
    {
        player_t *new_player = (player_t *)malloc(sizeof(player_t));
        player_init(new_player);
        new_player->idx = i;
        server->player_list[i] = new_player;
    }

    for (int i = 0; i < server->playercnt; i++)
    {
        clrscr();
        printf("Preparation phase for %s\n", server->player_list[i]->playerName);
        phase_place_ships(server->player_list[i]);
    }    

    while (!game_over_base_case(server))
    {
        for (int i = 0; i < server->playercnt; i++)
        {
            printf("%s's turn\n", server->player_list[i]->playerName);
            printf("What would you like to do?\n");
            printf("1) Fire on selected player\n");
            printf("2) View your map of selected player\n");
            printf("3) End round\n");
            char c, choice;
            while ((c = getchar()) != '\n' && c != EOF)
                choice = c;
            if (choice == '1')
            {
                printf("Choose player to fire on.\n");
                for (int j = 0; j < server->playercnt; j++)
                {
                    if (i != server->player_list[j]->idx)
                        printf("player %d: %s", server->player_list[j]->idx, server->player_list[j]->playerName);
                }
                printf("\n");
                while ((c = getchar()) != '\n' && c != EOF)
                    choice = c;
                choice = choice - '0';
                if (i == choice || choice > server->playercnt - 1)
                {
                    printf("Invalid Choice. Ending round for being a smartass\n");
                    continue;
                }
                phase_fire(server->player_list[choice]);
                printf("Ending round.\n");
            }
            else if (choice == '2')
            {
                // Implement this please
                printf("IMPLEMENT THIS!!!!\n");
            }
            else
                continue;

        }
    }
}

void lan_game(void)
{
    printf("Feature yet to be implemented\n");
}

uint8_t game_over_base_case(server_t *server)
{
    uint8_t sunk_cnt = 0;

    for (int i = 0; i < server->playercnt; i++)
    {
        sunk_cnt += all_ships_sunk(server->player_list[i]);
    }

    return (sunk_cnt == server->playercnt - 1);
}