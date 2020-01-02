
#define TCP_TRYTIMES 2			//��Ҫ��γ���


/*����ͬʱ���ڵ�SOCKET������*/
#define CLIENTSOCKET_MAX  500

/*���ݰ�����󳤶�*/
#define PACKAGE_SIZE_MAX 20*1024

/*SOCKETͨ���п��ܳ��ֵĴ������*/
typedef enum
{
	TCP_ERR_OK = 0,					//�޴���
	TCP_ERR_SOCKET_ERR,				//����SOCKET����
	TCP_ERR_CONNECT_SETOUTTIME,		//����ʧ�ܣ����ó�ʱʧ��
	TCP_ERR_CONNECT_IOCTL,			//����ʧ�ܣ�����SOCKET����ʧ��
	TCP_ERR_CONNECT_ADDRESS,		//����ʧ�ܣ���ַ����
	TCP_ERR_CONNECT_OUTTIME,		//����ʧ�ܣ���ʱ����
	TCP_ERR_SEND,					//����ʧ��
	TCP_ERR_RECIVE,					//����ʧ��
	TCP_ERR_SEND_OUTTIME,			//���ͳ�ʱ
	TCP_ERR_RECIVE_OUTTIME,			//���ճ�ʱ
	TCP_ERR_SEND_EXCEPT,			//����ʧ�ܣ�����δ֪����
	TCP_ERR_RECIVE_EXCEPT,			//����ʧ�ܣ�����δ֪����
	TCP_ERR_CONNECT_MAX 			//ͬʱ�������ﵽ���
}TCP_ERR_T;

/*SOCKETӦ�õ�״̬*/
typedef enum
{
	TCP_SOCKET_STATUS_ERR = 0,//����
	TCP_SOCKET_STATUS_OK,//����
}TCP_SOCKET_STATE_T;

/*SOCKET���ӵ�״̬*/
typedef enum
{
	TCP_CLIENTSOCKET_STATE_IDLE = 0,//����
	TCP_CLIENTSOCKET_STATE_DISCONN,//���ӶϿ�
	TCP_CLIENTSOCKET_STATE_CONN,//��������
}TCP_CLIENTSOCKET_STATE_T;
