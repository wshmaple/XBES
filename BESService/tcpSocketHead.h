
#define TCP_TRYTIMES 2			//需要多次尝试


/*允许同时存在的SOCKET连接数*/
#define CLIENTSOCKET_MAX  500

/*数据包的最大长度*/
#define PACKAGE_SIZE_MAX 20*1024

/*SOCKET通信中可能出现的错误代码*/
typedef enum
{
	TCP_ERR_OK = 0,					//无错误
	TCP_ERR_SOCKET_ERR,				//建立SOCKET错误
	TCP_ERR_CONNECT_SETOUTTIME,		//连接失败，设置超时失败
	TCP_ERR_CONNECT_IOCTL,			//连接失败，设置SOCKET参数失败
	TCP_ERR_CONNECT_ADDRESS,		//连接失败，地址错误
	TCP_ERR_CONNECT_OUTTIME,		//连接失败，超时错误
	TCP_ERR_SEND,					//发送失败
	TCP_ERR_RECIVE,					//接收失败
	TCP_ERR_SEND_OUTTIME,			//发送超时
	TCP_ERR_RECIVE_OUTTIME,			//接收超时
	TCP_ERR_SEND_EXCEPT,			//发送失败，其他未知错误
	TCP_ERR_RECIVE_EXCEPT,			//接收失败，其他未知错误
	TCP_ERR_CONNECT_MAX 			//同时连接数达到最大
}TCP_ERR_T;

/*SOCKET应用的状态*/
typedef enum
{
	TCP_SOCKET_STATUS_ERR = 0,//错误
	TCP_SOCKET_STATUS_OK,//正常
}TCP_SOCKET_STATE_T;

/*SOCKET连接的状态*/
typedef enum
{
	TCP_CLIENTSOCKET_STATE_IDLE = 0,//空闲
	TCP_CLIENTSOCKET_STATE_DISCONN,//连接断开
	TCP_CLIENTSOCKET_STATE_CONN,//正在连接
}TCP_CLIENTSOCKET_STATE_T;
