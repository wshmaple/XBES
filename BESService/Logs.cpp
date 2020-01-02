#include "stdafx.h"
#include "Logs.h"
#include "time.h"


CLogs::CLogs()
{
}


CLogs::~CLogs()
{
}

int CLogs::LogPrint(char *pLogAddress, char *pLogString)
{
	//LOG_WARN(pLogString);

//
//	char LogFileName[512];
//#if 1
//	if (NULL != pLogAddress)
//	{
//		sprintf_s(LogFileName, "%s.log", pLogAddress);
//	}
//	else
//	{
//		sprintf_s(LogFileName, "%s.log", "BES200 DLL");
//	}
//	FILE* LogFile = NULL;
//	fopen_s(&LogFile, LogFileName, "at+");
//	if (NULL != LogFile){
//		char Logbuf[1512];
//		struct tm local;
//		time_t now;
//		time(&now);
//		localtime_s(&local, &now);
//		sprintf_s(Logbuf, "%d-%02d-%02d %02d:%02d:%02d %s\n",
//			local.tm_year + 1900,
//			local.tm_mon + 1,
//			local.tm_mday,
//			local.tm_hour,
//			local.tm_min,
//			local.tm_sec,
//			pLogString);
//
//		LOG_WARN(pLogString);
//
//		fputs(Logbuf, LogFile);
//		fclose(LogFile);
//		LogFile = NULL;
//	}
//#endif
	return 0;
}
