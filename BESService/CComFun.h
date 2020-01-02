#pragma once

/**
 * A com fun. 公用的函数接口
 *
 * @author WSH
 * @date 2018/9/11
 */

class CComFun
{
public:
	CComFun();
	~CComFun();
	uint16_t BES200_CRC_16(const char *cBuf, int cLen);
	uint16_t BES200_htons(uint16_t n);
	uint16_t BES200_ntohs(uint16_t n);
	uint32_t BES200_htonl(uint32_t n);
	int32_t BES200_htonl2(int32_t n);
	uint32_t BES200_ntohl(uint32_t n);

};

