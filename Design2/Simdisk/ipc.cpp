/*
ipc.cpp
关于共享内存通信的函数定义
*/
#include"ipc.h"
#define _CRT_SECURE_NO_WARNINGS
void CloseFileMap()
{
	for (int i = 0; i < 2; i++)
	{
		//4.撤销文件视图UnMapViewOfFile()
		UnmapViewOfFile(m_pBuf[i]);
		//5.关闭映射文件句柄CloseHandle()
		CloseHandle(m_hMapFile[i]);
	}
}
void CreateFileMap()
{
	m_cstrSzName[0] = L"NameOfMappingObject0";//共享内存0的名字，程序A和B中对该变量的命名必须相同！
	m_cstrSzName[1] = L"NameOfMappingObject1";//共享内存1的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName[0] = L"ReadSemaphoreName0";//读信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName[1] = L"ReadSemaphoreName1";//读信号量1的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName[0] = L"WriteSemaphoreName0"; //写信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName[1] = L"WriteSemaphoreName1"; //写信号量1的名字，程序A和B中对该变量的命名必须相同！
	m_ReadBeforeWrite = L"ReadBeforeWriteSemaphore";//读后写信号量的名字
	for (int i = 0; i < 2; i++)
	{
		//1.创建共享文件句柄 hMapFile，CreateFileMapping()函数创建一个文件映射内核对象
		m_hMapFile[i] = CreateFileMapping(
			INVALID_HANDLE_VALUE,     //物理文件句柄，设为INVALID_HANDLE_VALUE（无效句柄）以创建一个进程间共享的对象
			NULL,				      //默认安全级别
			PAGE_READWRITE,           //权限可读可写
			0,						  //高位文件大小
			BUF_SIZE,				  //低位文件大小
			(LPCWSTR)m_cstrSzName[i].c_str()			  //共享内存名
		);
		//2.获取指向文件视图的指针 pBuf，MapViewOfFile()函数负责把文件数据映射到进程的地址空间
		m_pBuf[i] = (char*)MapViewOfFile(
			m_hMapFile[i],				  //共享内存的句柄
			FILE_MAP_ALL_ACCESS,      //可读写
			0,
			0,
			BUF_SIZE
		);
	}
	m_Read[0] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName[0].c_str());
	m_Write[0] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName[0].c_str());
	m_Read[1] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName[1].c_str());
	m_Write[1] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName[1].c_str());

	InitializeCriticalSection(&critical_section);

	InitializeConditionVariable(&condition_variable);

	mutexSim[0] = CreateSemaphore(NULL, 1, 1, NULL);
	mutexSim[1] = CreateSemaphore(NULL, 0, 1, NULL);
	mutexSim[2] = CreateSemaphore(NULL, 0, 1, NULL);
}
unsigned int __stdcall WriteSharedData(void* pPM)
{
	while (true)
	{
		//进入临界区
		EnterCriticalSection(&critical_section);
		// 如果不是写数据的轮次，则等待
		while (!is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}

		//3.将数据放到共享内存
			//通过m_pBuf[0]向共享内存m_hMapFile[0]发送数据
		WaitForSingleObject(m_Write[0], INFINITE);
		char szInfo[BUF_SIZE] = { 0 };	// 新增
		// 写一个功能函数，从主程序中读取输出
		string message = GetInfoFromSimdisk();
		strcpy_s(szInfo, message.c_str());	// 不能函数返回值const char* 否则会出现乱码现象，也不能先转c_str()再+'\n'，这样也会出现乱码
		strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);
		ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据
		is_Write_turn = false;
		//释放条件变量并离开临界区
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
	}
	return true;
}
unsigned int __stdcall ReadSharedData(void* pPM)
{
	while (true)
	{
		//进入临界区
		EnterCriticalSection(&critical_section);
		// 如果是写数据的轮次，则等待
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}

		//3.从共享内存中读取数据
		WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]信号量由程序B增加
		char getInfo[BUF_SIZE] = { 0 };		// 新增
		strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);
		cout << "\n读到的信息是：" << getInfo << endl;
		SendInfoToSimdisk(getInfo);
		ReleaseSemaphore(m_Write[1], 1, NULL);
		is_Write_turn = true;
		//释放条件变量并离开临界区
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
	}
	return true;
}

//将ReadSharedData读到的信息传送给Simdisk主线程
void SendInfoToSimdisk(char getInfo[BUF_SIZE])
{
	WaitForSingleObject(mutexSim[2], INFINITE);
	sstream_todisk.str(string());
	sstream_todisk.clear();
	sstream_todisk << getInfo;
	ReleaseSemaphore(mutexSim[0], 1, NULL);
}

//将Simdisk主线程处理完成的信息传给WriteSharedData
string GetInfoFromSimdisk()
{
	WaitForSingleObject(mutexSim[1], INFINITE);
	string msg = sstream_toshell.str();	//这里不会把sstream_toshell内容清空！
	sstream_toshell.str(string());
	sstream_toshell.clear();
	ReleaseSemaphore(mutexSim[2], 1, NULL);
	return msg;
}
