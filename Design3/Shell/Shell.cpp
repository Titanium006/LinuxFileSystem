// Shell.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include"ipc.h"

#define BUF_SIZE 1024 * 1024 * 100 + 1

int UserCount;							//当前用户数量
int identification;						//Simdisk管理程序返回的通信使用的专用线程的线程号
HANDLE m_ThreadControl_Read[2];			//用于专用读线程运行控制的信号量句柄组
HANDLE m_ThreadControl_Write[2];		//用于专用写线程运行控制的信号量句柄组

HANDLE exitSemaphore;

wstring   m_cstrSzName[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName[2];//读数据的信号量名
wstring   m_WriteSemaphoreName[2];//写数据的信号量名
HANDLE  m_hMapFile[2];//共享内存句柄
char* m_pBuf[2];//读取数据的缓存
HANDLE	m_Read[2];//读数据的信号量句柄
HANDLE  m_Write[2];//写数据的信号量句柄
CRITICAL_SECTION critical_section;					//ReadSharedData和WriteSharedData之间的临界区
bool is_Write_turn;									//设置当前是否为WriteSharedData线程运行环节的布尔量
CONDITION_VARIABLE condition_variable;				//ReadSharedData和WriteSharedData之间的条件变量

wstring   m_cstrSzName_portal[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName_portal[2]; //读数据的信号量名
wstring   m_WriteSemaphoreName_portal[2];//写数据的信号量名
HANDLE  m_hMapFile_portal[2];//共享内存句柄
char* m_pBuf_portal[2]; //读取数据缓存
HANDLE	m_Read_portal[2];//读数据的信号量句柄
HANDLE  m_Write_portal[2];//写数据的信号量句柄
//用于两个线程间的同步
CRITICAL_SECTION critical_section_portal;			//ReadSharedData_portal和WriteSharedData_portal之间的临界区
int is_Write_turn_portal;							//设置当前是否为WriteSharedData_portal线程运行环节的布尔量
CONDITION_VARIABLE condition_variable_portal;		//ReadSharedData_portal和WriteSharedData_portal之间的条件变量

wstring   m_cstrSzName_one[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName_one[2]; //读数据的信号量名
wstring   m_WriteSemaphoreName_one[2];//写数据的信号量名
HANDLE  m_hMapFile_one[2];//共享内存句柄
char* m_pBuf_one[2]; //读取数据缓存
HANDLE	m_Read_one[2];//读数据的信号量句柄
HANDLE  m_Write_one[2];//写数据的信号量句柄
//用于两个线程间的同步
CRITICAL_SECTION critical_section_one;				//ReadSharedData_one和WriteSharedData_one之间的临界区
bool is_Write_turn_one;								//设置当前是否为WriteSharedData_one线程运行环节的布尔量
CONDITION_VARIABLE condition_variable_one;			//ReadSharedData_one和WriteSharedData_one之间的条件变量

int main()
{
	CreateFileMap();//创建共享内存
	system("title shell");
	cout << "正在连接Simdisk管理程序............" << endl;
	cout << "如果未见程序反应请重新尝试打开Shell程序" << endl;	
	UINT threadId;
	HANDLE hThread3 = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData_portal, 0, 0, &threadId);//创建写数据的线程
	if (hThread3 == NULL)
	{
		cout << "Starting WriteSharedData_portal Thread  Failed!" << endl;
	}
	HANDLE hThread4 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData_portal, 0, 0, &threadId);//创建读数据的线程s4
	if (hThread4 == NULL)
	{
		cout << "Starting ReadSharedData_portal Thread  Failed!" << endl;
	}
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData, 0, 0, &threadId);//创建写数据的线程
	if (hThread == NULL)
	{
		cout << "Starting WriteSharedData Thread  Failed!" << endl;
	}
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData, 0, 0, &threadId);//创建读数据的线程
	if (hThread2 == NULL)
	{
		cout << "Starting ReadSharedData Thread  Failed!" << endl;
	}
	HANDLE hThread5 = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData_one, 0, 0, &threadId);//创建写数据的线程
	if (hThread5 == NULL)
	{
		cout << "Starting WriteSharedData_one Thread  Failed!" << endl;
	}
	HANDLE hThread6 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData_one, 0, 0, &threadId);//创建读数据的线程
	if (hThread6 == NULL)
	{
		cout << "Starting ReadSharedData_one Thread  Failed!" << endl;
	}
	WaitForSingleObject(exitSemaphore, INFINITE);
	CloseFileMap();
	return 0;
}