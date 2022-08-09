#include <playertools.h>
#include <battleship.h>
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

void player_init(player_t *player, uint8_t players_cnt, uint8_t player_idx)
{
    char c;
    char* name;
    
    player->idx = player_idx;

    name = get_string("Enter player name: ");
    player->playerName = name;

    player->oppn_info = (uint8_t ***)malloc(sizeof(uint8_t **) * players_cnt);
    for (int i = 0; i < players_cnt; i++)
    {
        if (i != player_idx)
        {
            player->oppn_info[i] = alloc_dynamic_2d_arr(NUM_ROWS, NUM_COLS);
            clear_2d_arr(player->oppn_info[i], 'O', NUM_ROWS, NUM_COLS);
            //memset(player->oppn_info[i], 0, sizeof(uint8_t) * NUM_ROWS * NUM_COLS);
        }
        else
            player->oppn_info[i] = NULL;
    }

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
    player_init(host, server->playercnt, 0);

    server->player_list[0] = host;
    server->joined_players = 1;

    enter_wait_prompt("Press ENTER to start server");
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
        player_init(new_player, server->playercnt, i);
        // new_player->idx = i;
        server->player_list[i] = new_player;
    }

    for (int i = 0; i < server->playercnt; i++)
    {
        clrscr();
        printf("Preparation phase for %s\n", server->player_list[i]->playerName);
        phase_place_ships(server->player_list[i]);
    }    

    while (1)
    {
        for (int i = 0; i < server->playercnt; i++)
        {
            if (game_over_base_case(server))
                goto game_over;
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
                // Make function from this. Redundant code
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
                    printf("Invalid Choice\n");
                    i--;
                    continue;
                }
                phase_fire(server->player_list[i], server->player_list[choice]);
            }
            else if (choice == '2')
            {
                // Make function from this. Redundant code.
                printf("Choose player.\n");
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
                    printf("Invalid Choice\n");
                    i--;
                    continue;
                }

                clrscr();
                // Print API locked to static 2d arrs. Fix Please!!
                printf("  ");
                for (int format = 0; format < NUM_COLS; format++)
                    printf("%d ", format);
                printf("\n");
                for (int row = 0; row < NUM_ROWS; row++)
                {
                    printf("%c ", row + 'A');
                    for (int col = 0; col < NUM_COLS; col++)
                    {
                        printf("%c ", server->player_list[i]->oppn_info[choice][row][col]);
                    }
                    printf("\n");
                }

                enter_wait_prompt("Press ENTER to continue\n");
                clrscr();
                i--;
            }
            else if (choice == '3')
            {
                continue;
            }
            else
            {
                printf("Invalid Choice\n");
                i--;
                continue;
            }

        }
    }
game_over:
    printf("GAME OVER\n");
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