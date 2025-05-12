#pragma once
#pragma once
/*
ipc.h
关于共享内存通信的函数声明
*/
#include <windows.h>
#include <process.h>
#include<iostream>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

constexpr auto BUF_SIZE = 1024;

extern stringstream sstream;
extern wstring   m_cstrSzName[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName[2]; //读数据的信号量名
extern wstring   m_WriteSemaphoreName[2];//写数据的信号量名
extern wstring m_WriteBeforeRead;		//实现写后读的信号量名
extern HANDLE  m_hMapFile[2];//共享内存句柄
extern char* m_pBuf[2]; //读取数据缓存
extern char szInfo[BUF_SIZE];//先存储系统返回信息，后再传入m_pBuf[0]
extern char getInfo[BUF_SIZE];//存储从共享内存获取的数据
extern HANDLE	m_Read[2];//读数据的信号量句柄
extern HANDLE  m_Write[2];//写数据的信号量句柄
extern HANDLE writeBeforeRead;	//实现写后读的信号量句柄

extern CRITICAL_SECTION critical_section;//ReadSharedData和WriteSharedData之间的临界区
extern bool is_Write_turn;//设置当前是否为WriteSharedData线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable;//ReadSharedData和WriteSharedData之间的条件变量

extern HANDLE exitSemaphore;//退出信号量

void CloseFileMap();
void CreateFileMap();
unsigned int __stdcall WriteSharedData(void* pPM);
unsigned int __stdcall ReadSharedData(void* pPM);
