
#include <model/Model.h>

#ifndef _Included_ReadData_H
#define _Included_ReadData_H
#ifdef __cplusplus
extern "C" {
#endif

float readFloatData(unsigned char *pData);

void setPosition(int position);

int getPosition();

byte readBoolean(byte* data);

byte readByte(byte* data);

float readFloat(byte* data);;

int readInt(byte* data);

short readShort(byte* data);

byte readUnsignedByte(byte* data);

word readUnsignedShort(byte* data);

void readString(byte* data, char* des, int len);

#ifdef __cplusplus
}
#endif
#endif
