#ifndef __PLAYERTOOLS_H__
#define __PLAYERTOOLS_H__

#include <stdint.h>

#ifndef NUM_ROWS
#define NUM_ROWS    10
#endif

#ifndef NUM_COLS
#define NUM_COLS    10
#endif

#ifndef NUM_SHIPS
#define NUM_SHIPS   5
#endif

typedef struct {
    uint8_t origin_row;
    uint8_t origin_col;
    uint8_t orientation;
} ship_loc_t;

typedef struct {
    uint8_t ship_health[NUM_SHIPS];
    ship_loc_t ship_locs[NUM_SHIPS];
} ship_status_t;

typedef struct {
    char *playerName;
    uint8_t idx;
    char *playerPlacement;
    uint8_t **oppn_info;
    ship_status_t player_ship_status;
    uint8_t streak;
    uint8_t ability;
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
void player_init(player_t *player, char *player_init, uint8_t player_cnt, uint8_t player_idx);
char select_a_player(server_t *server, uint8_t invoker_idx);
void start_server(server_t *server);

void couch_multiplayer(void);
void lan_game(void);

void exit_free(server_t *server);
uint8_t game_over_base_case(server_t *server);

#endif