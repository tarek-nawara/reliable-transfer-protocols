//
// Created by tarek on 12/7/17.
//

#include "SentPacket.h"

SentPacket::SentPacket(utils::Packet *packet, time_t sent_time) {
    this->packet = packet;
    this->sent_time = sent_time;
}
