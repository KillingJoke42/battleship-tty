#include <playertools.h>
#include <battleship.h>
#include <abilities.h>
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <time.h>

void create_server(server_t *server)
{
    char c;
    char* server_name;

    server_name = get_string("Enter server name: ");
    server->server_name = server_name;

    char* player_count = get_string("Number of players: ");
    
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
        }
        else
            player->oppn_info[i] = NULL;
    }

    memset(player->playerPlacement, 0, sizeof(player->playerPlacement));

    player->player_ship_status.ship_health[CARRIER] = 0;
    player->player_ship_status.ship_health[BATTLESHIP] = 0;
    player->player_ship_status.ship_health[DESTROYER] = 0;
    player->player_ship_status.ship_health[SUBMARINE] = 0;
    player->player_ship_status.ship_health[PATROL_BOAT] = 0;
}

char select_a_player(server_t *server, uint8_t invoker_idx)
{
    char c, choice;
    printf("Choose player\n");

    for (int j = 0; j < server->playercnt; j++)
    {
        if (invoker_idx != server->player_list[j]->idx)
            printf("player %d: %s\n", server->player_list[j]->idx, server->player_list[j]->playerName);
    }
    printf("\n");

    while ((c = getchar()) != '\n' && c != EOF)
        choice = c;
    choice = choice - '0';
    if (invoker_idx == choice || choice > server->playercnt - 1)
    {
        printf("Invalid Choice\n");
        return -1;
    }

    return choice;
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

    srand(time(0));
    uint8_t *rng_arr = alloc_rng_arr(server->playercnt);
    uint8_t server_sz = server->playercnt;
    for (int i = 0; i < server->playercnt; i++)
    {
        uint8_t rng_val = get_rng_val(rng_arr, &server_sz);
        player_t *new_player = (player_t *)malloc(sizeof(player_t));
        player_init(new_player, server->playercnt, rng_val);
        server->player_list[rng_val] = new_player;
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
            
            // Convert choice to switch case. Looks better
            switch (choice)
            {
                case '0':
                    RevealPlayerPlacement(server, i);
                    break;
                case '1':
                    if ((choice = select_a_player(server, i)) == -1)
                    {
                        printf("Invalid Choice\n");
                        i--;
                        continue;
                    }

                    phase_fire(server->player_list[i], server->player_list[choice]);
                    break;

                case '2':
                    if ((choice = select_a_player(server, i)) == -1)
                    {
                        printf("Invalid Choice\n");
                        i--;
                        continue;
                    }

                    clrscr();
                    // Print API locked to static 2d arrs. Fix Please!!
                    print_2d_dynamic_arr(server->player_list[i]->oppn_info[choice],
                                        NUM_ROWS,
                                        NUM_COLS);

                    enter_wait_prompt("Press ENTER to continue\n");
                    clrscr();
                    i--;
                    continue;
                    break;

                case '3':
                    break;

                default:
                    printf("Invalid Choice\n");
                    i--;
                    break;
            }
            enter_wait_prompt("Press ENTER to end round\n");
            clrscr();
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
        sunk_cnt += all_ships_sunk(server->player_list[i]);

    return (sunk_cnt == server->playercnt - 1);
}