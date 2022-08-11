#include <battleship.h>
#include <playertools.h>
#include <utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void phase_place_ships(player_t *player)
{
    // printf("Preparation Phase Begins.\n");
    // printf("You have 1 min 30 s to place your ships, else unplaced ships will be placed randomly\n");

    for (int i = 0; i < NUM_SHIPS; i++)
    {
        // char ship_row = 10, ship_col = 10, orientation = 2;
        printf("Placing %s.\n", ship_name_map[i]);
        ship_loc_t loc_vector = get_loc_vector(player, i);

        player->player_ship_status.ship_locs[i].origin_row = loc_vector.origin_row;
        player->player_ship_status.ship_locs[i].origin_col = loc_vector.origin_col;
        player->player_ship_status.ship_locs[i].orientation = loc_vector.orientation;
        
        if (loc_vector.orientation == 0)
        {
            for (int j = 0; j < ship_sz_map[i]; j++)
            {
                player->playerPlacement[loc_vector.origin_row][loc_vector.origin_col + j] = (i + 1);
            }
        }
        else
        {
            for (int j = 0; j < ship_sz_map[i]; j++)
            {
                player->playerPlacement[loc_vector.origin_row + j][loc_vector.origin_col] = (i + 1);
            }
        }

        print_player_placements(player->playerPlacement);
        enter_wait_prompt("Press ENTER to continue");
        clrscr();
    }
}

uint8_t find_arr_sum(char *arr, int range)
{
    char *ptr;
    uint8_t sum = 0;
    ptr = arr;
    
    for (int i = 0; i < range; i++)
    {
        sum += *(ptr + i);
    }

    return sum;
}

uint8_t find_vertical_sum(char arr[NUM_ROWS][NUM_COLS], char ship_row, char ship_col, int range)
{
    uint8_t sum = 0;

    for (int i = 0; i < range; i++)
    {
        sum += arr[ship_row + i][ship_col];
    }

    return sum;
}

void print_player_placements(char player_arr[NUM_ROWS][NUM_COLS])
{
    printf("  ");
    for (int i = 0; i < NUM_COLS; i++)
        printf("%d ", i);
    printf("\n");
    for (int i = 0; i < NUM_ROWS; i++)
    {
        printf("%c ", i + 'A');
        for (int j = 0; j < NUM_COLS; j++)
        {
            printf("%d ", player_arr[i][j]);
        }
        printf("\n");
    }
}

void phase_fire(player_t *attacker, player_t *being_attacked)
{
    char c, ship_row = 10, ship_col = 10;
    printf("Firing at %s.\n", being_attacked->playerName);

    ship_row = get_character("Row (A-J): ",
                            "You must fire between row A to J\n",
                            'A',
                            'J') - 'A';

    ship_col = get_character("Col (0-9): ",
                            "You must fire between column 0 to 9\n",
                            '0',
                            '9') - '0';

    if (being_attacked->playerPlacement[ship_row][ship_col] <= 0)
    {
        printf("MISS\n");
        if (attacker->oppn_info[being_attacked->idx][ship_row][ship_col] != 'X')
            attacker->oppn_info[being_attacked->idx][ship_row][ship_col] = '-';
    }
    else
    {
        printf("HIT\n");
        switch (being_attacked->playerPlacement[ship_row][ship_col] - 1)
        {
            case CARRIER:
                being_attacked->player_ship_status.ship_health[CARRIER] += 1;
                being_attacked->playerPlacement[ship_row][ship_col] = -(CARRIER + 1);
                if (is_ship_sunk(being_attacked, 0))
                    printf("Enemy Carrier Sunk\n");
                break;

            case BATTLESHIP:
                being_attacked->player_ship_status.ship_health[BATTLESHIP] += 1;
                being_attacked->playerPlacement[ship_row][ship_col] = -(BATTLESHIP + 1);
                if (is_ship_sunk(being_attacked, 1))
                    printf("Enemy Battleship Sunk\n");
                break;

            case DESTROYER:
                being_attacked->player_ship_status.ship_health[DESTROYER] += 1;
                being_attacked->playerPlacement[ship_row][ship_col] = -(DESTROYER + 1);
                if (is_ship_sunk(being_attacked, 2))
                    printf("Enemy Destroyer Sunk\n");
                break;
                
            case SUBMARINE:
                being_attacked->player_ship_status.ship_health[SUBMARINE] += 1;
                being_attacked->playerPlacement[ship_row][ship_col] = -(SUBMARINE + 1);
                if (is_ship_sunk(being_attacked, 3))
                    printf("Enemy Submarine Sunk\n");
                break;

            case PATROL_BOAT:
                being_attacked->player_ship_status.ship_health[PATROL_BOAT] += 1;
                being_attacked->playerPlacement[ship_row][ship_col] = -(PATROL_BOAT + 1);
                if (is_ship_sunk(being_attacked, 4))
                    printf("Enemy Patrol Boat Sunk\n");
                break;
        }
        attacker->oppn_info[being_attacked->idx][ship_row][ship_col] = 'X';
    }
}

uint8_t is_ship_sunk(player_t *being_attacked, char ship_idx)
{
    switch (ship_idx)
    {
        case CARRIER:
            return (being_attacked->player_ship_status.ship_health[CARRIER] == SHIP_CARRIER_SZ);
            break;
        
        case BATTLESHIP:
            return (being_attacked->player_ship_status.ship_health[BATTLESHIP] == SHIP_BATTLESHIP_SZ);
            break;

        case DESTROYER:
            return (being_attacked->player_ship_status.ship_health[DESTROYER] == SHIP_DESTROYER_SZ);
            break;

        case SUBMARINE:
            return (being_attacked->player_ship_status.ship_health[SUBMARINE] == SHIP_SUBMARINE_SZ);
            break;

        case PATROL_BOAT:
            return (being_attacked->player_ship_status.ship_health[PATROL_BOAT] == SHIP_PATROL_BOAT_SZ);
            break;

        default:
            printf("Invalid Ship index\n");
            return 0;
            break;
    }
    return 0;
}

uint8_t all_ships_sunk(player_t *being_attacked)
{
    uint8_t sunkcnt = 0;
    for (int i = 0; i < NUM_SHIPS; i++)
    {
        sunkcnt += (is_ship_sunk(being_attacked, i));
    }
    return (sunkcnt == NUM_SHIPS);
}

uint8_t check_availability(player_t *player, ship_loc_t loc_vector, uint8_t ship_sz)
{
    uint8_t ship_row = loc_vector.origin_row;
    uint8_t ship_col = loc_vector.origin_col;
    uint8_t ship_ori = loc_vector.orientation;
    if (player->playerPlacement[ship_row][ship_col])
    {
        printf("A ship has already been placed here.\n");
        return 0;
    }

    if ((ship_ori == 0 &&
        (ship_col + ship_sz > NUM_COLS ||
        find_arr_sum(&(player->playerPlacement[ship_row][ship_col]), ship_sz))) || 
        (ship_ori == 1 &&
        (ship_row + ship_sz > NUM_ROWS ||
        find_vertical_sum(player->playerPlacement, ship_row, ship_col, ship_sz))))
    {
        printf("Cannot place ship here: invalid placement\n");
        return 0;
    }

    return 1;
}

ship_loc_t get_loc_vector(player_t *player, uint8_t shiptype)
{
    char new_loc_row, new_loc_col, new_loc_ori;
    ship_loc_t loc_vector;
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

        loc_vector = (ship_loc_t){new_loc_row, new_loc_col, new_loc_ori};
        if (!check_availability(player, loc_vector, ship_sz_map[shiptype]))
            continue;

        break;
    }
    return loc_vector;
}