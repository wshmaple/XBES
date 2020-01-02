// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

//#include "Log4Manager.h"
#include<string>
#include<map>
using namespace std;
#include "log4z.h"
#include <signal.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#if WIN32
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#endif
using namespace zsummer::log4z;
#pragma once
#ifndef _JSON_HPP_H_ 

#define _JSON_HPP_H_ 
#include "json.hpp"
#include "csv.hpp"
#endif



// TODO:  在此处引用程序需要的其他头文件
