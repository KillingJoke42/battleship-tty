#ifndef __ABILITIES_H__
#define __ABILITIES_H__

#include <playertools.h>
#include <stdint.h>

enum map_reveal_ori {
    UPLEFT = 0,
    UPRIGHT = 1,
    DOWNLEFT = 2,
    DOWNRIGHT = 3
};

void ReviveCell(server_t *server, uint8_t invoker_idx, uint8_t ack);
void ReviveShip(server_t *server, uint8_t invoker_idx, uint8_t ack);
void SimpleSkip(int *turn_counter);
void RandomCellReveal(server_t *server);
void DeployDecoy(server_t *server, uint8_t invoker_idx);
void ChunkReveal(server_t *server, uint8_t invoker_idx);
void GridSwap(server_t *server, uint8_t invoker_idx);
void MoveShip(server_t *server, uint8_t invoker_idx);

#endif