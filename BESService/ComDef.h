#ifndef _COMDEF_H_
#define _COMDEF_H_

/*��������Ҫ�õ�������λ������*/
typedef  unsigned       char uint8_t;
typedef  unsigned short int  uint16_t;
typedef  unsigned       int  uint32_t;
typedef    signed       char int8_t;
typedef    signed short int  int16_t;
typedef    signed       int  int32_t;

/*��������Ҫ�õ�������λ������*/
typedef  unsigned       char uint8_t;
typedef  unsigned short int  uint16_t;
typedef  unsigned       int  uint32_t;
typedef    signed       char int8_t;
typedef    signed short int  int16_t;
typedef    signed       int  int32_t;
/* �Ƿ�����Activeȡֵ */
enum Active
{
	ACTIVE_DISABLE = 0,  //��ֹ
	ACTIVE_ENABLE        //ʹ��
};
/*DDC������ȡֵ��ģ��Ĵ�����ȡֵ*/
enum ErrorCode
{
	ERROR_CODE_OK = 0x00,//û�д���
	ERROR_CODE_OFF_LINE = 0x01,//����
	ERROR_CODE_INIT_FAILED = 0x02,//��ʼ��ʧ��
};

#endif