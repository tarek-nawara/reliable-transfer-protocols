/**
 *  File: SentPacket.cpp
 *  Description: implementation for SentPacket interface
 *  Created at: 2017-12-8
 */

#include "SentPacket.h"

SentPacket::SentPacket(utils::Packet *packet, time_t sent_time) {
    this->packet = packet;
    this->sent_time = sent_time;
}

SentPacket::~SentPacket() {
    delete packet;
}
