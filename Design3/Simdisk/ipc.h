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
#include"simdisk.h"
#include <queue>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

constexpr auto BUF_SIZE = 1024;

constexpr auto UserCount = 2;	//设置最多同时并行的Shell程序个数

//用于存储Shell的用户信息的结构体
struct UserInfo
{
	string threadID;
	int id_to_WandR;
	char userID[10];
	char cur_dirN[128];
	Dir user_dir;
	user_type user_tType;
};
//用于存储所有连接的Shell的用户信息的向量
extern vector<UserInfo> ThreadID_UserID;

//用于“先来先服务”的排队信息结构体
struct QueuedNum
{
	int Requester;
	int ReqCount;
};

unsigned int __stdcall Dispatcher(void* pPM);	// 调度者线程
extern bool createError;		//创建错误状态位
extern HANDLE errorSema;		//保证互斥访问错误状态位的信号量句柄
extern int curShellCount;		//当前连接上Simdisk的Shell程序个数
extern HANDLE exitSema;			//让exit命令生效的信号量句柄

extern HANDLE tuSema;			//对ThreadID_UserID的读写访问保护信号量句柄
extern bool creatingDisk;		//md命令使用的创盘状态位
extern bool creatingUser;		//md命令使用的创建用户状态位
extern HANDLE hDSema;			//保证互斥访问隐藏目录名状态位的信号量句柄
extern int curShellNum;			//当前连接上Simdisk的Shell数量
extern bool hideDir_zero;		//隐藏目录名状态位（专用线程0）
extern bool hideDir_one;		//隐藏目录名状态位（专用线程1）
extern bool logfail;			//登陆失败状态位

extern HANDLE mutexSim_zero[3];	//读写线程（专用线程0）与Dispatcher线程实现顺序运行的信号量句柄组

unsigned int __stdcall Manager(void* pPM);	// 管理者线程
void ManagerInit();				//管理员线程的初始化函数
extern HANDLE m_ThreadControl_Read[2];		//用于专用读线程运行控制的信号量句柄组
extern HANDLE m_ThreadControl_Write[2];		//用于专用写线程运行控制的信号量句柄组
extern HANDLE m_ThreadControl_Simdisk;		//用于Simdisk主线程运行控制的信号量句柄组
extern int userCount;			//当前连接用户数
void Thread_useradd_p1(const char* strUserId, user_type user_tType);	//专用线程0的用户添加函数
void Thread_useradd_p2(const char* strUserId, user_type user_tType);	//专用线程1的用户添加函数
//用于两个线程、Dispatcher线程与Simdisk主线程之间对公共变量的互斥访问
extern HANDLE mutexSim[3];


void CloseFileMap();
void CreateFileMap();

extern stringstream sstream_toshell;		//Simdisk主线程实际操作的发送给Shell程序的字符串流
extern stringstream sstream_todisk;			///Simdisk主线程实际操作的发送给Simdisk主线程的字符串流

extern stringstream sstream_toshell_zero;	//专用线程0发送给Shell程序的字符串流
extern stringstream sstream_todisk_zero;	//专用线程0发送给Simdisk主线程的字符串流
extern wstring   m_cstrSzName[2];			//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName[2];	//读数据的信号量名
extern wstring   m_WriteSemaphoreName[2];	//写数据的信号量名
extern HANDLE  m_hMapFile[2];				//共享内存句柄
extern char* m_pBuf[2];						//读取数据缓存
extern char szInfo[BUF_SIZE];				//先存储系统返回信息，后再传入m_pBuf[0]
extern char getInfo[BUF_SIZE];
extern HANDLE	m_Read[2];					//读数据的信号量句柄
extern HANDLE  m_Write[2];					//写数据的信号量句柄

//用于两个线程间的顺序运行
extern CRITICAL_SECTION critical_section;		//ReadSharedData和WriteSharedData之间的临界区
extern bool is_Write_turn;						//设置当前是否为WriteSharedData线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable;	//ReadSharedData和WriteSharedData之间的条件变量


// 对于开发 Windows 平台的应用程序，__stdcall 是默认的函数调用约定
void SendInfoToSimdisk(char getInfo[BUF_SIZE]);
string GetInfoFromSimdisk();
unsigned int __stdcall WriteSharedData(void* pPM);
unsigned int __stdcall ReadSharedData(void* pPM);

extern stringstream sstream_toshell_portal;		//公共读写线程发送给Shell程序的字符串流
extern stringstream sstream_toManager_portal;	//公共读写线程发送给Manager线程的字符串流
extern wstring   m_cstrSzName_portal[2];		//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName_portal[2]; //读数据的信号量名
extern wstring   m_WriteSemaphoreName_portal[2];//写数据的信号量名
extern HANDLE  m_hMapFile_portal[2];			//共享内存句柄
extern char* m_pBuf_portal[2];					//读取数据缓存
extern char szInfo_portal[BUF_SIZE];			//先存储系统返回信息，后再传入m_pBuf[0]
extern char getInfo_portal[BUF_SIZE];
extern HANDLE	m_Read_portal[2];				//读数据的信号量句柄
extern HANDLE  m_Write_portal[2];				//写数据的信号量句柄

//用于两个线程间的同步
extern CRITICAL_SECTION critical_section_portal;		 //ReadSharedData_portal和WriteSharedData_portal之间的临界区
extern bool is_Write_turn_portal;						 //设置当前是否为WriteSharedData_portal线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable_portal;	 //ReadSharedData_portal和WriteSharedData_portal之间的条件变量
//用于ReadSharedData_portal、WriteSharedData_portal与Manager主线程之间的互斥访问
extern HANDLE mutexSim_portal[3];

//将接收到的信息发送给Manager线程
void SendInfoToManager(char getInfo[BUF_SIZE]);
// 对于开发 Windows 平台的应用程序，__stdcall 是默认的函数调用约定
unsigned int __stdcall WriteSharedData_portal(void* pPM);
unsigned int __stdcall ReadSharedData_portal(void* pPM);

extern stringstream sstream_toshell_one;		//专用线程1发送给Shell程序的字符串流
extern stringstream sstream_todisk_one;			//专用线程1发送给Simdisk主线程的字符串流
extern wstring   m_cstrSzName_one[2];			//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
extern wstring   m_ReadSemaphoreName_one[2];	//读数据的信号量名
extern wstring   m_WriteSemaphoreName_one[2];	//写数据的信号量名
extern HANDLE  m_hMapFile_one[2];				//共享内存句柄
extern char* m_pBuf_one[2];						//读取数据缓存
extern char szInfo_one[BUF_SIZE];				//先存储系统返回信息，后再传入m_pBuf_one[0]
extern char getInfo_one[BUF_SIZE];
extern HANDLE	m_Read_one[2];					//读数据的信号量句柄
extern HANDLE  m_Write_one[2];					//写数据的信号量句柄

//用于两个线程间的同步
extern CRITICAL_SECTION critical_section_one;		//ReadSharedData_one和WriteSharedData_one之间的临界区
extern bool is_Write_turn_one;						//设置当前是否为WriteSharedData_one线程运行环节的布尔量
extern CONDITION_VARIABLE condition_variable_one;	//ReadSharedData_one和WriteSharedData_one之间的条件变量
//用于两个线程与Simdisk主线程之间的互斥访问
extern HANDLE mutexSim_one[3];
void SendInfoToSimdisk_one(char getInfo[BUF_SIZE]);
string GetInfoFromSimdisk_one();
// 对于开发 Windows 平台的应用程序，__stdcall 是默认的函数调用约定
unsigned int __stdcall WriteSharedData_one(void* pPM);
unsigned int __stdcall ReadSharedData_one(void* pPM);