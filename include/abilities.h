#ifndef __ABILITIES_H__
#define __ABILITIES_H__

#include <playertools.h>
#include <stdint.h>

enum ability_list {
    SIMPLE_SKIP,
    REVIVE_CELL,
    RANDOM_CELL_REVEAL,
    MOVE_SHIP,
    CHUNK_REVEAL,
    DEPLOY_DECOY,
    REVIVE_SHIP,
    GRID_SWAP,
    REVEAL_PLAYER_PLACEMENT,
};

enum map_reveal_ori {
    UPLEFT = 0,
    UPRIGHT = 1,
    DOWNLEFT = 2,
    DOWNRIGHT = 3
};

static char ability_list[][24] = {"Simple Skip",
                            "Revive Cell",
                            "Random Cell Reveal",
                            "Move Ship",
                            "Chunk Reveal",
                            "Deploy Decoy",
                            "Revive Ship",
                            "Grid Swap",
                            "Reveal Player Placement"};

#define NUM_THREE_STREAK    2
#define NUM_FIVE_STREAK     4
#define NUM_SEVEN_STREAK    3

void ReviveCell(server_t *server, uint8_t invoker_idx, uint8_t ack);
void ReviveShip(server_t *server, uint8_t invoker_idx, uint8_t ack);
void SimpleSkip(server_t *server, int *turn_counter);
void RandomCellReveal(server_t *server);
void DeployDecoy(server_t *server, uint8_t invoker_idx);
void ChunkReveal(server_t *server, uint8_t invoker_idx);
void GridSwap(server_t *server, uint8_t invoker_idx);
void MoveShip(server_t *server, uint8_t invoker_idx);
void RevealPlayerPlacement(server_t *server, uint8_t invoker_idx);

uint8_t attach_ability(player_t *player);
void prompt_ability_gain(player_t *player);
void execute_ability(server_t *server, int *invoker_idx);
#endif