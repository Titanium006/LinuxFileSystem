#pragma once
#include<ctime>
#include<iostream>
#include<fstream>
#include<sstream>
#include<conio.h>
#include<iomanip>
#include<string>
#include<vector>
#include <windows.h>
#include <process.h>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

constexpr auto BUF_SIZE = 1024;

extern stringstream sstream_toshell;
extern stringstream sstream_todisk;
extern wstring   m_cstrSzName[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName[2]; //读数据的信号量名
extern wstring   m_WriteSemaphoreName[2];//写数据的信号量名
extern wstring   m_ReadBeforeWrite;		//读后写的信号量名
extern HANDLE  m_hMapFile[2];//共享内存句柄
extern char* m_pBuf[2]; //读取数据缓存
extern char szInfo[BUF_SIZE];//先存储系统返回信息，后再传入m_pBuf[0]
extern char getInfo[BUF_SIZE];
extern HANDLE	m_Read[2];//读数据的信号量句柄
extern HANDLE  m_Write[2];//写数据的信号量句柄

//用于两个线程间的有序运行
extern CRITICAL_SECTION critical_section;		//ReadSharedData和WriteSharedData之间的临界区
extern bool is_Write_turn;						//设置当前是否为WriteSharedData线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable;	//ReadSharedData和WriteSharedData之间的条件变量

//用于两个线程与Simdisk主线程之间的互斥访问
extern HANDLE mutexSim[3];

//用于simdisk主进程与写线程同步
//extern HANDLE mutexSimdisk;

void CloseFileMap();
void CreateFileMap();
// 对于开发 Windows 平台的应用程序，__stdcall 是默认的函数调用约定
unsigned int __stdcall WriteSharedData(void* pPM);
unsigned int __stdcall ReadSharedData(void* pPM);

//与Simdisk主线程进行通信的函数
void SendInfoToSimdisk(char getInfo[BUF_SIZE]);
string GetInfoFromSimdisk();
