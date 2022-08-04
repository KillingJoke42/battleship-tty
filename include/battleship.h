#ifndef __BATTLESHIP_H__
#define __BATTLESHIP_H__

#include <playertools.h>
#include <stdint.h>

enum row_axis {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7,
    I = 8,
    J = 9
};

enum ships {
    CARRIER = 0,
    BATTLESHIP = 1,
    DESTROYER = 2,
    SUBMARINE = 3,
    PATROL_BOAT = 4
};

#define SHIP_CARRIER_SZ     5
#define SHIP_BATTLESHIP_SZ  4
#define SHIP_DESTROYER_SZ   3
#define SHIP_SUBMARINE_SZ   3
#define SHIP_PATROL_BOAT_SZ  2

static char ship_name_map[][11] = {"carrier", "battleship", "destroyer", "submarine", "patrol boat"};
static char ship_sz_map[] = {SHIP_CARRIER_SZ, SHIP_BATTLESHIP_SZ, SHIP_DESTROYER_SZ, SHIP_SUBMARINE_SZ, SHIP_PATROL_BOAT_SZ};

#define NUM_ROWS    10
#define NUM_COLS    10
#define NUM_SHIPS   5

void phase_place_ships(player_t *player);
void phase_fire(player_t *being_attacked);
uint8_t find_arr_sum(uint8_t* arr, int range);
uint8_t find_vertical_sum(uint8_t arr[NUM_ROWS][NUM_COLS], char ship_row, char ship_col, int range);
uint8_t is_ship_sunk(player_t *being_attacked, char ship_idx);
void print_player_placements(uint8_t player_arr[NUM_ROWS][NUM_COLS]);
uint8_t all_ships_sunk(player_t *being_attacked);
// char identify_hit_ship(player_t *being_attacked, char ship_row, char ship_col);
#endif