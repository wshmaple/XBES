#pragma once
class CLogs
{
public:
	CLogs();
	~CLogs();

public:
	/*生成日志文件*/
	int LogPrint(char *pLogAddress, char *pLogString);
};