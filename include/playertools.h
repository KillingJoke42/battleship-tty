#ifndef __PLAYERTOOLS_H__
#define __PLAYERTOOLS_H__

#include <stdint.h>

#define NUM_ROWS    10
#define NUM_COLS    10

typedef struct {
    uint8_t carrier;
    uint8_t battleship;
    uint8_t destroyer;
    uint8_t submarine;
    uint8_t patrol_boat;
} ship_status_t;

typedef struct {
    char *playerName;
    uint8_t idx;
    uint8_t playerPlacement[NUM_ROWS][NUM_COLS];
    uint8_t ***oppn_info;
    ship_status_t player_ship_status;
} player_t;

typedef struct {
    char* server_name;
    uint8_t playercnt;
    char* pass;
    player_t** player_list;
    uint8_t joined_players;
} server_t;

void create_server(server_t *server);
void protect_server(server_t *server);
void player_init(player_t *player, uint8_t player_cnt, uint8_t player_idx);
void start_server(server_t *server);

void couch_multiplayer(void);
void lan_game(void);

uint8_t game_over_base_case(server_t *server);

#endif