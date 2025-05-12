#pragma once
#pragma once
/*
ipc.h
关于共享内存通信的函数声明
*/
#include <windows.h>
#include <process.h>
#include<iostream>
#include<string>
#include<sstream>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

constexpr auto BUF_SIZE = 1024;

extern int UserCount;					//当前用户数量
extern int identification;				//Simdisk管理程序返回的通信使用的专用线程的线程号
extern HANDLE m_ThreadControl_Read[2];	//用于专用读线程运行控制的信号量句柄组
extern HANDLE m_ThreadControl_Write[2];	//用于专用写线程运行控制的信号量句柄组

extern wstring   m_cstrSzName[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName[2]; //读数据的信号量名
extern wstring   m_WriteSemaphoreName[2];//写数据的信号量名
extern HANDLE  m_hMapFile[2];//共享内存句柄
extern char* m_pBuf[2]; //读取数据缓存
extern char szInfo[BUF_SIZE];//先存储系统返回信息，后再传入m_pBuf[0]
extern char getInfo[BUF_SIZE];//存储从共享内存获取的数据
extern HANDLE	m_Read[2];//读数据的信号量句柄
extern HANDLE  m_Write[2];//写数据的信号量句柄
extern CRITICAL_SECTION critical_section;					//ReadSharedData和WriteSharedData之间的临界区
extern bool is_Write_turn;									//设置当前是否为WriteSharedData线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable;				//ReadSharedData和WriteSharedData之间的条件变量
extern HANDLE exitSemaphore;//退出信号量
unsigned int __stdcall WriteSharedData(void* pPM);
unsigned int __stdcall ReadSharedData(void* pPM);

extern wstring   m_cstrSzName_portal[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName_portal[2]; //读数据的信号量名
extern wstring   m_WriteSemaphoreName_portal[2];//写数据的信号量名
extern HANDLE  m_hMapFile_portal[2];//共享内存句柄
extern char* m_pBuf_portal[2]; //读取数据缓存
extern char szInfo_portal[BUF_SIZE];//先存储系统返回信息，后再传入m_pBuf[0]
extern char getInfo_portal[BUF_SIZE];
extern HANDLE	m_Read_portal[2];//读数据的信号量句柄
extern HANDLE  m_Write_portal[2];//写数据的信号量句柄
//用于两个线程间的同步
extern CRITICAL_SECTION critical_section_portal;			//ReadSharedData_portal和WriteSharedData_portal之间的临界区
extern int is_Write_turn_portal;							//设置当前是否为WriteSharedData_portal线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable_portal;		//ReadSharedData_portal和WriteSharedData_portal之间的条件变量
// 对于开发 Windows 平台的应用程序，__stdcall 是默认的函数调用约定
unsigned int __stdcall WriteSharedData_portal(void* pPM);
unsigned int __stdcall ReadSharedData_portal(void* pPM);

extern wstring   m_cstrSzName_one[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName_one[2]; //读数据的信号量名
extern wstring   m_WriteSemaphoreName_one[2];//写数据的信号量名
extern HANDLE  m_hMapFile_one[2];//共享内存句柄
extern char* m_pBuf_one[2]; //读取数据缓存
extern char szInfo_one[BUF_SIZE];//先存储系统返回信息，后再传入m_pBuf[0]
extern char getInfo_one[BUF_SIZE];
extern HANDLE	m_Read_one[2];//读数据的信号量句柄
extern HANDLE  m_Write_one[2];//写数据的信号量句柄
//用于两个线程间的同步
extern CRITICAL_SECTION critical_section_one;				//ReadSharedData_one和WriteSharedData_one之间的临界区
extern bool is_Write_turn_one;								//设置当前是否为WriteSharedData_one线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable_one;			//ReadSharedData_one和WriteSharedData_one之间的条件变量
// 对于开发 Windows 平台的应用程序，__stdcall 是默认的函数调用约定
unsigned int __stdcall WriteSharedData_one(void* pPM);
unsigned int __stdcall ReadSharedData_one(void* pPM);

void CloseFileMap();
void CreateFileMap();