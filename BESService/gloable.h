#ifndef _GLOABLE_
#define _GLOABLE_
#pragma once
#ifndef _JSON_HPP_H_ 

#define _JSON_HPP_H_ 
#include "json.hpp"
#include "csv.hpp"
#endif

namespace Gloable_Bes{


// �������� ���е� define ���������ô������ļ��н����޸�

#pragma pack(1)
#define MAX_ELECTRIC_COUNT  100      //����������
#define MAX_ELECTRIC_PARAM_COUNT  10 // ���ܲ��� ���� ��Ϊ���������Ӳ��д����,����Ҫ�������޸ģ�������øĳɿ������õ�
#define MAX_STORE_HOUR_COUNT 24



/*BES200���������붨��*/
enum Bes200ErrorCode{
	BES200_ERROR_OK = 0,			//�޴�����������
	BES200_ERROR_CONNECTFAILED,	    //���������������ʧ��
	BES200_ERROR_SENDFAILED,		//���ݷ���ʧ��
	BES200_ERROR_SENDTIMEOUT,		//���ͳ�ʱ
	BES200_ERROR_RECEIVEFAILED,		//���ݽ���ʧ��
	BES200_ERROR_RECEIVETIMEOUT,    //���ճ�ʱ
	BES200_ERROR_REFUSED,			//���󱻷������ܾ�
	BES200_ERROR_MALLOCERROR,		//����ռ����
	BES200_ERROR_SOCKETFAILED,		//����socket����
	BES200_ERROR_CRCERROR,			//У�鲻��
	BES200_ERROR_METERIDEXISTED,	//���ID�Ѿ�����
	BES200_ERROR_MAXMETERCOUNT,		//�����Ŀ�ﵽ����
	BES200_ERROR_METERID_NOTFOUND,	//���ID������
	BES200_ERROR_DATASIZEERR,		//���ݳ��Ȳ���
	BES200_ERROR_DATANOTRIGHT,		//���ݲ�����Ҫ��
	BES200_ERROR_FAILED,			//ʧ��
	BES200_ERROR_HEAD,				//����ͷ������
	BES200_ERROR_OTHERS,			//��������
};


/*************************************���ӿ⺯������*********************************/



typedef struct CollectControllerParameter{
	uint32_t 	ID;					//��������ID������Ψһ
	char     	Name[32];			//������������
	char     	Alias[64];			//������������֧�ֺ���
	char     	Description[128];	//��������������Ϣ��֧�ֺ���
	char     	Location[64];		//�������İ�װλ��
	char     	Zone[64];			//������Ϣ
	uint8_t 	Active;				//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 	//��������		   ��λΪ����
	uint8_t  	HisdataSavePeriod;	//��ʷ���ݱ�������  ��λΪСʱ
}_TAG_CollectControllerParameter;

#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE_TEST   sizeof(struct CollectControllerParameterTest)
typedef struct CollectControllerParameterTest {
	uint32_t 	ID;					//��������ID������Ψһ
	string     	Name;				//������������
	string     	Alias;				//������������֧�ֺ���
	string     	Description;		//��������������Ϣ��֧�ֺ���
	string     	Location;			//�������İ�װλ��
	string     	Zone;				//������Ϣ
	uint8_t 	Active;				//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 	//��������		   ��λΪ����
	uint8_t  	HisdataSavePeriod;	//��ʷ���ݱ�������  ��λΪСʱ
}_TAG_CollectControllerParameterTest;
REFLECTION(CollectControllerParameterTest, ID, Name, Alias, Description, Location, Zone, Active, SamplePeriod, HisdataSavePeriod)

struct CollectControllerInfo{
	char		Name[32];				//����������
	char		CampanyName[32];		//��˾����
	char		Model[32];				//�ͺ�
	uint8_t		HWVer;					//Ӳ���汾��
	uint8_t		FWVer;					//����汾��
	uint8_t		IP[4];					//IP��ַ
	uint8_t		Mask[4];				//Mask��ַ
	uint8_t		GateWay[4];				//����
	uint8_t		Mac[6];					//Mac��ַ
	uint32_t 	ID;					//��������ID������Ψһ
	char     	Alias[64];			//������������֧�ֺ���
	char     	Description[128];	//��������������Ϣ��֧�ֺ���
	char     	Location[64];		//�������İ�װλ��
	char     	Zone[64];			//������Ϣ
	uint8_t 	Active;				//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 	//��������		   ��λΪ����
	uint8_t  	HisdataSavePeriod;	//��ʷ���ݱ�������  ��λΪСʱ
};


/*************************************8********������*****************************************************************/
//��Ӧ17����Դ����
//enum MeterType
//{
//	METER_TYPE_ELETRIC = 0,					//���
//	METER_TYPE_WATER,						//ˮ��
//	METER_TYPE_GAS,							//ȼ��
//	METER_TYPE_CENTERAL_HEATING,			//���й�ů
//	METER_TYPE_CENTERAL_COOLING,			//���й���
//	METER_TYPE_ARTIFICAL,					//ȼú
//	METER_TYPE_LPGAS,						//Һ��ʯ����
//	METER_TYPE_ACGAS,						//�˹�ú��
//	METER_TYPE_GASOLINE, 					//����
//	METER_TYPE_KEROSENE,					//ú��
//	METER_TYPE_DIESELOIL,					//����
//	METER_TYPE_RENEWABLE_ENERGY, 			//������
//	METER_TYPE_OTHER						//�����ܺ�
//};
enum MeterType
{
	METER_TYPE_ELETRIC = 0,					//���
	METER_TYPE_ARTIFICAL,					//ȼú
	METER_TYPE_LPGAS,						//Һ��ʯ����
	METER_TYPE_ACGAS,						//�˹�ú��
	METER_TYPE_GASOLINE, 				    //����
	METER_TYPE_KEROSENE = 5,			    //ú��
	METER_TYPE_DIESELOIL,				    //����
	METER_TYPE_RENEWABLE_ENERGY, 		    //������
	METER_TYPE_COOL_WATER = 0x10,           //��ˮˮ��
	METER_TYPE_HOT_WATER = 0x11,            //������ˮˮ��
	METER_TYPE_DIRECT_WATER = 0x12,         //ֱ��ˮˮ��
	METER_TYPE_MIDDLE_WATER = 0x13,         //��ˮˮ��
	METER_TYPE_HEAT = 0x20,                 //��������������
	METER_TYPE_COLD = 0x21,                 //��������������
	METER_TYPE_GAS = 0x30,                  //ȼ����
	METER_TYPE_OTHER = 50				    //����
};
enum ComRate
{
	COMRATE_600 = 0,					//������ 600bps
	COMRATE_1200,						//������ 1200bps
	COMRATE_2400,						//������ 2400bps
	COMRATE_4800,						//������ 4800bps
	COMRATE_7200,						//������ 7200bps
	COMRATE_9600,						//������ 9600bps
	COMRATE_19200,						//������ 19200bps
	COMRATE_115200,						//������ 115200bps
	COMRATE_OTHER
};
enum ComPort{
	COMPORT_ONE = 0,					//�˿�1
	COMPORT_TWO,						//�˿�2
	COMPORT_THREE,						//�˿�3
	COMPORT_FOUR						//�˿�4
};
//enum ComAgreementType{
//	COMAGREEMENTTYPE_DLT97 = 0,			//Э��dlt97
//	COMAGREEMENTTYPE_DLT07,				//Э��dlt07
//	COMAGREEMENTTYPE_MODBUS,			//modbus
//	COMAGREEMENTTYPE_OTHER
//};
enum ComAgreementType {
	COMAGREEMENTTYPE_DLT97 = 0,			//Э��dlt97
	COMAGREEMENTTYPE_DLT07,				//Э��dlt07
	COMAGREEMENTTYPE_MODBUS,			//modbus
	COMAGREEMENTTYPE_CJT188,            //��׼188Э��
	COMAGREEMENTTYPE_SIMPLE_CJT188,     //����188Э��
	COMAGREEMENTTYPE_EN1434,            //ŷ��1434Э��
	COMAGREEMENTTYPE_W645,              //ˮ��645Э��
	COMAGREEMENTTYPE_SDJYL_WaterMeter,					//add 20190713
	COMAGREEMENTTYPE_OTHER
};
//�ܺĵ���ܺ�����
struct MeterElectricData{
	uint32_t 	MeterID;			    //������к�
	uint32_t 	ElectricData[MAX_ELECTRIC_COUNT];		//���һ�ٸ����ܲ���
	uint8_t     CollectCount;		    //ʵ�ʲɼ��ĵ��ܲ�������
	uint8_t  	DateYear;				//�����꣬��14
	uint8_t  	DateMonth; 				//�����£�1-12
	uint8_t  	DateDay; 				//�����գ�1-31
	uint8_t  	TimeHour;				//ʱ��ʱ��0-23
	uint8_t  	TimeMinute; 		    //ʱ���, 0-59
	uint8_t 	TimeSecond; 			//ʱ����, 0-59
};
//�ܺĵ���ܺĲ���
enum DataEncodeType{
	DATA_ENCODETYPE_BCD = 0,
	DATA_ENCODETYPE_DEC,
	DATA_ENCODETYPE_OTHER
};

struct ElectricDataInfo{
	uint32_t 	ElectricID;			//�ܺĲ��� ID
	char    	ElectricName[32];	//�ܺ�����
	uint32_t 	OffsetAddr;      	//�Ĵ���ƫ�Ƶ�ַ 
	uint8_t 	DataLength;			//���ݳ��� �ֽ���		
	uint8_t 	DataEncodeType;	    //������� BCD���룬������10���Ʊ���
	char  	    UintType[32];	    //���չ淶119ҳ�ı��������
	uint8_t		PointLocation;		//С����λ�� ��������
};

//���ɼ�����
struct ElectricDataCollectMethod{
	uint32_t CollectMethodID;						//�ɼ�����ID
	struct ElectricDataInfo  ElectricDataInfo[MAX_ELECTRIC_COUNT];	//���ɼ�һ�ٸ�������
	uint8_t CollectCount;							//Ҫ�ɼ���ʵ�ʵĵ�����

};
//�ܺĵ�����ò��� 
struct MeterParameter{
	uint32_t	MeterID;					//�ܺĵ�����к�
	uint8_t		MeterType;					//������� 		 
	uint8_t		ComRate;					//ͨ������ 		 
	uint8_t		ComPort;					//ͨ�Ŷ˿ں�	     
	uint8_t		ComAgreementType;			//ͨ�Ź�Լ����	 
	uint8_t		PhyAddr[7];					//�����ַ	����BCD�������ʽ	���չ淶 136ҳ ���ո�ʽ03 BYTE1-BYTE6   ����λ������ ��ʮ��ǧд����
	char		Alias[64];					//��ı���
	char		Description[128];			//���������Ϣ
	char		Location[64];				//���İ�װλ��
	uint8_t		Active;						//����ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
	struct  ElectricDataCollectMethod CollectMethod;//�ɼ�����
};
//��ʷ���� ����ܺ�����
struct MeterElectricHisData{
	uint32_t 	MeterID;			    						   //������к�
	uint32_t 	ElectricData[MAX_ELECTRIC_PARAM_COUNT];			   //���һ�ٸ����ܲ���
	uint8_t     CollectCount;		    						   //ʵ�ʲɼ��ĵ��ܲ�������
	uint8_t  	DateYear;										   //��ʼ�����꣬��14
	uint8_t  	DateMonth; 										   //��ʼ�����£�1-12
	uint8_t  	DateDay; 										   //��ʼ�����գ�1-31
	uint8_t  	TimeHour;										   //��ʼʱ��ʱ��0-23
	uint8_t  	TimeMinute; 									   //��ʼʱ���, 0-59
	uint8_t 	TimeSecond; 									   //��ʼʱ����, 0-59
};
//��ʷ���� ��һ�����һ������ݣ������24Сʱ��
struct MeterHisElectricData{
	uint32_t 										MeterID;			    	   //������к�
	struct MeterElectricHisData MeterElectricData[MAX_STORE_HOUR_COUNT];		   //�����һ��Сʱһ������
	uint8_t 										HisDataHourCount;			   //Сʱ����
};

/*********************************************************���������Ϣ*******************************************************/
enum AlarmInforCategory{
	CATEGORY_SYSTEM = 0,			//ϵͳ�¼�
	CATEGORY_METER,					//����¼�
	CATEGORY_OTHER
};
enum AlarmInforType{
	ALARMINFO_TYPE_NANDERROR = 0,			//�洢ʧ��
	ALARMINFO_TYPE_BATTERY_UNDER_VOLTAGE,	//���Ƿѹ
	ALARMINFO_TYPE_COMBREAK,				//���ͨ��ʧ��  �����ͨ�ſ��Ժ��ԣ���Ϊʵʱ��ѯ��û������
	ALARMINFO_TYPE_RESET,					//ϵͳ��λ
	ALARMINFO_TYPE_NONE,					//û�б�����Ϣ  
	ALARMINFO_TYPE_OTHER					//����
};

struct AlarmInfor{
	uint32_t 	AlarmInforID;				    //������Ϣ���к�
	uint8_t  	AlarmInforDateYear;				//��ʼ�����꣬��14
	uint8_t  	AlarmInforDateMonth; 			//��ʼ�����£�1-12
	uint8_t  	AlarmInforDateDay; 				//��ʼ�����գ�1-31
	uint8_t     AlarmInforTimeHour;				//��ʼʱ��ʱ��0-23
	uint8_t     AlarmInforTimeMinute; 			//��ʼʱ���, 0-59
	uint8_t     AlarmInforTimeSecond; 			//��ʼʱ����, 0-59
	uint8_t 	AlarmInforCategory;				//�¼�����
	uint16_t 	AlarmInforSeqNum;				//�Ǳ����к� ����  Ϊ�ɼ�����ID
	uint8_t 	AlarmInforType;					//�¼�����
};


/////////////////////////////////////////////////////////////////
////////////////////////////
//////////////////
/////////////  begp

#define  SENDBUF_SIZE   1*1024*1024
#define  RECVBUF_SIZE   1*1024*1024

#define   HISDATA_SENDBUF_SIZE    1*1024*1024
#define   HISDATA_RECVBUF_SIZE    1*1024*1024

#define   CONTROL_MODE_SIZE    10
#define   COLLECT_MODE_SIZE    10
#define   CONTROL_POINT_SIZE    100
#define   COLLECT_POINT_SIZE    100

#define   MAX_STORE_HOUR_COUNT   24

/* *****************************************************************����ͨ�Ź����� **************************************/
typedef enum
{
	/* �Ͳɼ������ */
	BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER = 0,				//����һ��������
	BEGP_FUNCODE_SET_CONTROLLER_PARAMETER,					//���ÿ����������в���
	BEGP_FUNCODE_GET_CONTROLLER_PARAMETER,					//��ȡ�����������в���
	BEGP_FUNCODE_SET_CONTROLLER_TIME,					    //���ÿ�������ʱ��
	BEGP_FUNCODE_GET_CONTROLLER_TIME,				        //��ȡ��������ʱ��
	BEGP_FUNCODE_REBOOT_CONTROLLER,						    //�������������൱��������λ
	BEGP_FUNCODE_RESET_CONTROLLER,							//���ÿ��������ָ��������ã�������
	BEGP_FUNCODE_DELETE_CONTROLLER,						    //ɾ��һ������������ɾ��������صĵ��
	BEGP_FUNCODE_GET_CONTROLLER_INFO,						//ͨ��UDP�㲥��ʽ��ȡ���п���������Ϣ
	BEGP_FUNCODE_SET_CONTROLLER_INFO,						//ͨ��UDP�㲥��ʽ���ÿ������Ĳ���
	/* ���������� */
	BEGP_FUNCODE_ADD_METER,									//������һ�������Ϣ
	BEGP_FUNCODE_DELETE_METER,								//ɾ��һ�����
	BEGP_FUNCODE_SET_METER_PARAMETER,						//����һ���������в���
	BEGP_FUNCODE_GET_METER_PARAMETER,						//��ȡһ����������������Ϣ
	/* ʵʱ������� */
	BEGP_FUNCODE_GET_METER_RTDATA,							//��ȡʵʱ����
	/* ��ʷ������� */
	BEGP_FUNCODE_GET_METER_HISDATA,							//��ȡ��ʷ����
	/* ������� */
	BEGP_FUNCODE_GET_ALARMINFO, 						    //��ȡһ��������Ϣ
	/*Զ������*/
	BEGP_FUNCODE_IAP										//Զ������
}BEGP_FUNCODE_TYPE_T;

typedef enum
{
	BEGP_PACKETDATA_ECHO_CODE_SUCCESS = 0,					//�ɹ�
	BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR,					//���ݳ��Ȳ���
	BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND,				//meterid ����
	BEGP_PACKETDATA_ECHO_CODE_METERALREADYEXIST,		    //����Ѿ�����
	BEGP_PACKETDATA_ECHO_CODE_VALUEERR,				  		//���ݲ��Դ��� ���粨���� ���˿ںŵ�
	BEGP_PACKETDATA_ECHO_CODE_CRCERROR,				  		//crc����
	BEGP_PACKETDATA_ECHO_CODE_MAXMETERCOUNT,				//������ﵽ���޲���������
	BEGP_PACKETDATA_ECHO_CODE_ERROR

}BEGP_PACKETDATA_ECHO_CODE_TYPE_T;

/* ��������֡ͷ */
#define BEGP_REQ_HEAD_SIZE	sizeof(struct _BEGP_REQ_HEAD_)
typedef struct _BEGP_REQ_HEAD_
{
	uint8_t    SlaveAddress;		//DDC��485ͨ��ʱ�ĵ�ַ
	uint8_t	   FunctionCode;		//������
	uint16_t   SequenceNumber;		//���к�
	uint32_t   DataSize;			//Ҫ���͵����ݳ���
}BEGP_REQ_HEAD_T;

REFLECTION(_BEGP_REQ_HEAD_, SlaveAddress, FunctionCode, SequenceNumber, DataSize)

/* ��Ӧ����֡ͷ */
#define BEGP_RSP_HEAD_SIZE   sizeof(struct _BEGP_RSP_HEAD_)
typedef struct _BEGP_RSP_HEAD_
{
	uint8_t    SlaveAddress;		//DDC��485ͨ��ʱ�ĵ�ַ
	uint8_t	   FunctionCode;		//������
	uint16_t   SequenceNumber;		//���к�
	uint32_t   DataSize;			//Ҫ���͵����ݳ���
}BEGP_RSP_HEAD_T;
REFLECTION(_BEGP_RSP_HEAD_, SlaveAddress, FunctionCode, SequenceNumber, DataSize)
/* �����������Ӧ�������� */
#define BEGP_RSP_PACKETDATA_ECHO_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_ECHO_)
typedef struct _BEGP_RSP_PACKETDATA_ECHO_
{
	uint8_t  Code;
}BEGP_RSP_PACKETDATA_ECHO_T;
REFLECTION(_BEGP_RSP_PACKETDATA_ECHO_, Code)
/********************************** �ɼ������ **********************************/
/* ����һ���ɼ��������� */
#define BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_)
typedef struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_
{
	uint32_t 	ID;						//��������ID������Ψһ
	char     	Name[32];				//������������
	char     	Alias[64];				//������������֧�ֺ���
	char     	Description[128];		//��������������Ϣ��֧�ֺ���
	char     	Location[64];			//�������İ�װλ��
	char     	Zone[64];				//������Ϣ
	uint8_t 	Active;					//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 		//��������			��λΪ����
	uint8_t  	HisdataSavePeriod;		//��ʷ���ݱ�������	��λΪСʱ
}BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T;

/* ���ÿ����������в��� */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_)
typedef struct  _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_
{
	uint32_t 	ID;						//��������ID������Ψһ
	char     	Name[32];				//������������
	char     	Alias[64];				//������������֧�ֺ���
	char     	Description[128];		//��������������Ϣ��֧�ֺ���
	char     	Location[64];			//�������İ�װλ��
	char     	Zone[64];				//������Ϣ
	uint8_t 	Active;					//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 		//��������		   ��λΪ����
	uint8_t  	HisdataSavePeriod;		//��ʷ���ݱ������� ��λΪСʱ
}BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T;
REFLECTION(_BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_, ID, Name, Alias, Description, Location, Zone, Active, SamplePeriod, HisdataSavePeriod);
/* ��ȡ���������������ò��� */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_)
typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_
{
	uint32_t 	ID;						//��������ID������Ψһ
	char     	Name[32];				//������������
	char     	Alias[64];				//������������֧�ֺ���
	char     	Description[128];		//��������������Ϣ��֧�ֺ���
	char     	Location[64];			//�������İ�װλ��
	char     	Zone[64];				//������Ϣ
	uint8_t 	Active;					//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 		//��������		   ��λΪ����
	uint8_t  	HisdataSavePeriod;		//��ʷ���ݱ�������  ��λΪСʱ
}BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T;

/* ���ÿ�������ʱ�� */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_)
typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_
{
	uint8_t year;			//��ݱ�ʾ2000���Ժ��������12����2012��
	uint8_t month;			//1-12
	uint8_t day;			//1-31
	uint8_t wday;			//���ڼ�,ȡֵ��ΧMONDAY-SUNDAY(1-7)
	uint8_t hour;			//0-23
	uint8_t minute;			//0-59
	uint8_t second;			//0-59
}BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T;

REFLECTION(_BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_, year,month,day,wday,hour,minute,second)

/* ��ȡ��������ʱ�� */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_)
typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_
{
	uint8_t year;		//��ݱ�ʾ2000���Ժ��������12����2012��
	uint8_t month;		//1-12
	uint8_t day;		//1-31
	uint8_t wday;		//���ڼ�,ȡֵ��ΧMONDAY-SUNDAY(1-7)
	uint8_t hour;		//0-23
	uint8_t minute;		//0-59
	uint8_t second;		//0-59
}BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T;



/* ��ȡ���������������ò��� */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_)
typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_
{
	char	Name[32];				//����
	char	CampanyName[32];		//��˾����
	char	Model[32];				//�ͺ�
	uint8_t	HWVer;					//Ӳ���汾��
	uint8_t	FWVer;					//����汾��
	uint8_t	IP[4];					//IP��ַ
	uint8_t Mask[4];				//Mask��ַ
	uint8_t	GateWay[4];				//����
	uint8_t Mac[6];					//Mac��ַ
	uint32_t 	ID;						//��������ID������Ψһ
	char     	Alias[64];				//������������֧�ֺ���
	char     	Description[128];		//��������������Ϣ��֧�ֺ���
	char     	Location[64];			//�������İ�װλ��
	char     	Zone[64];				//������Ϣ
	uint8_t 	Active;					//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 		//��������		   ��λΪ����
	uint8_t  	HisdataSavePeriod;		//��ʷ���ݱ������� ��λΪСʱ
}BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T;

/* ���ÿ��������������ò��� */
#define BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_)
typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_
{
	char	Name[32];			//����
	char	CampanyName[32];	//��˾����
	char	Model[32];			//�ͺ�
	uint8_t	HWVer;				//Ӳ���汾��
	uint8_t	FWVer;				//����汾��
	uint8_t	IP[4];				//IP��ַ
	uint8_t Mask[4];			//Mask��ַ
	uint8_t	GateWay[4];			//����
	uint8_t Mac[6];				//Mac��ַ
	uint32_t 	ID;						//��������ID������Ψһ
	char     	Alias[64];				//������������֧�ֺ���
	char     	Description[128];		//��������������Ϣ��֧�ֺ���
	char     	Location[64];			//�������İ�װλ��
	char     	Zone[64];				//������Ϣ
	uint8_t 	Active;					//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	uint8_t  	SamplePeriod;	 		//��������		   ��λΪ����
	uint8_t  	HisdataSavePeriod;		//��ʷ���ݱ������� ��λΪСʱ
}BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_T;











/********************************** ������ **********************************/
/*����һ�����*/
//����֡ 
#define BEGP_REQ_PACKETDATA_ADD_METER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_METER_)
typedef struct _BEGP_REQ_PACKETDATA_ADD_METER_
{
	uint32_t		MeterID;			    //������к�
	uint8_t			MeterType;				//������� 		���չ淶 114ҳ
	uint8_t			ComRate;				//ͨ������ 		���չ淶 114ҳ
	uint8_t			ComPort;				//ͨ�Ŷ˿ں�	���չ淶 114ҳ
	uint8_t			ComAgreementType;		//ͨ�Ź�Լ����	���չ淶 114ҳ
	uint8_t			PhyAddr[7];			    //�����ַ		���չ淶 136ҳ ���ո�ʽ03 BYTE1-BYTE6 ����BCD�������ʽ  ����λ������ ��ʮ��ǧд����
	char			Alias[64];				//��ı���
	char			Description[128];		//���������Ϣ
	char			Location[64];			//���İ�װλ��
	uint8_t			Active;					//����ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
	struct ElectricDataCollectMethod CollectMethod;//�ɼ�����
}BEGP_REQ_PACKETDATA_ADD_METER_T;
//��Ӧ֡

/* ����ģ����������ò��� */
//����֡
#define BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_)
typedef struct _BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_
{
	uint32_t    MeterID;								//������к�
	uint8_t		MeterType;								//������� 		���չ淶 114ҳ
	uint8_t		ComRate;								//ͨ������ 		���չ淶 114ҳ
	uint8_t		ComPort;								//ͨ�Ŷ˿ں�	���չ淶 114ҳ
	uint8_t		ComAgreementType;						//ͨ�Ź�Լ����	���չ淶 114ҳ
	uint8_t		PhyAddr[7];							    //�����ַ		���չ淶 136ҳ ���ո�ʽ03 BYTE1-BYTE6 ����BCD�������ʽ  ����λ������ ��ʮ��ǧд����
	char		Alias[64];								//��ı���
	char		Description[128];						//���������Ϣ
	char		Location[64];							//���İ�װλ��
	uint8_t		Active;									//����ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
	struct ElectricDataCollectMethod	 CollectMethod;//�ɼ�����
}BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_T;
//��Ӧ֡
//echo 1��id������ 2������ͨ��

/* ɾ��һ����� */
//����֡
#define BEGP_REQ_PACKETDATA_DELETE_METER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_METER_)
typedef struct _BEGP_REQ_PACKETDATA_DELETE_METER_
{
	uint32_t MeterID;//ģ���ʶ����
}BEGP_REQ_PACKETDATA_DELETE_METER_T;
//��Ӧ֡
//echo 1��id������2��ͨ�ô���


/* **********************************��ȡ��������������Ϣ ********************************************/
/* ����֡���� */
#define BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_)
typedef struct _BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_
{
	uint32_t MeterID;//����ID
}BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_T;
/* ��Ӧ֡���� */
#define BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_)
typedef struct _BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_
{

	uint32_t    MeterID;								//������к�
	uint8_t		MeterType;								//������� 		���չ淶 114ҳ
	uint8_t		ComRate;								//ͨ������ 		���չ淶 114ҳ
	uint8_t		ComPort;								//ͨ�Ŷ˿ں�	���չ淶 114ҳ
	uint8_t		ComAgreementType;						//ͨ�Ź�Լ����	���չ淶 114ҳ
	uint8_t		PhyAddr[7];							//�����ַ		���չ淶 136ҳ ���ո�ʽ03 BYTE1-BYTE6 ����BCD�������ʽ  ����λ������ ��ʮ��ǧд����
	char		Alias[64];								//��ı���
	char		Description[128];						//���������Ϣ
	char		Location[64];							//���İ�װλ��
	uint8_t		Active;									//����ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
	struct ElectricDataCollectMethod	 CollectMethod;//�ɼ�����
}BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_T;

/**********************************����ʵʱ����****************************/

#define	BEGP_REQ_PACKETDATA_GET_METER_RTDATA_SIZE sizeof(struct _BEGP_REQ_PACKETDATA_GET_METER_RTDATA_)
//����֡
typedef struct _BEGP_REQ_PACKETDATA_GET_METER_RTDATA_
{
	uint32_t MeterID;//����ID
}BEGP_REQ_PACKETDATA_GET_METER_RTDATA_T;

// ��Ӧ֡����  
#define BEGP_RSP_PACKETDATA_GET_METER_RTDATA_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_METER_RTDATA_)
typedef struct _BEGP_RSP_PACKETDATA_GET_METER_RTDATA_
{
	uint32_t 	MeterID;			    //������к�
	uint32_t 	ElectricData[MAX_ELECTRIC_COUNT];		//���һ�ٸ����ܲ���
	uint8_t     CollectCount;		    //ʵ�ʲɼ��ĵ��ܲ�������
	uint8_t  	DateYear;																			//�����꣬��14
	uint8_t  	DateMonth; 																			//�����£�1-12
	uint8_t  	DateDay; 																			//�����գ�1-31
	uint8_t  	TimeHour;																			//ʱ��ʱ��0-23
	uint8_t  	TimeMinute; 																		//ʱ���, 0-59
	uint8_t 	TimeSecond; 																		//ʱ����, 0-59
}BEGP_RSP_PACKETDATA_GET_METER_RTDATA_T;


/**********************************������ʷ����****************************/

#define	BEGP_REQ_PACKETDATA_GET_METER_HISDATA_SIZE sizeof(struct _BEGP_REQ_PACKETDATA_GET_METER_HISDATA_)
//����֡
typedef struct _BEGP_REQ_PACKETDATA_GET_METER_HISDATA_
{
	uint32_t MeterID;	//����ID
	uint8_t  SeletDay;	//ѡ����һ��
}BEGP_REQ_PACKETDATA_GET_METER_HISDATA_T;

// ��Ӧ֡����  
#define BEGP_RSP_PACKETDATA_GET_METER_HISDATA_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_METER_HISDATA_)
typedef struct _BEGP_RSP_PACKETDATA_GET_METER_HISDATA_
{
	uint32_t 	  MeterID;			    										     //������к�
	struct MeterElectricHisData MeterElectricData[MAX_STORE_HOUR_COUNT];			 //�����һ��Сʱһ������
	uint8_t	     HisDataHourCount;													 //Сʱ����
}BEGP_RSP_PACKETDATA_GET_METER_HISDATA_T;


/*************************************����ʵʱ������Ϣ**********************************/

// ��Ӧ֡����  
#define BEGP_RSP_PACKETDATA_GET_ALARMINFO_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_ALARMINFO_)
typedef struct _BEGP_RSP_PACKETDATA_GET_ALARMINFO_
{
	uint32_t 	AlarmInforID;				    //������Ϣ���к�
	uint8_t  	AlarmInforDateYear;				//��ʼ�����꣬��14
	uint8_t  	AlarmInforDateMonth; 			//��ʼ�����£�1-12
	uint8_t  	AlarmInforDateDay; 				//��ʼ�����գ�1-31
	uint8_t     AlarmInforTimeHour;				//��ʼʱ��ʱ��0-23
	uint8_t     AlarmInforTimeMinute; 			//��ʼʱ���, 0-59
	uint8_t     AlarmInforTimeSecond; 			//��ʼʱ����, 0-59
	uint8_t 	AlarmInforCategory;				//�¼�����
	uint16_t 	AlarmInforSeqNum;				//�Ǳ����к� ����  Ϊ�ɼ�����ID
	uint8_t 	AlarmInforType;					//�¼�����
}BEGP_RSP_PACKETDATA_GET_ALARMINFO_T;

#define BEGP_CRC_SIZE 2

#pragma pack() // ȡ�� ���� �ɶԳ��ֵ�

}
#endif