#include"ipc.h"

void CloseFileMap()
{
	for (int i = 0; i < 2; i++)
	{
		if (identification == 1)
		{
			//4.撤销文件视图UnMapViewOfFile()
			UnmapViewOfFile(m_pBuf[i]);
			//5.关闭映射文件句柄CloseHandle()
			CloseHandle(m_hMapFile[i]);
		}
		else if (identification == 2)
		{
			//4.撤销文件视图UnMapViewOfFile()
			UnmapViewOfFile(m_pBuf_one[i]);
			//5.关闭映射文件句柄CloseHandle()
			CloseHandle(m_hMapFile_one[i]);
		}
		//4.撤销文件视图UnMapViewOfFile()
		UnmapViewOfFile(m_pBuf_portal[i]);
		//5.关闭映射文件句柄CloseHandle()
		CloseHandle(m_hMapFile_portal[i]);
	}
}
void CreateFileMap()
{
	UserCount = 2;
	for (int i = 0; i < UserCount; i++)
	{
		m_ThreadControl_Read[i] = CreateSemaphore(NULL, 0, 1, NULL);
		m_ThreadControl_Write[i] = CreateSemaphore(NULL, 0, 1, NULL);
	}

	m_cstrSzName[0] = L"NameOfMappingObject0";//共享内存0的名字，程序A和B中对该变量的命名必须相同！
	m_cstrSzName[1] = L"NameOfMappingObject1";//共享内存1的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName[0] = L"ReadSemaphoreName0";//读信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName[1] = L"ReadSemaphoreName1";//读信号量1的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName[0] = L"WriteSemaphoreName0";//写信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName[1] = L"WriteSemaphoreName1";//写信号量1的名字，程序A和B中对该变量的命名必须相同！

	m_cstrSzName_portal[0] = L"NameOfMappingObject0_portal";//共享内存0的名字，程序A和B中对该变量的命名必须相同！
	m_cstrSzName_portal[1] = L"NameOfMappingObject1_portal";//共享内存1的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_portal[0] = L"ReadSemaphoreName0_portal";//读信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_portal[1] = L"ReadSemaphoreName1_portal";//读信号量1的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_portal[0] = L"WriteSemaphoreName0_portal"; //写信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_portal[1] = L"WriteSemaphoreName1_portal"; //写信号量1的名字，程序A和B中对该变量的命名必须相同！

	m_cstrSzName_one[0] = L"NameOfMappingObject0_one";//共享内存0的名字，程序A和B中对该变量的命名必须相同！
	m_cstrSzName_one[1] = L"NameOfMappingObject1_one";//共享内存1的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_one[0] = L"ReadSemaphoreName0_one";//读信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_one[1] = L"ReadSemaphoreName1_one";//读信号量1的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_one[0] = L"WriteSemaphoreName0_one"; //写信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_one[1] = L"WriteSemaphoreName1_one"; //写信号量1的名字，程序A和B中对该变量的命名必须相同！

	exitSemaphore = CreateSemaphore(NULL, 0, 1, NULL);//退出信号量的名字
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

		//1.创建共享文件句柄 hMapFile，CreateFileMapping()函数创建一个文件映射内核对象
		m_hMapFile_portal[i] = CreateFileMapping(
			INVALID_HANDLE_VALUE,     //物理文件句柄，设为INVALID_HANDLE_VALUE（无效句柄）以创建一个进程间共享的对象
			NULL,				      //默认安全级别
			PAGE_READWRITE,           //权限可读可写
			0,						  //高位文件大小
			BUF_SIZE,				  //低位文件大小
			(LPCWSTR)m_cstrSzName_portal[i].c_str()			  //共享内存名
		);
		//2.获取指向文件视图的指针 pBuf，MapViewOfFile()函数负责把文件数据映射到进程的地址空间
		m_pBuf_portal[i] = (char*)MapViewOfFile(
			m_hMapFile_portal[i],				  //共享内存的句柄
			FILE_MAP_ALL_ACCESS,      //可读写
			0,
			0,
			BUF_SIZE
		);

		//1.创建共享文件句柄 hMapFile，CreateFileMapping()函数创建一个文件映射内核对象
		m_hMapFile_one[i] = CreateFileMapping(
			INVALID_HANDLE_VALUE,     //物理文件句柄，设为INVALID_HANDLE_VALUE（无效句柄）以创建一个进程间共享的对象
			NULL,				      //默认安全级别
			PAGE_READWRITE,           //权限可读可写
			0,						  //高位文件大小
			BUF_SIZE,				  //低位文件大小
			(LPCWSTR)m_cstrSzName_one[i].c_str()			  //共享内存名
		);
		//2.获取指向文件视图的指针 pBuf，MapViewOfFile()函数负责把文件数据映射到进程的地址空间
		m_pBuf_one[i] = (char*)MapViewOfFile(
			m_hMapFile_one[i],				  //共享内存的句柄
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
	is_Write_turn = false;

	m_Read_portal[0] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_portal[0].c_str());
	m_Write_portal[0] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_portal[0].c_str());
	m_Read_portal[1] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_portal[1].c_str());
	m_Write_portal[1] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_portal[1].c_str());
	InitializeCriticalSection(&critical_section_portal);
	InitializeConditionVariable(&condition_variable_portal);
	is_Write_turn_portal = 1;	//让_portal线程先写后读

	m_Read_one[0] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_one[0].c_str());
	m_Write_one[0] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_one[0].c_str());
	m_Read_one[1] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_one[1].c_str());
	m_Write_one[1] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_one[1].c_str());
	InitializeCriticalSection(&critical_section_one);
	InitializeConditionVariable(&condition_variable_one);
	is_Write_turn_one = false;
}
unsigned int __stdcall WriteSharedData(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Write[0], INFINITE);
	//3.将数据放到共享内存
	while (true)
	{
		EnterCriticalSection(&critical_section);
		// 如果不是写数据的轮次，则等待
		while (!is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}


		//通过m_pBuf[0]向共享内存m_hMapFile[0]发送数据
		WaitForSingleObject(m_Write[1], INFINITE);
		char szInfo[BUF_SIZE] = { 0 };
		gets_s(szInfo, BUF_SIZE);//c++11中用gets_s() 代替gets()
		memcpy(m_pBuf[1], szInfo, BUF_SIZE - 1);
		//如果用户输入清屏命令cls，则清屏
		if (strcmp(szInfo, "cls") == 0)
		{
			system("cls");
		}
		ReleaseSemaphore(m_Read[1], 1, NULL);

		//判断用户输入命令是否为exit，如果是，则发送信号退出Shell程序
		if (strcmp(szInfo, "exit") == 0)
		{
			ReleaseSemaphore(exitSemaphore, 1, NULL); // 发送信号量给main函数
			//break; // 退出循环，结束线程
		}

		is_Write_turn = false;
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
	}
	return true;
}
unsigned int __stdcall ReadSharedData(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Read[0], INFINITE);
	int counts = 0;
	// 3.从共享内存中读取数据
	while (true)
	{
		EnterCriticalSection(&critical_section);
		// 如果不是写数据的轮次，则等待
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}

		WaitForSingleObject(m_Read[0], INFINITE); //m_Read[1]信号量由程序B增加
		string str = m_pBuf[0];
		Sleep(200);
		cout << m_pBuf[0];
		ReleaseSemaphore(m_Write[0], 1, NULL);
		is_Write_turn = true;
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
		if (str.find("您已没有登陆机会，shell程序将退出") != string::npos)
		{
			Sleep(500);
			ReleaseSemaphore(exitSemaphore, 1, NULL);
		}
		counts++;
	}
	return true;
}

unsigned int __stdcall WriteSharedData_portal(void* pPM)
{
	//3.将数据放到共享内存
	while (true)
	{
		EnterCriticalSection(&critical_section_portal);
		// 如果不是写数据的轮次，则等待
		while (!is_Write_turn_portal)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}
		if (is_Write_turn_portal == 2)				//已接收到有效的id，退出该线程
		{
			WakeConditionVariable(&condition_variable_portal);
			LeaveCriticalSection(&critical_section_portal);
			break;
		}
		//通过m_pBuf[0]向共享内存m_hMapFile[0]发送数据
		WaitForSingleObject(m_Write_portal[1], INFINITE);
		//将本线程的唯一标识ID发送给Manager
		char szInfo_portal[BUF_SIZE] = { 0 };
		DWORD threadID = GetCurrentThreadId();
		string tmp = to_string(threadID);
		memcpy(m_pBuf_portal[1], tmp.c_str(), BUF_SIZE - 1);
		//system("cls");
		ReleaseSemaphore(m_Read_portal[1], 1, NULL);
		is_Write_turn_portal = 0;
		WakeConditionVariable(&condition_variable_portal);
		LeaveCriticalSection(&critical_section_portal);
	}
	return true;
}
unsigned int __stdcall ReadSharedData_portal(void* pPM)
{
	// 3.从共享内存中读取数据
	while (true)
	{
		EnterCriticalSection(&critical_section_portal);
		// 如果不是写数据的轮次，则等待
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}
		WaitForSingleObject(m_Read_portal[0], INFINITE); //m_Read[1]信号量由程序B增加

		//接收到Manager返回的用户通信标识
		identification = atoi(m_pBuf_portal[0]);
		if (identification > 0 && identification < 10)		//接收到有效的id，释放对应的线程进行通信，退出该线程
		{
			ReleaseSemaphore(m_ThreadControl_Read[identification - 1], 1, NULL);
			ReleaseSemaphore(m_ThreadControl_Write[identification - 1], 1, NULL);

			ReleaseSemaphore(m_Write_portal[0], 1, NULL);
			is_Write_turn_portal = 2;						//给WriteSharedData_portal线程传递信号，让它也退出
			WakeConditionVariable(&condition_variable_portal);
			LeaveCriticalSection(&critical_section_portal);
			break;
		}
		ReleaseSemaphore(m_Write_portal[0], 1, NULL);
		is_Write_turn_portal = 1;
		WakeConditionVariable(&condition_variable_portal);
		LeaveCriticalSection(&critical_section_portal);
	}
	return true;
}

unsigned int __stdcall WriteSharedData_one(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Write[1], INFINITE);
	//3.将数据放到共享内存
	while (true)
	{
		EnterCriticalSection(&critical_section_one);
		// 如果不是写数据的轮次，则等待
		while (!is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}

		//通过m_pBuf[0]向共享内存m_hMapFile[0]发送数据
		WaitForSingleObject(m_Write_one[1], INFINITE);
		char szInfo_one[BUF_SIZE] = { 0 };
		gets_s(szInfo_one, BUF_SIZE);//c++11中用gets_s() 代替gets()
		memcpy(m_pBuf_one[1], szInfo_one, BUF_SIZE - 1);
		//如果用户输入清屏命令cls，则清屏
		if (strcmp(szInfo_one, "cls") == 0)
		{
			system("cls");
		}
		ReleaseSemaphore(m_Read_one[1], 1, NULL);


		//判断用户输入命令是否为exit，如果是，则发送信号退出Shell程序
		if (strcmp(szInfo_one, "exit") == 0)
		{
			ReleaseSemaphore(exitSemaphore, 1, NULL); // 发送信号量给main函数
			//break; // 退出循环，结束线程
		}

		is_Write_turn_one = false;
		WakeConditionVariable(&condition_variable_one);
		LeaveCriticalSection(&critical_section_one);
	}
	return true;
}
unsigned int __stdcall ReadSharedData_one(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Read[1], INFINITE);
	int counts = 0;
	// 3.从共享内存中读取数据
	while (true)
	{
		EnterCriticalSection(&critical_section_one);
		// 如果不是写数据的轮次，则等待
		while (is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}
		WaitForSingleObject(m_Read_one[0], INFINITE); //m_Read[1]信号量由程序B增加

		string str = m_pBuf_one[0];
		Sleep(200);
		cout << m_pBuf_one[0];
		
		ReleaseSemaphore(m_Write_one[0], 1, NULL);
		is_Write_turn_one = true;
		WakeConditionVariable(&condition_variable_one);
		LeaveCriticalSection(&critical_section_one);
		counts++;
		if (str.find("您已没有登陆机会，shell程序将退出") != string::npos)
		{
			Sleep(500);
			ReleaseSemaphore(exitSemaphore, 1, NULL);
		}
	}
	return true;
}
