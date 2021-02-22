#ifndef CRC_H
#define CRC_H

unsigned long crc32_get(unsigned long crc, unsigned char *buf, unsigned int len);
unsigned short crc16(unsigned char *puchMsg,unsigned int usDataLen);

#endif

