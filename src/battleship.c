#include <battleship.h>
#include <playertools.h>
#include <utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void phase_place_ships(player_t *player)
{
    printf("Preparation Phase Begins.\n");
    // printf("You have 1 min 30 s to place your ships, else unplaced ships will be placed randomly\n");

    for (int i = 0; i < NUM_SHIPS; i++)
    {
        char ship_row = 10, ship_col = 10, orientation = 2;
        printf("Placing %s.\n", ship_name_map[i]);
        while (1)
        {
            char c;
            while (ship_row < A || ship_row > J)
            {
                printf("Row (A-J): ");
                while((c = getchar()) != '\n' && c != EOF)
                    ship_row = c - 'A';
                if (ship_row < A || ship_row > J)
                    printf("Ship must be placed between row A to J\n");                
            }

            while (ship_col < 0 || ship_col > 9)
            {
                printf("Col (0-9): ");
                while((c = getchar()) != '\n' && c != EOF)
                    ship_col = c - '0';
                if (ship_col < 0 || ship_col > 9)
                    printf("Ship must be placed between column 0 to 9\n");
            }

            while (orientation != 0 && orientation != 1)
            {
                printf("Orientation (Horizontal = 0/ Vertical = 1): ");
                while((c = getchar()) != '\n' && c != EOF)
                    orientation = c - '0';
                if (orientation != 0 && orientation != 1)
                    printf("Orientation must be either 0 (Horizontal) or 1 (Vertical)\n");
            }

            if (player->playerPlacement[ship_row][ship_col])
            {
                printf("A ship has already been placed here.\n");
                ship_row = 10; ship_col = 10; orientation = 2;
                continue;
            }

            printf("%d\n", ship_col + ship_sz_map[i] > NUM_COLS);
            if ((orientation == 0 &&
                (ship_col + ship_sz_map[i] > NUM_COLS ||
                find_arr_sum(&(player->playerPlacement[ship_row][ship_col]), ship_sz_map[i]))) || 
                (orientation == 1 &&
                (ship_row + ship_sz_map[i] > NUM_ROWS ||
                find_vertical_sum(player->playerPlacement, ship_row, ship_col, ship_sz_map[i]))))
            {
                printf("Cannot place ship here: invalid placement\n");
                ship_row = 10; ship_col = 10; orientation = 2;
                continue;
            }

            if (orientation == 0)
            {
                for (int j = 0; j < ship_sz_map[i]; j++)
                {
                    player->playerPlacement[ship_row][ship_col + j] = (i + 1);
                }
            }
            else
            {
                for (int j = 0; j < ship_sz_map[i]; j++)
                {
                    player->playerPlacement[ship_row + j][ship_col] = (i + 1);
                }
            }

            print_player_placements(player->playerPlacement);
            break;
        }
    }
}

uint8_t find_arr_sum(uint8_t *arr, int range)
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

uint8_t find_vertical_sum(uint8_t arr[NUM_ROWS][NUM_COLS], char ship_row, char ship_col, int range)
{
    uint8_t sum = 0;

    for (int i = 0; i < range; i++)
    {
        sum += arr[ship_row + i][ship_col];
    }

    return sum;
}

void print_player_placements(uint8_t player_arr[NUM_ROWS][NUM_COLS])
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

void phase_fire(player_t *being_attacked)
{
    char c, ship_row = 10, ship_col = 10;
    printf("Firing at %s.\n", being_attacked->playerName);

    while (ship_row < A || ship_row > J)
    {
        printf("Row (A-J): ");
        while((c = getchar()) != '\n' && c != EOF)
            ship_row = c - 'A';
        if (ship_row < A || ship_row > J)
            printf("You must fire between row A to J\n");                
    }

    while (ship_col < 0 || ship_col > 9)
    {
        printf("Col (0-9): ");
        while((c = getchar()) != '\n' && c != EOF)
            ship_col = c - '0';
        if (ship_col < 0 || ship_col > 9)
            printf("You must fire between column 0 to 9\n");
    }

    if (being_attacked->playerPlacement[ship_row][ship_col] == 0)
        printf("MISS\n");
    else
    {
        printf("HIT\n");
        switch (being_attacked->playerPlacement[ship_row][ship_col] - 1)
        {
            case CARRIER:
                being_attacked->player_ship_status.carrier += 1;
                if (is_ship_sunk(being_attacked, 0))
                    printf("Enemy Carrier Sunk\n");
                break;

            case BATTLESHIP:
                being_attacked->player_ship_status.battleship += 1;
                if (is_ship_sunk(being_attacked, 1))
                    printf("Enemy Battleship Sunk\n");
                break;

            case DESTROYER:
                being_attacked->player_ship_status.destroyer += 1;
                if (is_ship_sunk(being_attacked, 2))
                    printf("Enemy Destroyer Sunk\n");
                break;
                
            case SUBMARINE:
                being_attacked->player_ship_status.submarine += 1;
                if (is_ship_sunk(being_attacked, 3))
                    printf("Enemy Submarine Sunk\n");
                break;

            case PATROL_BOAT:
                being_attacked->player_ship_status.patrol_boat += 1;
                if (is_ship_sunk(being_attacked, 4))
                    printf("Enemy Patrol Boat Sunk\n");
                break;
        }
        being_attacked->playerPlacement[ship_row][ship_col] = 0;
    }
}

uint8_t is_ship_sunk(player_t *being_attacked, char ship_idx)
{
    switch (ship_idx)
    {
    case CARRIER:
        return (being_attacked->player_ship_status.carrier == SHIP_CARRIER_SZ);
        break;
    
    case BATTLESHIP:
        return (being_attacked->player_ship_status.battleship == SHIP_BATTLESHIP_SZ);
        break;

    case DESTROYER:
        return (being_attacked->player_ship_status.destroyer == SHIP_DESTROYER_SZ);
        break;

    case SUBMARINE:
        return (being_attacked->player_ship_status.submarine == SHIP_SUBMARINE_SZ);
        break;

    case PATROL_BOAT:
        return (being_attacked->player_ship_status.patrol_boat == SHIP_PATROL_BOAT_SZ);
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