#include "stdafx.h"
#include "CComFun.h"


CComFun::CComFun()
{
}


CComFun::~CComFun()
{
}

uint16_t CComFun::BES200_CRC_16(const char *cBuf, int cLen)
{
	uint16_t  i, j;
	uint16_t  c, crc = 0xFFFF;
	for (i = 0; i < cLen; i++)
	{
		c = cBuf[i] & 0x00FF;
		crc ^= c;
		for (j = 0; j < 8; j++)
		{
			if (crc & 0x0001)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else crc >>= 1;
		}
	}
	return(crc);
}

uint16_t CComFun::BES200_htons(uint16_t n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

uint16_t CComFun::BES200_ntohs(uint16_t n)
{
	return BES200_htons(n);
}

uint32_t CComFun::BES200_htonl(uint32_t n)
{
	return ((n & 0xff) << 24) |
		((n & 0xff00) << 8) |
		((n & 0xff0000UL) >> 8) |
		((n & 0xff000000UL) >> 24);
}
int32_t CComFun::BES200_htonl2(int32_t n)
{
	return ((n & 0xff) << 24) |
		((n & 0xff00) << 8) |
		((n & 0xff0000UL) >> 8) |
		((n & 0xff000000UL) >> 24);
}

uint32_t CComFun::BES200_ntohl(uint32_t n)
{
	return BES200_htonl(n);
}
