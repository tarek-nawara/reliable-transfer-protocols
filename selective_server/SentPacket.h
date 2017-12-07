/**
 *  File: SentPacket.h
 *  Description: Interface for an entity representing the sent
 *               packet to the client.
 *  Created at: 2017-12-8
 */

#ifndef SELECTIVE_SERVER_SENTPACKET_H
#define SELECTIVE_SERVER_SENTPACKET_H

#include "socket_utils.h"

class SentPacket {
public:
    utils::Packet *packet;
    time_t sent_time;

    SentPacket(utils::Packet *packet, time_t sent_time);
private:
};


#endif //SELECTIVE_SERVER_SENTPACKET_H
