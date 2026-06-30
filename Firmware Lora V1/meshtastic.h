#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "crypto.h"

#define MESH_FREQ         869.525f   
#define MESH_SF           11
#define MESH_BW           250.0f    
#define MESH_CR           5         
#define MESH_PREAMBLE     16
#define MESH_TX_POWER     22        
#define MESH_SYNC_WORD    0x2B       

#define BROADCAST_ADDR    0xFFFFFFFF

#define PORTNUM_TEXT      1
#define PORTNUM_NODEINFO  67

#define MAX_MSG_LEN       200
#define MAX_CALLSIGN_LEN  12

struct MeshPacketHeader {
  uint32_t to;        
  uint32_t from;       
  uint32_t id;       
  uint8_t  flags;       
  uint8_t  channel_id;
  uint8_t  portnum;   
  uint8_t  reserved;
};                 

#define HEADER_SIZE  16

struct MeshMessage {
  uint32_t from;
  uint32_t id;
  uint32_t timestamp;  
  char     sender[MAX_CALLSIGN_LEN + 1];
  char     text[MAX_MSG_LEN + 1];
  bool     outgoing;
  bool     acked;
};


inline int meshEncodeText(uint8_t* buf, size_t bufLen,
                           uint32_t fromId, uint32_t toId,
                           const char* text, uint32_t packetId,
                           const uint8_t* channelKey) {
  size_t textLen = strlen(text);
  if (textLen > MAX_MSG_LEN) textLen = MAX_MSG_LEN;
  size_t total = HEADER_SIZE + textLen;
  if (total > bufLen) return -1;

  MeshPacketHeader* hdr = (MeshPacketHeader*)buf;
  hdr->to         = __builtin_bswap32(toId);
  hdr->from       = __builtin_bswap32(fromId);
  hdr->id         = __builtin_bswap32(packetId);
  hdr->flags      = 0x03;        
  hdr->channel_id = 0x08;       
  hdr->portnum    = PORTNUM_TEXT;
  hdr->reserved   = 0x00;

  memcpy(buf + HEADER_SIZE, text, textLen);

  uint8_t nonce[8];
  nonce[0] = packetId & 0xFF;
  nonce[1] = (packetId >> 8) & 0xFF;
  nonce[2] = (packetId >> 16) & 0xFF;
  nonce[3] = (packetId >> 24) & 0xFF;
  nonce[4] = (fromId >> 24) & 0xFF;
  nonce[5] = (fromId >> 16) & 0xFF;
  nonce[6] = (fromId >> 8) & 0xFF;
  nonce[7] =  fromId & 0xFF;

  aesCTR(buf + HEADER_SIZE, textLen, channelKey, nonce);

  return (int)total;
}

inline bool meshDecodePacket(const uint8_t* buf, size_t len,
                              MeshMessage& out,
                              const uint8_t* channelKey) {
  if (len < HEADER_SIZE + 1) return false;

  const MeshPacketHeader* hdr = (const MeshPacketHeader*)buf;
  uint8_t portnum = hdr->portnum;
  if (portnum != PORTNUM_TEXT) return false;

  uint32_t fromId   = __builtin_bswap32(hdr->from);
  uint32_t packetId = __builtin_bswap32(hdr->id);

  size_t payloadLen = len - HEADER_SIZE;
  uint8_t payload[MAX_MSG_LEN + 1];
  if (payloadLen > MAX_MSG_LEN) payloadLen = MAX_MSG_LEN;
  memcpy(payload, buf + HEADER_SIZE, payloadLen);
  payload[payloadLen] = '\0';

  uint8_t nonce[8];
  nonce[0] = packetId & 0xFF;
  nonce[1] = (packetId >> 8) & 0xFF;
  nonce[2] = (packetId >> 16) & 0xFF;
  nonce[3] = (packetId >> 24) & 0xFF;
  nonce[4] = (fromId >> 24) & 0xFF;
  nonce[5] = (fromId >> 16) & 0xFF;
  nonce[6] = (fromId >> 8) & 0xFF;
  nonce[7] =  fromId & 0xFF;

  aesCTR(payload, payloadLen, channelKey, nonce);

  for (size_t i = 0; i < payloadLen; i++) {
    if (payload[i] != 0 && (payload[i] < 0x20 || payload[i] > 0x7E)) {
      return false; // Likely wrong key or corrupted
    }
  }

  out.from      = fromId;
  out.id        = packetId;
  out.outgoing  = false;
  out.acked     = false;
  snprintf(out.sender, MAX_CALLSIGN_LEN, "%08X", fromId);
  memcpy(out.text, payload, payloadLen);
  out.text[payloadLen] = '\0';

  return true;
}
