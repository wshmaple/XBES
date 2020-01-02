#ifndef _COMDEF_H_
#define _COMDEF_H_

/*函数中需要用到的整型位数定义*/
typedef  unsigned       char uint8_t;
typedef  unsigned short int  uint16_t;
typedef  unsigned       int  uint32_t;
typedef    signed       char int8_t;
typedef    signed short int  int16_t;
typedef    signed       int  int32_t;

/*函数中需要用到的整型位数定义*/
typedef  unsigned       char uint8_t;
typedef  unsigned short int  uint16_t;
typedef  unsigned       int  uint32_t;
typedef    signed       char int8_t;
typedef    signed short int  int16_t;
typedef    signed       int  int32_t;
/* 是否启用Active取值 */
enum Active
{
	ACTIVE_DISABLE = 0,  //禁止
	ACTIVE_ENABLE        //使能
};
/*DDC错误码取值和模块的错误码取值*/
enum ErrorCode
{
	ERROR_CODE_OK = 0x00,//没有错误
	ERROR_CODE_OFF_LINE = 0x01,//掉线
	ERROR_CODE_INIT_FAILED = 0x02,//初始化失败
};

#endif