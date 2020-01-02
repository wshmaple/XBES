#ifndef _GLOABLE_CROSS_
#define _GLOABLE_CROSS_
namespace Gloable_Cross{
	// ԭDllTest  BES200.h
#define BEGP_CRC_SIZE 2
/*��������Ҫ�õ�������λ������*/
	typedef  unsigned       char uint8_t;
	typedef  unsigned short int  uint16_t;
	typedef  unsigned       int  uint32_t;
	typedef    signed       char int8_t;
	typedef    signed short int  int16_t;
	typedef    signed       int  int32_t;

#pragma pack(1)

	/* �Ƿ�����Activeȡֵ */
	enum Active
	{
		ACTIVE_DISABLE = 0,  //��ֹ
		ACTIVE_ENABLE        //ʹ��
	};

	/*BES200���������붨��*/
	enum Bes200ErrorCode {
		BES200_ERROR_OK = 0,         //�޴�����������
		BES200_ERROR_CONNECT = 1,   //���������������ʧ��
		BES200_ERROR_SEND = 2,     //���ݷ���ʧ��
		BES200_ERROR_RECIVE = 3,  //���ݽ���ʧ��
		BES200_ERROR_REFUSED = 4,//���󱻷������ܾ�
		BES200_ERROR_OTHERS = 5 //��������
	};
	/*DDC������ȡֵ��ģ��Ĵ�����ȡֵ*/
	enum ErrorCode
	{
		ERROR_CODE_OK = 0x00,//û�д���
		ERROR_CODE_OFF_LINE = 0x01,//����
		ERROR_CODE_INIT_FAILED = 0x02,//��ʼ��ʧ��
	};

	/****************************�����ȡֵ����*******************************/
	/* �����Ͷ��� */
	enum PointType
	{
		POINT_TYPE_LAI = 0,   //ʵ��AI
		POINT_TYPE_LAO = 1,   //ʵ��AO
		POINT_TYPE_LDI = 2,   //ʵ��DO
		POINT_TYPE_LDO = 3,   //ʵ��DI
		POINT_TYPE_VIRTUAL_AI = 4,//���AI����
		POINT_TYPE_VIRTUAL_AO = 5,//���AO����
		POINT_TYPE_VIRTUAL_DI = 6,//���DI����
		POINT_TYPE_VIRTUAL_DO = 7,//���DO����
	};
	/* �㹤��ģʽ���� */
	enum WorkMode
	{
		WORK_MODE_AUTO = 0,//�߼��㹤��ģʽ�Զ�
		WORK_MODE_MANUAL = 1, //�߼��㹤��ģʽ�ֶ�
	};
	/* �㼫�Զ��� */
	enum Polarity
	{
		POLARITY_DIRECT = 0,//����
		POLARITY_REVERSE = 1,//����
	};
	/* �㱨���Ƿ����ö��� */
	enum AlarmActive
	{
		ALARM_ACTIVE_DISABLE = 0,//������ֹ
		AlARM_ACTIVE_ENABLE = 1,//��������
	};
	/* �㱨�����Ͷ��� */
	enum AlarmType
	{
		ALARM_TYPE_OK = 0, //������
		ALARM_TYPE_STANDARD = 1, //��׼����
		ALARM_TYPE_ENHANCED = 2, //��ǿ����
	};
	/* �������ȼ����� */
	enum AlarmPriority
	{
		AlARM_PRIORITY_GENERAL = 0,//һ��
		AlARM_PRIORITY_LARGER = 1,//�ϴ�
		AlARM_PRIORITY_MAJOR = 2,//�ش�
	};
	/* ������������������ */
	enum AlarmTrigger
	{
		ALARM_TRIGGER_ON = 0,//�պϱ���
		ALARM_TRIGGER_OFF = 1,//�Ͽ�����
	};
	/* �㱨��״̬���� */
	enum AlarmState
	{
		ALARM_STATE_OK = 0,//û�б���
		ALARM_STATE_HIGH_LIMIT = 1,//���ޱ���
		ALARM_STATE_LOW_LIMIT = 2,//���ޱ���
		ALARM_STATE_ON = 3,//�պϱ���
		ALARM_STATE_OFF = 4,//�Ͽ�����
	};
	/*ģ����ͨ�����Ͷ���*/
	enum LineType
	{
		LV0010 = 0,  //���Ե�ѹ0..10V 
		LC0420 = 1,  //���Ե���4..20mA 
		LC0020 = 2,  //���Ե���0..20mA
	};

	/* ��������ֵ���� */
#define OFF     0
#define ON      255

/****************************�������ȡֵ����**************************/
/* �ɼ���ʽ���� */
	enum CollectID
	{
		COLLECT_ID_TIME_INTERVAL = 1,//ʱ����
		COLLECT_ID_COV = 2,//�仯��
	};
	/* �ɼ�ʱ������λ���� */
	enum TimeUnit
	{
		TIME_UNIT_DAY = 1,//ʱ������λ��
		TIME_UNIT_HOUR = 2,//ʱ������λʱ
		TIME_UNIT_MINUTE = 3,//ʱ������λ��
		TIME_UNIT_SECOND = 4,//ʱ������λ��
	};

	/**************************�ƻ����ȡֵ����**************************/
	/* �ƻ����Ͷ��� */
	enum PlanType
	{
		PLAN_TYPE_REGULAR = 0, //�����
		PLAN_TYPE_HOLIDAY = 1, //���ڵ�
	};
	/* ִ�з�ʽ���� */
	enum ExecutionWay
	{
		EXECUTION_WAY_DAILY = 0, //ÿ��ִ��
		EXECUTION_WAY_SPAN = 1, //����ִ��
	};
	/* �������Ͷ��� */
	enum SceneType
	{
		SCENE_TYPE_CONTROL = 0, //���Ƴ���
		SCENE_TYPE_COLLECT = 1, //�ɼ�����
	};
	/*�����붨��*/
	//��һλ��ʾ����һ��0��ʾ��ִ�У�1��ʾִ�У���0x01��
	//�ڶ�λ��ʾ���ڶ���0��ʾ��ִ�У�1��ʾִ�У���0x02��
	//����λ��ʾ��������0��ʾ��ִ�У�1��ʾִ�У���0x04��
	//����λ��ʾ�����ģ�0��ʾ��ִ�У�1��ʾִ�У���0x08��
	//����λ��ʾ�����壺0��ʾ��ִ�У�1��ʾִ�У���0x10��
	//����λ��ʾ��������0��ʾ��ִ�У�1��ʾִ�У���0x20��
	//����λ��ʾ�����ߣ�0��ʾ��ִ�У�1��ʾִ�У���0x40��
	//8λȫΪ0�����ʾÿ��ִ�С�

	/*BES200�����п����õ��Ľṹ�嶨��*/
	/********************************* ��������� ***********************************/
	struct ControllerParameter
	{
		char     Name[32];//������������
		char     Alias[64];//������������֧�ֺ���
		char     Description[128];//��������������Ϣ��֧�ֺ���
		char     Location[64];//�������İ�װλ��
		char     Zone[64];//������Ϣ
		uint16_t ID;//��������ID������Ψһ
		uint8_t  Active;//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	};


	struct ControllerInfo {
		char	Name[32];//����
		char	CampanyName[32];//��˾����
		char	Model[12];//�ͺ�
		uint8_t	HWVer;//Ӳ���汾��
		uint8_t	FWVer;//����汾��
		uint8_t	IP[4];//IP��ַ
		uint8_t Mask[4];//Mask��ַ
		uint8_t	GateWay[4];//����
		uint8_t Mac[6];//Mac��ַ
	};

	//struct tm {
	//	int tm_sec;     /* seconds after the minute - [0,59] */
	//	int tm_min;     /* minutes after the hour - [0,59] */
	//	int tm_hour;    /* hours since midnight - [0,23] */
	//	int tm_mday;    /* day of the month - [1,31] */
	//	int tm_mon;     /* months since January - [0,11] */
	//	int tm_year;    /* years since 1900 */
	//	int tm_wday;    /* days since Sunday - [0,6] */
	//	int tm_yday;    /* days since January 1 - [0,365] */
	//	int tm_isdst;   /* daylight savings time flag */
	//};
	/************************************ ģ����� *********************************/
	struct ModuleParameter
	{
		char Name[32];//ģ�������
		char Alias[64];//ģ��ı���
		char Description[128];//ģ���������Ϣ
		char Location[64];//ģ��İ�װλ��

		//ģ��������������ϵĺ궨��
#define FLN1 1
#define FLN2 2
#define FLN3 3
#define FLN4 4
		uint8_t  ModelID;//ģ���ͺű���
		uint8_t  FlnID;//ģ���������FLN�����ϣ�����ȡֵFLN1��FLN2��FLN3��FLN4
		uint8_t  SlaveAddress;//ģ���ͨ�ŵ�ַ
		uint8_t  Active;//ģ���ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
		uint16_t ID;//ģ���ʶ����
	};

	/********************************** �߼������ **********************************/
	/* ģ��������ʵ��AI */
	struct LogicalPointParameterLAI
	{
		uint8_t  PointType; //������  ʵ��AI
		uint16_t ID; //���ID
		uint8_t  Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32]; //�������
		char     Alias[64]; //��ı���
		char     Description[64]; //����
		uint16_t ModuleID;  //������ģ���ID��
		uint8_t  ChannelIndex;  //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//ֵ

		/*�������*/
		uint8_t  AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t  AlarmType;  //�������ͣ�����������׼��������ǿ����
		uint8_t  AlarmPriority;  //�������ȼ�

		uint8_t  LineType;//��Ч�������ͣ�����0-10V��0-20mA��4-20mA
		char     Unit[12]; //���̵�λ����H���桢KWh
		int32_t  HighRange; //��߷�ֵ
		int32_t  LowRange;  //��ͷ�ֵ
		uint8_t  Precision;  //����
		int32_t  AlarmHighValue; //���ޱ���ֵ
		int32_t  AlarmLowValue;//���ޱ���ֵ
	};
	/* ģ�������ʵ��AO */
	struct LogicalPointParameterLAO
	{
		uint8_t  PointType;  //������  ʵ��AO
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		uint16_t ModuleID; //������ģ���ID��
		uint8_t  ChannelIndex; //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t LineType;  //��Ч������ͣ�����0-10V��0-20mA��4-20mA
		char    Unit[12];  //���̵�λ����H���桢KWh
		int32_t HighRange;  //��߷�ֵ
		int32_t LowRange;   //��ͷ�ֵ
		uint8_t Precision;   //����
		int32_t AlarmHighValue;  //���ޱ���ֵ
		int32_t AlarmLowValue;  //���ޱ���ֵ
	};
	/* ����������ʵ��DI */
	struct LogicalPointParameterLDI
	{
		uint8_t  PointType;  //������    ʵ��DI
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		uint16_t ModuleID; //������ģ���ID��
		uint8_t  ChannelIndex; //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//ֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������
		uint8_t ActivePassive;//��Դ��Դ
	};
	/* ���������ʵ��DO */
	struct LogicalPointParameterLDO
	{
		uint8_t  PointType;  //������   ʵ��DO
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		uint16_t ModuleID; //������ģ���ID��
		uint8_t  ChannelIndex; //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������  �����
	};
	/* ���AI */
	struct LogicalPointParameterVirtualAI
	{
		uint8_t   PointType;  //������  ���AI
		uint16_t  ID; //���ID
		uint8_t   Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //�������
		char      Alias[64]; //��ı���
		char      Description[64];//����
		int32_t   InitValue;//��ʼֵ
		char      Unit[12];  //���̵�λ����H���桢KWh
		uint8_t   Precision;  //����

		/*�������*/
		uint8_t  AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t  AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t  AlarmPriority;   //�������ȼ�
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	};
	/* ���AO */
	struct LogicalPointParameterVirtualAO
	{
		uint8_t   PointType;  //������  ���AO
		uint16_t  ID; //���ID
		uint8_t   Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //�������
		char      Alias[64]; //��ı���
		char      Description[64];//����
		int32_t   InitValue;//��ʼֵ
		char      Unit[12];  //���̵�λ����H���桢KWh
		uint8_t   Precision;  //����

		/*�������*/
		uint8_t  AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t  AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t  AlarmPriority;   //�������ȼ�
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	};
	/* ���DI */
	struct LogicalPointParameterVirtualDI
	{
		uint8_t   PointType;  //������  ���DI
		uint16_t  ID; //���ID
		uint8_t   Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //�������
		char      Alias[64]; //��ı���
		char      Description[64];//����
		int32_t   InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������  �����
	};
	/* ���DO */
	struct LogicalPointParameterVirtualDO
	{
		uint8_t   PointType;  //������  ���DO
		uint16_t  ID; //���ID
		uint8_t   Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //�������
		char      Alias[64]; //��ı���
		char      Description[64];//����
		int32_t   InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������  �����
	};
	/* ���ݵ����Ͳ�ͬ��Tab�д�ŵĵ���Ϣ��ͬ */
	struct LogicalPointParameter
	{
		uint8_t   PointType;//������
		char     Tab[sizeof(struct LogicalPointParameterLAO)];//���鳤����ģ������� void      *tab;
	};
	/*��ȡ��ı�����Ϣ */
	struct LogicalPointAlarmInfor
	{
		int32_t  CurrentValue;//��ȡ�ĵ�ǰֵ
		uint8_t  AlarmState;  //����״̬
		int32_t  AlarmTriggerValue; //�״α�������ֵ
		int32_t  AlarmMaxValue;     //�������ֵ(�������������һ�α���ֵ)
	};
	/*���õ��ֵ*/
	struct LogicalPointvalue
	{
		uint16_t  ID; //���ID
		uint8_t   WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		int32_t   Value;//��ʼֵ
	};
	/********************************** ������� *********************************/
	/* ���Ƴ��� */
	struct Control_Point
	{
		uint8_t  Active;   //ʹ��״̬
		uint16_t PointID;	 //�ƻ�����Ӧ���ID��
		int32_t  RunValue; //����ֵ,������0��255��ģ�����Ǹ���ֵ
	};
	struct Control_Mode
	{
		uint8_t Active;  //ʹ��״̬
		char    Name[32];  //����ģʽ����
		uint8_t ID;   //����ģʽID
		Control_Point ControlPoint[100];//ģʽ�����Ϣ
	};
	struct ControlSceneParameter
	{
		uint8_t   SceneType; //��������  ���ƻ�ɼ�������ȡ�ƻ������ֵ
		uint16_t  ID;       //���Ƴ�����ID
		uint8_t   Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //��������
		char      Alias[64]; //��������
		Control_Mode    ControlMode[10]; //����ģʽ
	};
	/* �ɼ����� */
	struct Collect_Point
	{
		uint8_t  Active;   //ʹ��״̬
		uint16_t PointID;	 //�ƻ�����Ӧ���ID��
		uint8_t  CollectID;//�ɼ���ʽ��ʱ�����ɼ���ֵ�ɼ�
		uint8_t  TimeUnit;//ʱ������λ
		int16_t  Value; //�ɼ�ʱ����ֵ���仯����ֵ
	};
	struct Collect_Mode
	{
		uint8_t  Active;  //ʹ��״̬
		char     Name[32];  //����ģʽ����
		uint8_t  ID;   //����ģʽID
		Collect_Point CollectPoint[100];//ģʽ�����Ϣ
	};
	struct CollectSceneParameter
	{
		uint8_t  SceneType; //��������  ���ƻ�ɼ�������ȡ�ƻ������ֵ
		uint16_t  ID;        //���Ƴ�����ID
		uint8_t   Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //��������
		char      Alias[64]; //��������
		Collect_Mode    CollectMode[10]; //����ģʽ
	};
	/* �������Ͳ�ͬ��Tab�д�ŵĳ�����Ϣ��ͬ */
	struct SceneParameter
	{
		uint8_t SceneType; //��������  ���ƻ�ɼ�����
		char    Tab[sizeof(struct CollectSceneParameter) > sizeof(struct ControlSceneParameter) ?
			sizeof(struct CollectSceneParameter) : sizeof(struct ControlSceneParameter)];//ȡ���Ƴ����Ͳɼ������ϴ��ֵ
	};

	/***************************************** �ƻ���� *******************************/
	struct SchedulerParameter
	{
		uint8_t  Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�ƻ�������
		char     Alias[64];  //����
		uint8_t  PlanType;  //�ƻ�����  ����ƻ���ڼ��ռƻ�

		uint8_t  StartDateYear;//��ʼ�����꣬��14
		uint8_t  StartDateMonth; //��ʼ�����£�1-12
		uint8_t  StartDateDay; //��ʼ�����գ�1-31
		uint8_t  EndDateYear; //����������
		uint8_t  EndDateMonth; //����������
		uint8_t  EndDateDay; //����������

		uint8_t  StartTimeHour;// ��ʼʱ��ʱ��0-23
		uint8_t  StartTimeMinute; //��ʼʱ���,0-59
		uint8_t  StartTimeSecond; //��ʼʱ����,0-59
		uint8_t  EndTimeHour;//����ʱ��ʱ
		uint8_t  EndTimeMinute;//����ʱ���
		uint8_t  EndTimeSecond;//����ʱ����

		uint8_t  ExecutionWay; //ִ�з�ʽ��ÿ��ִ�С�����ִ�У�
		uint8_t  WeekMask;	 //������
		uint8_t  SceneType; //�������ͣ����Ƴ�����ɼ�����
		uint16_t SceneID;  //����ID��
		uint8_t  ModeID;  //ģʽID��
		uint8_t  DefaultModeID;//Ĭ��ģʽID
	};



///< beg.h


#define  SENDBUF_SIZE   4096*3
#define  RECVBUF_SIZE   4096*3

#define   SCENE_SENDBUF_SIZE   10240
#define   SCENE_RECVBUF_SIZE   10240

#define   CONTROL_MODE_SIZE    10
#define   COLLECT_MODE_SIZE    10
#define   CONTROL_POINT_SIZE    100
#define   COLLECT_POINT_SIZE    100

/* ����ͨ�Ź����� */
	typedef enum
	{
		/* ��DDC��� */
		BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER = 0,//����һ��������
		BEGP_FUNCODE_SET_CONTROLLER_PARAMETER,    //���ÿ����������в���
		BEGP_FUNCODE_SET_CONTROLLER_TIME,         //���ÿ�������ʱ��
		BEGP_FUNCODE_REBOOT_CONTROLLER,           //�������������൱�ڸ�λ
		BEGP_FUNCODE_RESET_CONTROLLER,            //���ÿ��������ָ��������ã�������
		BEGP_FUNCODE_DELETE_CONTROLLER = 5,           //ɾ��һ������������ɾ��������ص�ģ��͵�
		BEGP_FUNCODE_GET_CONTROLLER_TIME,        //��ȡ��������ʱ��
		BEGP_FUNCODE_GET_CONTROLLER_PARAMETER,    //��ȡ���������������ò���
		BEGP_FUNCODE_GET_CONTROLLER_INFO,         //ͨ��UDP�㲥��ʽ��ȡ���п���������Ϣ
		BEGP_FUNCODE_SET_CONTROLLER_INFO,         //ͨ��UDP�㲥��ʽ���ÿ������Ĳ���
		/* ģ����� */
		BEGP_FUNCODE_ADD_MODULE = 10,                 //������һ��ģ��
		BEGP_FUNCODE_SET_MODULE_PARAMETER,       //����һ��ģ������в���
		BEGP_FUNCODE_DELETE_MODULE,              //ɾ��һ��ģ�飬��ɾ����ģ����صĵ�
		BEGP_FUNCODE_GET_MODULE_PARAMETER,       //��ȡһ��ģ�������������Ϣ
		BEGP_FUNCODE_GET_MODULE_ERROR_CODE,      //��ȡһ��ģ��Ĵ������
		/* ����� */
		BEGP_FUNCODE_ADD_LOGICAL_POINT = 15,             //������һ���߼���
		BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER,   //����һ���߼�������в���
		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE,        //����һ���߼����ֵ
		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME,//�����߼����ֵ�����ݵ������
		BEGP_FUNCODE_DELETE_LOGICAL_POINT,          //ɾ��һ���߼���
		BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER = 20,   //��ȡһ���߼�����������ò���
		BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR, //��ȡһ���߼���ı�����Ϣ
		/* ������� */
		BEGP_FUNCODE_ADD_SCENE,               //����һ������
		BEGP_FUNCODE_SET_SCENE_PARAMETER,     //����һ�����������в���
		BEGP_FUNCODE_DELETE_SCENE,            //ɾ��һ������
		BEGP_FUNCODE_GET_SCENE_PARAMETER = 25,     //��ȡһ��������������Ϣ
		/* �ƻ���� */
		BEGP_FUNCODE_ADD_SCHEDULER,           //����һ���ƻ�
		BEGP_FUNCODE_SET_SCHEDULER_PARAMETER, //�޸�һ���ƻ������в���
		BEGP_FUNCODE_DELETE_SCHEDULER,        //ɾ��һ���ƻ�
		BEGP_FUNCODE_GET_SCHEDULER_PARAMETER, //��ȡһ���ƻ������в���

		/*Զ������*/
		BEGP_FUNCUDE_IAP, //Զ������
	}BEGP_FUNCODE_TYPE_T;

	typedef enum
	{
		BEGP_PACKETDATA_ECHO_CODE_SUCCESS = 0,
		BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR,
		BEGP_PACKETDATA_ECHO_CODE_INDEXERR,
		BEGP_PACKETDATA_ECHO_CODE_VALUEERR,
		BEGP_PACKETDATA_ECHO_CODE_ERR
	}BEGP_PACKETDATA_ECHO_CODE_TYPE_T;

	/* ��������֡ͷ */
#define BEGP_REQ_HEAD_SIZE	sizeof(struct _BEGP_REQ_HEAD_)
	typedef struct _BEGP_REQ_HEAD_
	{
		uint8_t    SlaveAddress; //DDC��485ͨ��ʱ�ĵ�ַ
		uint8_t	   FunctionCode;  //������
		uint16_t   SequenceNumber; //���к�
		uint16_t   DataSize;  //Ҫ���͵����ݳ���
	}BEGP_REQ_HEAD_T;

	/* ��Ӧ����֡ͷ */
#define BEGP_RSP_HEAD_SIZE   sizeof(struct _BEGP_RSP_HEAD_)
	typedef struct _BEGP_RSP_HEAD_
	{
		uint8_t    SlaveAddress; //DDC��485ͨ��ʱ�ĵ�ַ
		uint8_t	   FunctionCode;  //������
		uint16_t   SequenceNumber; //���к�
		uint16_t   DataSize;  //Ҫ���͵����ݳ���
	}BEGP_RSP_HEAD_T;

	/* �����������Ӧ�������� */
#define BEGP_RSP_PACKETDATA_ECHO_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_ECHO_)
	typedef struct _BEGP_RSP_PACKETDATA_ECHO_
	{
		uint8_t  Code;
	}BEGP_RSP_PACKETDATA_ECHO_T;

	/********************************** DDC��� **********************************/
	/* ����һ�������� */
#define BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_
	{
		char      Name[32];//������������
		char      Alias[64];//������������֧�ֺ���
		char      Description[128];//��������������Ϣ��֧�ֺ���
		char      Location[64];//�������İ�װλ��
		char      Zone[64];//������Ϣ
		uint16_t  ID;//��������ID������Ψһ
		uint8_t   Active;//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	}BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T;

	/* ���ÿ����������в��� */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_)
	typedef struct  _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_
	{
		char     Name[32];//������������
		char     Alias[64];//������������֧�ֺ���
		char     Description[128];//��������������Ϣ��֧�ֺ���
		char     Location[64];//�������İ�װλ��
		char     Zone[64];//������Ϣ
		uint16_t ID;//��������ID������Ψһ
		uint8_t  Active;//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	}BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T;

	/* ���ÿ�������ʱ�� */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_
	{
		uint8_t year;//��ݱ�ʾ2000���Ժ��������12����2012��
		uint8_t month;//1-12
		uint8_t day;  //1-31
		uint8_t wday;  //���ڼ�,ȡֵ��ΧMONDAY-SUNDAY(1-7)
		uint8_t hour; //0-23
		uint8_t minute; //0-59
		uint8_t second; //0-59
	}BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T;

	/* ��ȡ��������ʱ�� */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_
	{
		uint8_t year;  //��ݱ�ʾ2000���Ժ��������12����2012��
		uint8_t month; //1-12
		uint8_t day;  //1-31
		uint8_t wday;  //���ڼ�,ȡֵ��ΧMONDAY-SUNDAY(1-7)
		uint8_t hour; //0-23
		uint8_t minute; //0-59
		uint8_t second; //0-59
	}BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T;

	/* ��ȡ���������������ò��� */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_
	{
		char     Name[32];//������������
		char     Alias[64];//������������֧�ֺ���
		char     Description[128];//��������������Ϣ��֧�ֺ���
		char     Location[64];//�������İ�װλ��
		char     Zone[64];//������Ϣ
		uint16_t ID;//��������ID������Ψһ
		uint8_t   Active;//��������ʹ��״̬��ֻ��ʹ��ʱ�Ż������������Ż�����λ��ͨ��
	}BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T;

	/* ��ȡ���������������ò��� */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_
	{
		char	Name[32];//����
		char	CampanyName[32];//��˾����
		char	Model[12];//�ͺ�
		uint8_t	HWVer;//Ӳ���汾��
		uint8_t	FWVer;//����汾��
		uint8_t	IP[4];//IP��ַ
		uint8_t Mask[4];//Mask��ַ
		uint8_t	GateWay[4];//����
		uint8_t Mac[6];//Mac��ַ
	}BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T;

	/* ���ÿ��������������ò��� */
#define BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_
	{
		char	Name[32];//����
		char	CampanyName[32];//��˾����
		char	Model[12];//�ͺ�
		uint8_t	HWVer;//Ӳ���汾��
		uint8_t	FWVer;//����汾��
		uint8_t	IP[4];//IP��ַ
		uint8_t Mask[4];//Mask��ַ
		uint8_t	GateWay[4];//����
		uint8_t Mac[6];//Mac��ַ
	}BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_T;

	/********************************** ģ����� **********************************/
	/*����һ��ģ��*/
#define BEGP_REQ_PACKETDATA_ADD_MODULE_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_MODULE_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_MODULE_
	{
		char     Name[32];//������������
		char     Alias[64];//ģ��ı���
		char     Description[128];//ģ���������Ϣ
		char     Location[64];//ģ��İ�װλ��

		uint8_t  ModelID;//ģ���ͺű���
		uint8_t  FlnID;//ģ���������FLN�����ϣ�����ȡֵFLN1��FLN2��FLN3��FLN4
		uint8_t  SlaveAddress;//ģ���ͨ�ŵ�ַ
		uint8_t  Active;//ģ���ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
		uint16_t ID;//ģ���ʶ����
	}BEGP_REQ_PACKETDATA_ADD_MODULE_T;

	/* ����ģ����������ò��� */
#define BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_
	{
		char     Name[32];//������������
		char     Alias[64];//ģ��ı���
		char     Description[128];//ģ���������Ϣ
		char     Location[64];//ģ��İ�װλ��

		uint8_t  ModelID;//ģ���ͺű���
		uint8_t  FlnID;//ģ���������FLN�����ϣ�����ȡֵFLN1��FLN2��FLN3��FLN4
		uint8_t  SlaveAddress;//ģ���ͨ�ŵ�ַ
		uint8_t  Active;//ģ���ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
		uint16_t ID;//ģ���ʶ����
	}BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_T;

	/* ɾ��һ��ģ�� */
#define BEGP_REQ_PACKETDATA_DELETE_MODULE_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_MODULE_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_MODULE_
	{
		uint16_t ID;//ģ���ʶ����
	}BEGP_REQ_PACKETDATA_DELETE_MODULE_T;

	/* ��ȡģ�������������Ϣ */
	/* ����֡���� */
#define BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_
	{
		uint16_t ID;//ģ���ʶ����
	}BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_T;
	/* ��Ӧ֡���� */
#define BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_
	{
		char     Name[32];//������������
		char     Alias[64];//ģ��ı���
		char     Description[128];//ģ���������Ϣ
		char     Location[64];//ģ��İ�װλ��

		uint8_t  ModelID;//ģ���ͺű���
		uint8_t  FlnID;//ģ���������FLN�����ϣ�����ȡֵFLN1��FLN2��FLN3��FLN4
		uint8_t  SlaveAddress;//ģ���ͨ�ŵ�ַ
		uint8_t  Active;//ģ���ʹ��״̬ ACTIVE_ENABLE  ʹ��  ACTIVE_DISABLE ����
		uint16_t ID;//ģ���ʶ����
	}BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T;

	/* ��ȡģ��Ĵ������ */
	/*����֡����*/
#define BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_
	{
		uint16_t ID;//ģ���ʶ����
	}BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_T;
	/*����֡����*/
#define BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_
	{
		uint8_t ErrorCode;//ģ��Ĵ�����
	}BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T;

	/********************************** �߼������ **********************************/
	/* ģ��������ʵ��AI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LAI_
	{
		uint8_t  PointType; //������  ʵ��AI
		uint16_t ID; //���ID
		uint8_t  Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32]; //�������
		char     Alias[64]; //��ı���
		char     Description[64]; //����
		uint16_t ModuleID;  //������ģ���ID��
		uint8_t  ChannelIndex;  //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//ֵ

		/*�������*/
		uint8_t  AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t  AlarmType;  //�������ͣ�����������׼��������ǿ����
		uint8_t  AlarmPriority;  //�������ȼ�

		uint8_t  LineType;//��Ч�������ͣ�����0-10V��0-20mA��4-20mA
		char     Unit[12]; //���̵�λ����H���桢KWh
		int32_t  HighRange; //��߷�ֵ
		int32_t  LowRange;  //��ͷ�ֵ
		uint8_t  Precision;  //����
		int32_t  AlarmHighValue; //���ޱ���ֵ
		int32_t  AlarmLowValue;//���ޱ���ֵ
	}BEGP_LOGICAL_POINT_PARAMETER_LAI_T;
	/* ģ�������ʵ��AO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_
	{
		uint8_t  PointType;  //������  ʵ��AO
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		uint16_t ModuleID; //������ģ���ID��
		uint8_t  ChannelIndex; //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t LineType;  //��Ч������ͣ�����0-10V��0-20mA��4-20mA
		char    Unit[12];  //���̵�λ����H���桢KWh
		int32_t HighRange;  //��߷�ֵ
		int32_t LowRange;   //��ͷ�ֵ
		uint8_t Precision;   //����
		int32_t AlarmHighValue;  //���ޱ���ֵ
		int32_t AlarmLowValue;  //���ޱ���ֵ
	}BEGP_LOGICAL_POINT_PARAMETER_LAO_T;
	/* ����������ʵ��DI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LDI_
	{
		uint8_t  PointType;  //������    ʵ��DI
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		uint16_t ModuleID; //������ģ���ID��
		uint8_t  ChannelIndex; //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//ֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������
		uint8_t ActivePassive;//��Դ��Դ
	}BEGP_LOGICAL_POINT_PARAMETER_LDI_T;
	/* ���������ʵ��DO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LDO_
	{
		uint8_t  PointType;  //������   ʵ��DO
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		uint16_t ModuleID; //������ģ���ID��
		uint8_t  ChannelIndex; //������ͨ������
		uint8_t  WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		uint8_t  Polarity;  //���ԣ��������
		int32_t  InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������  �����
	}BEGP_LOGICAL_POINT_PARAMETER_LDO_T;
	/* ���AI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VAI_
	{
		uint8_t   PointType;  //������  ���AI
		uint16_t  ID; //���ID
		uint8_t   Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //�������
		char      Alias[64]; //��ı���
		char      Description[64];//����
		int32_t   InitValue;//��ʼֵ
		char      Unit[12];  //���̵�λ����H���桢KWh
		uint8_t   Precision;  //����

		/*�������*/
		uint8_t  AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t  AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t  AlarmPriority;   //�������ȼ�
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	}BEGP_LOGICAL_POINT_PARAMETER_VAI_T;
	/* ���AO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VAO_
	{
		uint8_t   PointType;  //������  ���AO
		uint16_t  ID; //���ID
		uint8_t   Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char      Name[32];  //�������
		char      Alias[64]; //��ı���
		char      Description[64];//����
		int32_t   InitValue;//��ʼֵ
		char      Unit[12];  //���̵�λ����H���桢KWh
		uint8_t   Precision;  //����

		/*�������*/
		uint8_t  AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t  AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t  AlarmPriority;   //�������ȼ�
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	}BEGP_LOGICAL_POINT_PARAMETER_VAO_T;
	/* ���DI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VDI_
	{
		uint8_t  PointType;  //������  ���DI
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		int32_t  InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������  �����
	}BEGP_LOGICAL_POINT_PARAMETER_VDI_T;
	/* ���DO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VDO_
	{
		uint8_t  PointType;  //������  ���DO
		uint16_t ID; //���ID
		uint8_t  Active;     //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�������
		char     Alias[64]; //��ı���
		char     Description[64];//����
		int32_t  InitValue;//��ʼֵ

		/*�������*/
		uint8_t AlarmActive;  //�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		uint8_t AlarmType;   //�������ͣ�����������׼��������ǿ����
		uint8_t AlarmPriority;   //�������ȼ�

		uint8_t AlarmTrigger;//��������  �����
	}BEGP_LOGICAL_POINT_PARAMETER_VDO_T;

	/* ����һ���߼��� */
#define BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_
	{
		uint8_t   PointType;//������
		char      Tab[sizeof(struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_)];//���鳤����ģ�������
	}BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_T;

	/* ����һ���߼�������в��� */
#define BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_
	{
		uint8_t   PointType;//������
		char      Tab[sizeof(struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_)];//���鳤����ģ�������
	}BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T;

	/*�����߼����ֵ*/
#define BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_
	{
		uint16_t  ID; //���ID
		uint8_t   WorkMode;  //����ģʽ���ֶ�ģʽ���Զ�ģʽ
		int32_t   Value;//��ʼֵ
	}BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T;

	/*�����߼����ֵ*/
#define BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_
	{
		char Name[64];
		float  Value;//��ʼֵ
	}BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T;

	/* ɾ��һ���߼��� */
#define BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_
	{
		uint16_t  ID; //���ID
	}BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T;


	/*��ȡһ���߼�������в���*/
	/* ����֡ */
#define BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_
	{
		uint16_t  ID; //���ID
	}BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T;
	/* ��Ӧ֡ */
#define BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_
	{
		uint8_t  PointType;//������
		char     Tab[sizeof(struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_)];//���鳤����ģ�������
	}BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T;

	/*��ȡһ���߼���ı�����Ϣ*/
	/*����֡*/
#define BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_
	{
		uint16_t  ID; //���ID
	}BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T;
	/*��Ӧ֡*/
#define  BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_
	{
		int32_t  CurrentValue;//��ȡ�ĵ�ǰֵ
		uint8_t  AlarmState;  //����״̬
		int32_t  AlarmTriggerValue; //�״α�������ֵ
		int32_t  AlarmMaxValue;     //�������ֵ(�������������һ�α���ֵ)
	}BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T;

	/********************************** �ͳ������ **********************************/
	/*���Ƴ���*/
	typedef struct _BEGP_CONTROL_POINT_
	{
		uint8_t    Active;   //ʹ��״̬
		uint16_t   PointID;	 //�ƻ�����Ӧ���ID��
		int32_t    RunValue; //����ֵ,������0��255��ģ�����Ǹ���ֵ
	}BEGP_CONTROL_POINT_T;
	typedef struct _BEGP_CONTROL_MODE_
	{
		uint8_t                Active;  //ʹ��״̬
		char                   Name[32];  //����ģʽ����
		uint8_t                ID;   //����ģʽID
		BEGP_CONTROL_POINT_T   ControlPoint[CONTROL_POINT_SIZE];//ģʽ�����Ϣ
	}BEGP_CONTROL_MODE_T;
	typedef struct _BEGP_CONTROL_SCENE_PARAMETER_
	{
		uint8_t               SceneType; //��������  ���ƻ�ɼ�������ȡ�ƻ������ֵ
		uint16_t              ID;       //���Ƴ�����ID
		uint8_t               Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char                  Name[32];  //��������
		char                  Alias[64]; //��������
		BEGP_CONTROL_MODE_T   ControlMode[CONTROL_MODE_SIZE]; //����ģʽ
	}BEGP_CONTROL_SCENE_PARAMETER_T;

	/* �ɼ����� */
	typedef struct _BEGP_COLLECT_POINT_
	{
		uint8_t     Active;   //ʹ��״̬
		uint16_t    PointID;	 //�ƻ�����Ӧ���ID��
		uint8_t     CollectID;//�ɼ���ʽ��ʱ�����ɼ���ֵ�ɼ�
		uint8_t     TimeUnit;//ʱ������λ
		int16_t     Value; //�ɼ�ʱ����ֵ���仯����ֵ
	}BEGP_COLLECT_POINT_T;
	typedef struct _BEGP_COLLECT_MODE_
	{
		uint8_t                  Active;  //ʹ��״̬
		char                     Name[32];  //����ģʽ����
		uint8_t                  ID;   //����ģʽID
		BEGP_COLLECT_POINT_T     CollectPoint[COLLECT_POINT_SIZE];//ģʽ�����Ϣ
	}BEGP_COLLECT_MODE_T;
	typedef struct _BEGP_COLLECT_SCENE_PARAMETER_
	{
		uint8_t              SceneType; //��������  ���ƻ�ɼ�������ȡ�ƻ������ֵ
		uint16_t             ID;        //���Ƴ�����ID
		uint8_t              Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char                 Name[32];  //��������
		char                 Alias[64]; //��������
		BEGP_COLLECT_MODE_T  CollectMode[COLLECT_MODE_SIZE]; //����ģʽ
	}BEGP_COLLECT_SCENE_PARAMETER_T;

	/* ���һ������ */
#define  BEGP_REQ_PACKETDATA_ADD_SCENE_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_SCENE_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_SCENE_
	{
		uint8_t SceneType; //��������  ���ƻ�ɼ�����
		char    Tab[sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) > sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_) ?
			sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) : sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_)];//ȡ���Ƴ����Ͳɼ������ϴ��ֵ
	}BEGP_REQ_PACKETDATA_ADD_SCENE_T;

	/*�޸�һ���������������ò���*/
#define  BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_
	{
		uint8_t SceneType; //��������  ���ƻ�ɼ�����
		char    Tab[sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) > sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_) ?
			sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) : sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_)];//ȡ���Ƴ����Ͳɼ������ϴ��ֵ
	}BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_T;

	/* ɾ��һ������ */
#define  BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_SCENE_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_SCENE_
	{
		uint16_t ID;//����ID	
	}BEGP_REQ_PACKETDATA_DELETE_SCENE_T;

	/* ��ȡһ��������������Ϣ */
	/* ����֡ */
#define  BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_) 
	typedef struct _BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_
	{
		uint16_t ID;//����ID
	}BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T;
	/* ��Ӧ֡ */
#define  BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_
	{
		uint8_t SceneType; //��������  ���ƻ�ɼ�����
		char    Tab[sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) > sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_) ?
			sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) : sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_)];//ȡ���Ƴ����Ͳɼ������ϴ��ֵ
	}BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T;

	/********************************** �ͼƻ���� **********************************/
	/* ���һ���ƻ� */
#define  BEGP_REQ_PACKETDATA_ADD_SCHEDULER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_ADD_SCHEDULER_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_SCHEDULER_
	{
		uint16_t ID;//�ƻ���ID��
		uint8_t  Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�ƻ�������
		char     Alias[64];  //����
		uint8_t  PlanType;  //�ƻ�����  ����ƻ���ڼ��ռƻ�

		uint8_t  StartDateYear;//��ʼ�����꣬��14
		uint8_t  StartDateMonth; //��ʼ�����£�1-12
		uint8_t  StartDateDay; //��ʼ�����գ�1-31
		uint8_t  EndDateYear; //����������
		uint8_t  EndDateMonth; //����������
		uint8_t  EndDateDay; //����������

		uint8_t  StartTimeHour;// ��ʼʱ��ʱ��0-23
		uint8_t  StartTimeMinute; //��ʼʱ���,0-59
		uint8_t  StartTimeSecond; //��ʼʱ����,0-59
		uint8_t  EndTimeHour;//����ʱ��ʱ
		uint8_t  EndTimeMinute;//����ʱ���
		uint8_t  EndTimeSecond;//����ʱ����

		uint8_t  ExecutionWay; //ִ�з�ʽ��ÿ��ִ�С�����ִ�У�
		uint8_t  WeekMask;	 //������
		uint8_t  SceneType; //�������ͣ����Ƴ�����ɼ�����
		uint16_t SceneID;  //����ID��
		uint8_t  ModeID;  //ģʽID��
		uint8_t  DefaultModeID;//Ĭ��ģʽID
	}BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T;

	/* ����һ���ƻ������в��� */
#define  BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_
	{
		uint16_t ID;//�ƻ���ID��
		uint8_t  Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�ƻ�������
		char     Alias[64];  //����
		uint8_t  PlanType;  //�ƻ�����  ����ƻ���ڼ��ռƻ�

		uint8_t  StartDateYear;//��ʼ�����꣬��14
		uint8_t  StartDateMonth; //��ʼ�����£�1-12
		uint8_t  StartDateDay; //��ʼ�����գ�1-31
		uint8_t  EndDateYear; //����������
		uint8_t  EndDateMonth; //����������
		uint8_t  EndDateDay; //����������

		uint8_t  StartTimeHour;// ��ʼʱ��ʱ��0-23
		uint8_t  StartTimeMinute; //��ʼʱ���,0-59
		uint8_t  StartTimeSecond; //��ʼʱ����,0-59
		uint8_t  EndTimeHour;//����ʱ��ʱ
		uint8_t  EndTimeMinute;//����ʱ���
		uint8_t  EndTimeSecond;//����ʱ����

		uint8_t  ExecutionWay; //ִ�з�ʽ��ÿ��ִ�С�����ִ�У�
		uint8_t  WeekMask;	 //������
		uint8_t  SceneType; //�������ͣ����Ƴ�����ɼ�����
		uint16_t SceneID;  //����ID��
		uint8_t  ModeID;  //ģʽID��
		uint8_t  DefaultModeID;//Ĭ��ģʽID
	}BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T;

	/* ɾ��һ���ƻ� */
#define BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_
	{
		uint16_t ID;//�ƻ���ID��
	}BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T;

	/* ��ȡһ���ƻ������в��� */
	/* ����֡ */
#define BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_
	{
		uint16_t ID;//�ƻ���ID��
	}BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T;
	/* ��Ӧ֡ */
#define BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_
	{
		uint8_t  Active;  //�Ƿ����ã�ACTIVE_ENABLEʹ�ܣ�ACTIVE_DISABLE��ֹ
		char     Name[32];  //�ƻ�������
		char     Alias[64];  //����
		uint8_t  PlanType;  //�ƻ�����  ����ƻ���ڼ��ռƻ�

		uint8_t  StartDateYear;//��ʼ�����꣬��14
		uint8_t  StartDateMonth; //��ʼ�����£�1-12
		uint8_t  StartDateDay; //��ʼ�����գ�1-31
		uint8_t  EndDateYear; //����������
		uint8_t  EndDateMonth; //����������
		uint8_t  EndDateDay; //����������

		uint8_t  StartTimeHour;// ��ʼʱ��ʱ��0-23
		uint8_t  StartTimeMinute; //��ʼʱ���,0-59
		uint8_t  StartTimeSecond; //��ʼʱ����,0-59
		uint8_t  EndTimeHour;//����ʱ��ʱ
		uint8_t  EndTimeMinute;//����ʱ���
		uint8_t  EndTimeSecond;//����ʱ����

		uint8_t  ExecutionWay; //ִ�з�ʽ��ÿ��ִ�С�����ִ�У�
		uint8_t  WeekMask;	 //������
		uint8_t  SceneType; //�������ͣ����Ƴ�����ɼ�����
		uint16_t SceneID;  //����ID��
		uint8_t  ModeID;  //ģʽID��
		uint8_t  DefaultModeID;//Ĭ��ģʽID	
	}BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_T;

#pragma pack()
}

#endif