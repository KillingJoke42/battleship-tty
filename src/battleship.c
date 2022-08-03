#include <battleship.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void phase_place_ships(player_t *player)
{
    printf("Preparation Phase Begins.\n");
    printf("You have 1 min 30 s to place your ships, else unplaced ships will be placed randomly\n");

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
                    player->playerPlacement[ship_row][ship_col + j] = 1;
                }
            }
            else
            {
                for (int j = 0; j < ship_sz_map[i]; j++)
                {
                    player->playerPlacement[ship_row + j][ship_col] = 1;
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