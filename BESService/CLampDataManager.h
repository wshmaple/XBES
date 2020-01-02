#pragma once



#include "CCrossDataManager.h"

/**
 * Manager for lamp data. ��Ҫ����ƹ����ز�����
 *
 * @author WSH
 * @date 2018/9/14
 */
#define   LAMP_CONTROL_MODE_SIZE    512
#define   LAMP_COLLECT_MODE_SIZE    512
#define   LAMP_CONTROL_POINT_SIZE    512
#define   LAMP_COLLECT_POINT_SIZE    100
#pragma pack(1)
typedef struct _LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_
{
	char     Name[32];//ģ�������
	char     Alias[64];//ģ��ı���
	char     Description[128];//ģ���������Ϣ
	char     Location[64];//ģ��İ�װλ��

	uint8_t  ModelID;//ģ���ͺű���
	uint8_t  AreaNum;     //�����1-255
	uint8_t	 BranchNum;   //֧·��1-255
	uint8_t  SlaveAddress;//ģ���ͨ�ŵ�ַ
	uint8_t  Active;//ģ���ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
	uint16_t ID;//ģ���ʶ����
}LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T;


/********************************** �ͳ������ **********************************/
/*���Ƴ���*/
typedef struct  _LAMP_BEGP_CONTROL_POINT_
{
	uint8_t    Active;   //ʹ��״̬
	uint16_t   PointID;	 //�ƻ�����Ӧ���ID��
	int32_t    RunValue; //����ֵ,������0��255��ģ�����Ǹ���ֵ
	uint8_t    DelayTime;//��ʱִ��ʱ��
}LAMP_BEGP_CONTROL_POINT_T;
typedef struct  _LAMP_BEGP_CONTROL_MODE_
{
	uint8_t                Active;  //ʹ��״̬
	char                   Name[32];  //����ģʽ����
	uint8_t                ID;   //����ģʽID
	LAMP_BEGP_CONTROL_POINT_T   ControlPoint[LAMP_CONTROL_POINT_SIZE];//ģʽ�����Ϣ
}LAMP_BEGP_CONTROL_MODE_T;
typedef struct  _LAMP_BEGP_CONTROL_SCENE_PARAMETER_
{
	uint8_t               SceneType; //��������  ���ƻ�ɼ�������ȡ�ƻ������ֵ
	uint16_t              ID;       //���Ƴ�����ID
	uint8_t               Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
	char                  Name[32];  //��������
	char                  Alias[64]; //��������
	LAMP_BEGP_CONTROL_MODE_T   ControlMode[LAMP_CONTROL_MODE_SIZE]; //����ģʽ
}LAMP_BEGP_CONTROL_SCENE_PARAMETER_T;

/* �ɼ����� */
typedef struct  _LAMP_BEGP_COLLECT_POINT_
{
	uint8_t     Active;   //ʹ��״̬
	uint16_t    PointID;	 //�ƻ�����Ӧ���ID��
	uint8_t     CollectID;//�ɼ���ʽ��ʱ�����ɼ���ֵ�ɼ�
	uint8_t     TimeUnit;//ʱ������λ
	int16_t     Value; //�ɼ�ʱ����ֵ���仯����ֵ
}LAMP_BEGP_COLLECT_POINT_T;
typedef struct  _LAMP_BEGP_COLLECT_MODE_
{
	uint8_t                  Active;  //ʹ��״̬
	char                     Name[32];  //����ģʽ����
	uint8_t                  ID;   //����ģʽID
	LAMP_BEGP_COLLECT_POINT_T     CollectPoint[LAMP_COLLECT_POINT_SIZE];//ģʽ�����Ϣ
}LAMP_BEGP_COLLECT_MODE_T;
typedef struct  _LAMP_BEGP_COLLECT_SCENE_PARAMETER_
{
	uint8_t              SceneType; //��������  ���ƻ�ɼ�������ȡ�ƻ������ֵ
	uint16_t             ID;        //���Ƴ�����ID
	uint8_t              Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
	char                 Name[32];  //��������
	char                 Alias[64]; //��������
	LAMP_BEGP_COLLECT_MODE_T  CollectMode[LAMP_COLLECT_MODE_SIZE]; //����ģʽ
}LAMP_BEGP_COLLECT_SCENE_PARAMETER_T;
#pragma pack()
class CLampDataManager :
	public CCrossDataManager
{
public:
	CLampDataManager();
	~CLampDataManager();

	// �ܵĿ���

	// ģ��
	virtual  int   AddOrSetControllerModule(Json::Value jsonData, char *sendbuf);
	virtual  int   AddOrSetControllerModule2(Json::Value jsonData, char *sendbuf);
	virtual  Json::Value ParseGetModePar(void *pRecvData);//��ȡģ����Ϣ
	// ����
	virtual  int   AddScene(Json::Value jsonData, char * sendbuf); // ����һ������
	virtual  int   SetScene(Json::Value jsonData, char * sendbuf); // ����һ������
	virtual  Json::Value ParseGetSence(void *pRecvData);	// ��ȡ��������
};

