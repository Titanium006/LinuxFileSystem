// Shell.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include"ipc.h"

#define BUF_SIZE 1024 * 1024 * 100 + 1

wstring   m_cstrSzName[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName[2];//读数据的信号量名
wstring   m_WriteSemaphoreName[2];//写数据的信号量名
wstring   m_WriteBeforeRead;
HANDLE  m_hMapFile[2];//共享内存句柄
char* m_pBuf[2];//读取数据的缓存
HANDLE	m_Read[2];//读数据的信号量句柄
HANDLE  m_Write[2];//写数据的信号量句柄
HANDLE  writeBeforeRead;
bool is_Write_turn = false;//设置当前是否为WriteSharedData线程运行环节的布尔量
CRITICAL_SECTION critical_section;//ReadSharedData和WriteSharedData之间的临界区
CONDITION_VARIABLE condition_variable;//ReadSharedData和WriteSharedData之间的条件变量
HANDLE exitSemaphore;

int main()
{
	CreateFileMap();//创建共享内存
	system("title shell");
	cout << "正在连接Simdisk管理程序..........." << endl;
	UINT threadId;
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
	WaitForSingleObject(exitSemaphore, INFINITE);
	CloseFileMap();
	exit(0);
	return 0;
}