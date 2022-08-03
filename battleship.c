#include <battleship.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void phase_place_ships(char player_arr[][])
{
    char ship_row = 10, ship_col = 10, orientation = 2, ship_placed = 0;
    printf("Preparation Phase Begins.\n");
    printf("You have 1 min 30 s to place your ships, else unplaced ships will be placed randomly\n");

    for (int i = 0; i < NUM_SHIPS; i++)
    {
        printf("Placing %s.\n", ship_name_map[i]);
        while (!ship_placed)
        {
            while (ship_row < A || ship_row > J)
            {
                printf("Row (A-J): ");
                scanf("%c", &ship_row);
                ship_row = ship_row - 'A';
                if (ship_row < A || ship_row > J)
                    printf("Ship must be placed between row A to J\n");                
            }

            while (ship_col < 0 || ship_col > 9)
            {
                printf("Col (0-9): ");
                scanf("%d", &ship_col);
                if (ship_col < 0 || ship_col > 9)
                    printf("Ship must be placed between column 0 to 9\n");
            }

            while (orientation != 0 || orientation != 1)
            {
                printf("Orientation (Horizontal = 0/ Vertical = 1): ");
                scanf("%d", &orientation);
                if (orientation != 0 || orientation != 1)
                    printf("Orientation must be either 0 (Horizontal) or 1 (Vertical)\n");
            }

            if (player_arr[ship_row][ship_col])
            {
                printf("A ship has already been placed here.\n");
                ship_row = 10; ship_col = 10; orientation = 2;
                continue;
            }

            if ((orientation == 0 && 
                find_arr_sum(&player_arr[ship_row][ship_col], ship_sz_map[i])) || 
                (orientation == 1 && 
                find_vertical_sum(player_arr, ship_row, ship_col, ship_sz_map[i])))
            {
                printf("Cannot place ship here: collision with previously placed ship\n");
                ship_row = 10; ship_col = 10; orientation = 2;
                continue;
            }

            
            ship_placed = 1;
        }
    }
}

uint8_t find_arr_sum(char *arr, int range)
{
    char *ptr, uint8_t sum = 0;
    ptr = arr;
    
    for (int i = 0; i < range; i++)
    {
        sum += *(ptr + i);
    }

    return sum;
}

uint8_t find_vertical_sum(char arr[][], char ship_row, char ship_col, int range)
{
    uint8_t sum;

    for (int i = 0; i < range; i++)
    {
        sum += arr[ship_row + i][ship_col];
    }

    return sum;
}