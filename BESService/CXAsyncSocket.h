#pragma once
//#include <afxsock.h>
class CXAsyncSocket 
	/*:
	public CAsyncSocket*/
{
public:
	CXAsyncSocket();
	~CXAsyncSocket();
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnSend(int nErrorCode);
};

