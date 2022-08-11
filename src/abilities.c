#include <abilities.h>
#include <battleship.h>
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void ReviveCell(server_t * server, uint8_t invoker_idx, uint8_t ack)
{
    char ship_row, ship_col, shiptype;
    player_t *invoker_ptr = server->player_list[invoker_idx];
    
    do {
        char c;
        ship_row = 10; ship_col = 10;
        printf("Choose which cell to revive. You can only revive a hit cell.\n");

        ship_row = get_character("Row (A-J): ",
                                "You must fire between row A to J\n",
                                'A',
                                'J') - 'A';

        ship_col = get_character("Col (0-9): ",
                                "You must fire between column 0 to 9\n",
                                '0',
                                '9') - '0';

        shiptype = invoker_ptr->playerPlacement[ship_row][ship_col];

        if (shiptype >= 0)
            printf("Invalid cell. Is not a hit cell or no ship on selected cell");
    } while (shiptype >= 0);

    switch (shiptype)
    {
        case -(CARRIER + 1):
            invoker_ptr->playerPlacement[ship_row][ship_col] = (CARRIER + 1);
            invoker_ptr->player_ship_status.ship_health[CARRIER]--;
            break;

        case -(DESTROYER + 1):
            invoker_ptr->playerPlacement[ship_row][ship_col] = (DESTROYER + 1);
            invoker_ptr->player_ship_status.ship_health[DESTROYER]--;
            break; 

        case -(BATTLESHIP + 1):
            invoker_ptr->playerPlacement[ship_row][ship_col] = (BATTLESHIP + 1);
            invoker_ptr->player_ship_status.ship_health[BATTLESHIP]--;
            break;

        case -(SUBMARINE + 1):
            invoker_ptr->playerPlacement[ship_row][ship_col] = (SUBMARINE + 1);
            invoker_ptr->player_ship_status.ship_health[SUBMARINE]--;
            break;

        case -(PATROL_BOAT + 1):
            invoker_ptr->playerPlacement[ship_row][ship_col] = (PATROL_BOAT + 1);
            invoker_ptr->player_ship_status.ship_health[PATROL_BOAT]--;
            break;
    }

    if (ack == 1)
    {
        for (int i = 0; i < server->playercnt; i++)
        {
            if (i != invoker_idx)
            {
                server->player_list[i]->oppn_info[invoker_idx][ship_row][ship_col] = 'O';
            }
        }
        printf("%s has invoked REVIVE on %c-%d!\n", invoker_ptr->playerName, (ship_row + 'A'), ship_col);
        enter_wait_prompt("Press ENTER to continue");
        clrscr();
    }
    return;
}

void ReviveShip(server_t *server, uint8_t invoker_idx, uint8_t ack)
{
    char new_loc_row, new_loc_col, new_loc_ori;
    uint8_t downed = 0;
    printf("Choose which downed ship to revive\n");
    player_t *invoker_ptr = server->player_list[invoker_idx];
    for (int i = 0; i < NUM_SHIPS; i++)
    {
        if (is_ship_sunk(invoker_ptr, i))
        {
            printf("%d) %s\n", i, ship_name_map[i]);
            downed++;
        }
    }
    if (downed == 0)
    {
        printf("You have no downed ships to revive!!\n");
        return;
    }

    char choice;
    do {
        choice = get_character("", "Invalid Choice\n", '0', NUM_SHIPS + '0') - '0';
    } while (!is_ship_sunk(invoker_ptr, choice));
    
    printf("Enter new location for revived ship\n");
    while (1)
    {
        new_loc_row = get_character("Row (A-J): ",
                                        "Ship must be placed between row A to J\n",
                                        'A',
                                        'J') - 'A';
        new_loc_col = get_character("Col (0-9): ",
                                        "Ship must be placed between column 0 to 9\n",
                                        '0',
                                        '9') - '0';
        new_loc_ori = get_character("Orientation (Horizontal = 0/ Vertical = 1): ",
                                        "Orientation must be either 0 (Horizontal) or 1 (Vertical)\n",
                                        '0',
                                        '1') - '0';

        ship_loc_t loc_vector = (ship_loc_t){new_loc_row, new_loc_col, new_loc_ori};
        if (!check_availability(invoker_ptr, loc_vector, ship_sz_map[choice]))
            continue;

        break;
    }

    // change the invoker's ship placement
    for (int j = 0; j < ship_sz_map[choice]; j++)
    {
        char new_row_offset = new_loc_row + ((new_loc_ori == 0) ? 0 : j);
        char new_col_offset = new_loc_col + ((new_loc_ori == 0) ? j : 0);
        invoker_ptr->playerPlacement[new_row_offset][new_col_offset] = (choice + 1);
    }

    // change the other player's notes about the invoker
    if (ack == 1)
    {
        for (int i = 0; i < server->playercnt; i++)
        {
            if (i != invoker_idx)
            {
                for (int j = 0; j < ship_sz_map[choice]; j++)
                {
                    char old_ori = invoker_ptr->player_ship_status.ship_locs[choice].orientation;
                    char old_row = invoker_ptr->player_ship_status.ship_locs[choice].origin_row + ((old_ori == 0) ? 0 : j);
                    char old_col = invoker_ptr->player_ship_status.ship_locs[choice].origin_col + ((old_ori == 0) ? j : 0);
                    server->player_list[i]->oppn_info[invoker_idx][old_row][old_col] = 'O';
                }
            }
        }
    }

    invoker_ptr->player_ship_status.ship_health[choice] = 0;
    invoker_ptr->player_ship_status.ship_locs[choice].origin_row = new_loc_row;
    invoker_ptr->player_ship_status.ship_locs[choice].origin_col = new_loc_col;
    invoker_ptr->player_ship_status.ship_locs[choice].orientation = new_loc_ori;
}

void SimpleSkip(int *turn_counter)
{
    *(turn_counter) += 1;
    return;
}

void RandomCellReveal(server_t *server)
{
    srand(time(0));
    uint8_t rng_player = (rand() % server->playercnt);
    uint8_t rng_ship = (rand() % NUM_SHIPS);
    uint8_t rng_ship_cell_offset = (rand() % ship_sz_map[rng_ship]);
    uint8_t chosen_ship_orientation = server->player_list[rng_player]->player_ship_status.ship_locs[rng_ship].orientation;
    uint8_t chosen_ship_row = server->player_list[rng_player]->player_ship_status.ship_locs[rng_ship].origin_row + 
                                ((chosen_ship_orientation == 0) ? 0 : rng_ship_cell_offset);
    uint8_t chosen_ship_col = server->player_list[rng_player]->player_ship_status.ship_locs[rng_ship].origin_col + 
                                ((chosen_ship_orientation == 0) ? rng_ship_cell_offset : 0);
    printf("%s has a ship at cell %c-%d\n", server->player_list[rng_player]->playerName, chosen_ship_row + 'A', chosen_ship_col);
}

void DeployDecoy(server_t *server, uint8_t invoker_idx)
{
    player_t *invoker_ptr = server->player_list[invoker_idx];
    char decoy_row, decoy_col;

    printf("Choose location to place decoy\n");

    do {
        decoy_row = get_character("Row (A-J): ",
                                            "Ship must be placed between row A to J\n",
                                            'A',
                                            'J') - 'A';
        decoy_col = get_character("Col (0-9): ",
                                            "Ship must be placed between column 0 to 9\n",
                                            '0',
                                            '9') - '0';

        if (invoker_ptr->playerPlacement[decoy_row][decoy_col])
        {
            printf("A ship has already been placed here.\n");
            continue;
        }
        else
        {
            printf("%s has deployed a decoy!\n", invoker_ptr->playerName);
            invoker_ptr->playerPlacement[decoy_row][decoy_col] = (DECOY + 1);
            return;
        }
        break;
    } while (1);
}

void ChunkReveal(server_t *server, uint8_t invoker_idx)
{
    srand(time(0));
    player_t *invoker_ptr = server->player_list[invoker_idx];
    char reveal_row, reveal_col;
    char mask[NUM_ROWS][NUM_COLS] = {0};

    printf("Choose location to place reveal\n");

    reveal_row = get_character("Row (A-J): ",
                                        "Ship must be placed between row A to J\n",
                                        'A',
                                        'J') - 'A';
    reveal_col = get_character("Col (0-9): ",
                                        "Ship must be placed between column 0 to 9\n",
                                        '0',
                                        '9') - '0';

    mask[reveal_row][reveal_col] = 1;
    uint8_t mask_ori = (rand() % 4);
    switch (mask_ori)
    {
        case UPLEFT:
            mask[reveal_row - 1][reveal_col] = 1;
            mask[reveal_row][reveal_col - 1] = 1;
            mask[reveal_row - 1][reveal_col - 1] = 1;
            break;

        case UPRIGHT:
            mask[reveal_row - 1][reveal_col] = 1;
            mask[reveal_row][reveal_col + 1] = 1;
            mask[reveal_row - 1][reveal_col + 1] = 1;
            break;

        case DOWNLEFT:
            mask[reveal_row + 1][reveal_col] = 1;
            mask[reveal_row][reveal_col - 1] = 1;
            mask[reveal_row + 1][reveal_col - 1] = 1;
            break;

        case DOWNRIGHT:
            mask[reveal_row + 1][reveal_col] = 1;
            mask[reveal_row][reveal_col + 1] = 1;
            mask[reveal_row + 1][reveal_col + 1] = 1;
            break;
    }

    scalar_multiplication(&mask, &server->player_list[invoker_idx]->playerPlacement, 1);
    // print_player_placements(mask);
    printf("  ");
    for (int i = 0; i < NUM_COLS; i++)
        printf("%d ", i);
    printf("\n");
    for (int i = 0; i < NUM_ROWS; i++)
    {
        printf("%c ", i + 'A');
        for (int j = 0; j < NUM_COLS; j++)
        {
            printf("%c ", mask[i][j]);
        }
        printf("\n");
    }
    return;
}

void GridSwap(server_t *server, uint8_t invoker_idx)
{
    srand(time(0));
    uint8_t rng_player;

    do {
        rng_player = rand() % server->playercnt;
    } while (rng_player == invoker_idx);

    player_t *invoker_ptr = server->player_list[invoker_idx];
    player_t *chosen_player = server->player_list[rng_player];
    scalar_swap(invoker_ptr->playerPlacement, chosen_player->playerPlacement);
}

void MoveShip(server_t *server, uint8_t invoker_idx)
{
    player_t *invoker_ptr = server->player_list[invoker_idx];
    printf("Choose a ship to move (Note: you can only move damaged ships)\n");
    uint8_t damaged = 0;
    uint8_t choice;
    uint8_t new_loc_row, new_loc_col, new_loc_ori;
    
    for (int i = 0; i < NUM_SHIPS; i++)
    {
        if ((invoker_ptr->player_ship_status.ship_health[i] != 0) && 
            (!is_ship_sunk(invoker_ptr, i)))
        {
            printf("%d) %s\n", i, ship_name_map[i]);
            damaged++;
        }
    }
    if (damaged == 0)
    {
        printf("You don't have any damaged ships!!\n");
        return;
    }

    do {
        choice = get_character("", "Invalid Choice\n", '0', (NUM_SHIPS - 1) + '0') - '0';
    } while ((invoker_ptr->player_ship_status.ship_health[choice] == 0) || \
                is_ship_sunk(invoker_ptr, choice));

    printf("Enter new location for your ship\n");
    while (1)
    {
        new_loc_row = get_character("Row (A-J): ",
                                        "Ship must be placed between row A to J\n",
                                        'A',
                                        'J') - 'A';
        new_loc_col = get_character("Col (0-9): ",
                                        "Ship must be placed between column 0 to 9\n",
                                        '0',
                                        '9') - '0';
        new_loc_ori = get_character("Orientation (Horizontal = 0/ Vertical = 1): ",
                                        "Orientation must be either 0 (Horizontal) or 1 (Vertical)\n",
                                        '0',
                                        '1') - '0';

        ship_loc_t loc_vector = (ship_loc_t){new_loc_row, new_loc_col, new_loc_ori};
        if (!check_availability(invoker_ptr, loc_vector, ship_sz_map[choice]))
            continue;

        break;
    }

    for (int i = 0; i < ship_sz_map[choice]; i++)
    {
        char old_ori = invoker_ptr->player_ship_status.ship_locs[choice].orientation;
        char old_row = invoker_ptr->player_ship_status.ship_locs[choice].origin_row + ((old_ori == 0) ? 0 : i);
        char old_col = invoker_ptr->player_ship_status.ship_locs[choice].origin_col + ((old_ori == 0) ? i : 0);
        char new_row = new_loc_row + ((new_loc_ori == 0) ? 0 : i);
        char new_col = new_loc_col + ((new_loc_ori == 0) ? i : 0); 
        swapchars(&invoker_ptr->playerPlacement[new_row][new_col],
                    &invoker_ptr->playerPlacement[old_row][old_col]);
    }

    for (int i = 0; i < server->playercnt; i++)
    {
        if (i != invoker_idx)
        {
            for (int j = 0; j < ship_sz_map[choice]; j++)
            {
                char old_ori = invoker_ptr->player_ship_status.ship_locs[choice].orientation;
                char old_row = invoker_ptr->player_ship_status.ship_locs[choice].origin_row + ((old_ori == 0) ? 0 : j);
                char old_col = invoker_ptr->player_ship_status.ship_locs[choice].origin_col + ((old_ori == 0) ? j : 0);
                char new_row = new_loc_row + ((new_loc_ori == 0) ? 0 : j);
                char new_col = new_loc_col + ((new_loc_ori == 0) ? j : 0); 
                swapuint8(&server->player_list[i]->oppn_info[invoker_idx][new_row][new_col],
                            &server->player_list[i]->oppn_info[invoker_idx][old_row][old_col]);
            }
        }
    }

    invoker_ptr->player_ship_status.ship_locs[choice].origin_row = new_loc_row;
    invoker_ptr->player_ship_status.ship_locs[choice].origin_col = new_loc_col;
    invoker_ptr->player_ship_status.ship_locs[choice].orientation = new_loc_ori;
    return;
}