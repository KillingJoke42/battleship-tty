#ifndef __BATTLESHIP_H__
#define __BATTLESHIP_H__

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

char ship_name_map[][11] = {"carrier", "battleship", "destroyer", "submarine", "patrol boat"};
char ship_sz_map[] = {SHIP_CARRIER_SZ, SHIP_BATTLESHIP_SZ, SHIP_DESTROYER_SZ, SHIP_SUBMARINE_SZ, SHIP_PATROL_BOAT_SZ};

#define NUM_ROWS    10
#define NUM_COLS    10
#define NUM_SHIPS   5

typedef struct {
    char *playerName;
    uint8_t idx;
    uint8_t playerPlacement[NUM_ROWS][NUM_COLS];
    ship_status_t player_ship_status;
} player_t;

typedef struct {
    uint8_t carrier[SHIP_CARRIER_SZ];
    uint8_t battleship[SHIP_BATTLESHIP_SZ];
    uint8_t destroyer[SHIP_DESTROYER_SZ];
    uint8_t submarine[SHIP_SUBMARINE_SZ];
    uint8_t patrol_boat[SHIP_PATROL_BOAT_SZ];
} ship_status_t;

void phase_place_ships(char player_arr[][]);
uint8_t* phase_fire(void);
uint8_t find_arr_sum(char* arr, int range);
uint8_t find_vertical_sum(char arr[][], char ship_row, char ship_col, int range);
uint8_t is_ship_destroyed(ship_status_t *player_ship_status, char ship_type);
#endif