#ifndef __ABILITIES_H__
#define __ABILITIES_H__

#include <playertools.h>
#include <stdint.h>

void ReviveCell(server_t *server, uint8_t invoker_idx, uint8_t ack);
void ReviveShip(server_t *server, uint8_t invoker_idx, uint8_t ack);
void SimpleSkip(int *turn_counter);
void RandomCellReveal(server_t *server);

#endif