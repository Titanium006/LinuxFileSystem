/*
ipc.cpp
关于共享内存通信的函数定义
*/
#include"ipc.h"
#include"func.h"
#define _CRT_SECURE_NO_WARNINGS

// 第一个线程组的变量
bool succeed = false;
HANDLE succeedSema = CreateSemaphore(NULL, 1, 1, NULL);

//判断是否完成当前线程
HANDLE DoneSema_zero = CreateSemaphore(NULL, 0, 1, NULL);
HANDLE DoneSema_one = CreateSemaphore(NULL, 0, 1, NULL);

// 第二个线程组的变量
bool succeed_one = false;
HANDLE succeedSema_one = CreateSemaphore(NULL, 1, 1, NULL);

// 请求队列及其访问控制信号量
queue<QueuedNum> RequestQueue;
HANDLE reqSema = CreateSemaphore(NULL, 1, 1, NULL);

bool twoStep_zero = false;				//是否需要两步操作的状态位（专用线程0）
bool twoStep_one = false;				//是否需要两步操作的状态位（专用线程1）
bool hideDir_twice_zero = false;		//是否需要隐藏当前目录名 2 次的状态位（专用线程0）
bool hideDir_twice_one = false;			//是否需要隐藏当前目录名 2 次的状态位（专用线程1）
bool threeStep_zero = false;			//是否需要三步操作的状态位（专用线程0）
bool threeStep_one = false;				//是否需要三步操作的状态位（专用线程1）
bool fourStep_zero = false;				//是否需要四步操作的状态位（专用线程0）
bool fourStep_one = false;				//是否需要四步操作的状态位（专用线程1）
bool fiveStep_zero = false;				//是否需要五步操作的状态位（专用线程0）
bool fiveStep_one = false;				//是否需要五步操作的状态位（专用线程1）

int CountSim_zero = 0;		//判断是否第一次连接Simdisk主线程
int CountSim_one = 0;		//判断是否第一次连接Simdisk主线程

//调度者线程
unsigned int __stdcall Dispatcher(void* pPM)
{
	int dCount_zero = 0;	//判断是否第一次成功运行Dispatcher线程
	while (true)
	{
		//检查请求队列RequestQueue，看看是否有命令到来
		WaitForSingleObject(reqSema, INFINITE);
		if (!RequestQueue.empty())	//有命令
		{
			QueuedNum tmp;
			tmp = RequestQueue.front();
			RequestQueue.pop();
			ReleaseSemaphore(reqSema, 1, NULL);
			int i = 0;		//实际运行次数统计
			while (i < tmp.ReqCount)
			{
				//按照命令请求的专用线程号分开处理
				if (tmp.Requester == 0)		//专用线程0
				{
					//设置当前请求的Shell所对应专用线程号
					WaitForSingleObject(hDSema, INFINITE);
					curShellNum = 0;
					ReleaseSemaphore(hDSema, 1, NULL);
					//检查sstream_todisk_zero流是否为空
					WaitForSingleObject(mutexSim_zero[1], INFINITE);
					if (!sstream_todisk_zero.str().empty())
					{
						sstream_todisk_zero.str(string());
						sstream_todisk_zero.clear();
						ReleaseSemaphore(mutexSim_zero[1], 1, NULL);
						//如果第一次成功运行Dispatcher线程，则需要对mutexSim信号量句柄组进行一次操作以使环能够顺利运行
						if (dCount_zero == 0)
						{
							WaitForSingleObject(mutexSim[1], INFINITE);
							ReleaseSemaphore(mutexSim[2], 1, NULL);
							dCount_zero = 1;
						}
						ReleaseSemaphore(mutexSim_zero[2], 1, NULL);
						//等待运行成功的信号量
						WaitForSingleObject(DoneSema_zero, INFINITE);
						//检查Simdisk主线程是否报错
						WaitForSingleObject(errorSema, INFINITE);
						if (createError)
							i = tmp.ReqCount;
						else
							i++;
						ReleaseSemaphore(errorSema, 1, NULL);
					}
					else
						ReleaseSemaphore(mutexSim_zero[1], 1, NULL);
				}
				else if (tmp.Requester == 1)	//专用线程1
				{
					//设置当前请求的Shell所对应专用线程号
					WaitForSingleObject(hDSema, INFINITE);
					curShellNum = 1;
					ReleaseSemaphore(hDSema, 1, NULL);
					//检查sstream_todisk_one流是否为空
					WaitForSingleObject(mutexSim_one[1], INFINITE);
					if (!sstream_todisk_one.str().empty())
					{
						sstream_todisk_one.str(string());
						sstream_todisk_one.clear();
						ReleaseSemaphore(mutexSim_one[1], 1, NULL);
						//如果第一次成功运行Dispatcher线程，则需要对mutexSim信号量句柄组进行一次操作以使环能够顺利运行
						if (dCount_zero == 0)
						{
							WaitForSingleObject(mutexSim[1], INFINITE);
							ReleaseSemaphore(mutexSim[2], 1, NULL);
							dCount_zero = 1;
						}
						ReleaseSemaphore(mutexSim_one[2], 1, NULL);
						//等待运行成功的信号量
						WaitForSingleObject(DoneSema_one, INFINITE);
						//检查Simdisk主线程是否报错
						WaitForSingleObject(errorSema, INFINITE);
						if (createError)
							i = tmp.ReqCount;
						else
							i++;
						ReleaseSemaphore(errorSema, 1, NULL);
					}
					else
						ReleaseSemaphore(mutexSim_one[1], 1, NULL);
				}
			}
		}
		else
		{
			ReleaseSemaphore(reqSema, 1, NULL);
		}
	}
	return 0;
}

unsigned int __stdcall Manager(void* pPM)
{
	vector<string> threadID;		//用于存储用户线程的唯一标识符id
	while (1)	//在每一次有效的循环里面给新来的用户分配好所需的资源并做好调度
	{
		WaitForSingleObject(mutexSim_portal[0], INFINITE);
		string id;
		sstream_toManager_portal >> id;
		if (find(threadID.begin(), threadID.end(), id) == threadID.end())	//没有找到用户线程的id
		{
			userCount++;
			curShellCount++;
			UserInfo tmp;
			tmp.threadID = id;
			tmp.id_to_WandR = userCount;			//从1开始设定对应的读写线程

			// 设置tuSema对ThreadID_UserID向量进行读写保护
			WaitForSingleObject(tuSema, INFINITE);
			ThreadID_UserID.push_back(tmp);			//存储收到的线程标识符
			ReleaseSemaphore(tuSema, 1, NULL);

			threadID.push_back(id);

			//释放Simdisk程序对应的线程进行通信
			ReleaseSemaphore(m_ThreadControl_Read[userCount - 1], 1, NULL);
			ReleaseSemaphore(m_ThreadControl_Write[userCount - 1], 1, NULL);

			ReleaseSemaphore(mutexSim_portal[1], 1, NULL);	//让write线程给用户发送用户标识符以确定使用哪个线程进行通信
		}
		else
		{
			continue;	//等待下一个用户连接
		}
	}
	return true;
}

void ManagerInit()
{
	// 初始化错误状态位
	createError = false;
	errorSema = CreateSemaphore(NULL, 1, 1, NULL);
	//判断当前用户数是否应该退出
	curShellCount = 0;
	exitSema = CreateSemaphore(NULL, 1, 1, NULL);

	hDSema = CreateSemaphore(NULL, 1, 1, NULL);
	curShellNum = -1;

	tuSema = CreateSemaphore(NULL, 1, 1, NULL);

	userCount = 0;	//初始化当前连接用户数为0
	for (int i = 0; i < UserCount; i++)
	{
		m_ThreadControl_Read[i] = CreateSemaphore(NULL, 0, 1, NULL);
		m_ThreadControl_Write[i] = CreateSemaphore(NULL, 0, 1, NULL);
	}
	m_ThreadControl_Simdisk = CreateSemaphore(NULL, 0, 1, NULL);		//这里绝对不能用(LPCWSTR)"ThreadControl_Simdisk"命名，否则会出现启动多线程通信以后信号量直接失效的问题！！！

	m_cstrSzName_portal[0] = L"NameOfMappingObject0_portal";//共享内存0的名字，程序A和B中对该变量的命名必须相同！
	m_cstrSzName_portal[1] = L"NameOfMappingObject1_portal";//共享内存1的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_portal[0] = L"ReadSemaphoreName0_portal";//读信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_portal[1] = L"ReadSemaphoreName1_portal";//读信号量1的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_portal[0] = L"WriteSemaphoreName0_portal"; //写信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_portal[1] = L"WriteSemaphoreName1_portal"; //写信号量1的名字，程序A和B中对该变量的命名必须相同！

	for (int i = 0; i < 2; i++)
	{
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
	}

	m_Read_portal[0] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_portal[0].c_str());
	m_Write_portal[0] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_portal[0].c_str());
	m_Read_portal[1] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_portal[1].c_str());
	m_Write_portal[1] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_portal[1].c_str());
	InitializeCriticalSection(&critical_section_portal);
	InitializeConditionVariable(&condition_variable_portal);
	mutexSim_portal[0] = CreateSemaphore(NULL, 0, 1, NULL);
	mutexSim_portal[1] = CreateSemaphore(NULL, 0, 1, NULL);
	mutexSim_portal[2] = CreateSemaphore(NULL, 1, 1, NULL);
	is_Write_turn_portal = false;	//让中转线程先读后写
}

void Thread_useradd_p1(const char* strUserId, user_type user_tType)
{
	//如果当前用户类型不是系统或者管理员则报错没有权限创建账户
	if (cur_user_tType != SYSTEM && cur_user_tType != ADMIN) {
		error(NO_PRIVILEGE, "user");
		return;
	}
	if (cur_Idsum < MAX_USER_NUM)	//没有超过限制
	{
		//查找是否存在同名账号
		for (unsigned int i = 0; i < cur_Idsum; i++)
		{
			//如果账号同名则报错用户已存在
			if (strcmp(systemUsers[i].get_user_id(), strUserId) == 0)
			{
				error(USER_EXIST, strUserId);
				return;
			}
		}
		//初始化
		cout << "密码(最长20位)：";
		sstream_toshell_zero << "密码(最长20位)：";
	}
	else
	{
		error(USERS_MAX);
	}
}

void CloseFileMap()
{
	for (int i = 0; i < 2; i++)
	{
		//4.撤销文件视图UnMapViewOfFile()
		UnmapViewOfFile(m_pBuf[i]);
		//5.关闭映射文件句柄CloseHandle()
		CloseHandle(m_hMapFile[i]);

		//4.撤销文件视图UnMapViewOfFile()
		UnmapViewOfFile(m_pBuf_portal[i]);
		//5.关闭映射文件句柄CloseHandle()
		CloseHandle(m_hMapFile_portal[i]);

		//4.撤销文件视图UnMapViewOfFile()
		UnmapViewOfFile(m_pBuf_one[i]);
		//5.关闭映射文件句柄CloseHandle()
		CloseHandle(m_hMapFile_one[i]);
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


	m_cstrSzName_one[0] = L"NameOfMappingObject0_one";//共享内存0的名字，程序A和B中对该变量的命名必须相同！
	m_cstrSzName_one[1] = L"NameOfMappingObject1_one";//共享内存1的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_one[0] = L"ReadSemaphoreName0_one";//读信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_ReadSemaphoreName_one[1] = L"ReadSemaphoreName1_one";//读信号量1的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_one[0] = L"WriteSemaphoreName0_one"; //写信号量0的名字，程序A和B中对该变量的命名必须相同！
	m_WriteSemaphoreName_one[1] = L"WriteSemaphoreName1_one"; //写信号量1的名字，程序A和B中对该变量的命名必须相同！
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
	mutexSim[0] = CreateSemaphore(NULL, 1, 1, NULL);
	mutexSim[1] = CreateSemaphore(NULL, 0, 1, NULL);
	mutexSim[2] = CreateSemaphore(NULL, 0, 1, NULL);
	is_Write_turn = true;

	mutexSim_zero[0] = CreateSemaphore(NULL, 0, 1, NULL);
	mutexSim_zero[1] = CreateSemaphore(NULL, 1, 1, NULL);
	mutexSim_zero[2] = CreateSemaphore(NULL, 0, 1, NULL);

	m_Read_one[0] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_one[0].c_str());
	m_Write_one[0] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_one[0].c_str());
	m_Read_one[1] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_one[1].c_str());
	m_Write_one[1] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_one[1].c_str());
	InitializeCriticalSection(&critical_section_one);
	InitializeConditionVariable(&condition_variable_one);
	mutexSim_one[0] = CreateSemaphore(NULL, 0, 1, NULL);
	mutexSim_one[1] = CreateSemaphore(NULL, 1, 1, NULL);
	mutexSim_one[2] = CreateSemaphore(NULL, 0, 1, NULL);
	is_Write_turn_one = true;
}

unsigned int __stdcall WriteSharedData(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Write[0], INFINITE);
	int process = 1;
	char ID[MAX_ID_LENGTH];
	unsigned int nCount = 3;
	int tmp = 0;
	bool exitting = false;

	//设置在专用线程0中的登录模块
	while (true)
	{
		if (cur_Idsum == 0)			// 进行管理员账户的注册和初始化（意味着如果有线程在登录认证（特别是第一个管理员账户注册）
			//的时候，其他线程接入都必须要被阻塞等待，或者至少是推迟修改userCount！）
		{
			switch (process) {
			case 1: {				//发送的第一个信息：先读入账户信息
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				WaitForSingleObject(m_Write[0], INFINITE);


				cout << "\n\n******************************\n";
				sstream_toshell_zero << "\n\n******************************\n";
				cout << "正在初始化用户信息……\n";
				sstream_toshell_zero << "正在初始化用户信息……\n";
				cout << "注册管理员账户中……\n";
				sstream_toshell_zero << "注册管理员账户中……\n";
				//register_user()
				creatingUser = true;
				cmd_md("user", READ_ONLY);
				creatingUser = false;
				cout << "账号(最长10位)：";
				sstream_toshell_zero << "账号(最长10位)：";

				char szInfo[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());	// 不能函数返回值const char* 否则会出现乱码现象，也不能先转c_str()再+'\n'，这样也会出现乱码
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//输出完成必须要重置流对象!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;							//让下一次循环进入登录认证的第二个步骤
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}
			case 2: {				//发送的第二个信息：
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				WaitForSingleObject(m_Write[0], INFINITE);

				char szInfo[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//输出完成必须要重置流对象!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;							//让下一次循环进入登录认证的第二个步骤
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}	//Load()过完了
			default: {				//发送剩余的信息
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				WaitForSingleObject(succeedSema, INFINITE);
				if (succeed)exitting = true;
				ReleaseSemaphore(succeedSema, 1, NULL);
				if (exitting)
				{
					LeaveCriticalSection(&critical_section);
					break;
				}

				WaitForSingleObject(m_Write[0], INFINITE);

				char szInfo[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//输出完成必须要重置流对象!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();
				//exitting = true;

				process++;
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}
			}
			if (exitting)break;
		}
		else
		{
			switch (process)
			{
			case 1:
			{
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}

				//进入cmd_logon()
				cout << "当前还有 " << nCount << " 次机会" << endl;
				cout << "模拟Linux文件系统\n请先登录账号\n\n";
				sstream_toshell_zero << "模拟Linux文件系统\n请先登录账号\n\n";
				//输入帐号
				cout << "账号：";
				sstream_toshell_zero << "账号：";

				char szInfo[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//输出完成必须要重置流对象!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}
			default:
			{
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				WaitForSingleObject(succeedSema, INFINITE);
				if (succeed)exitting = true;
				ReleaseSemaphore(succeedSema, 1, NULL);
				if (exitting)
				{
					LeaveCriticalSection(&critical_section);
					break;
				}

				WaitForSingleObject(m_Write[0], INFINITE);

				char szInfo[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//输出完成必须要重置流对象!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}
			}
			if (exitting)break;
		}
	}
	int relsimdisk = 0;
	int getCount = 1;
	while (true)
	{
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
		if (relsimdisk == 0 && succeed_one == false)
		{
			ReleaseSemaphore(m_ThreadControl_Simdisk, 1, NULL);		//释放信号量让Simdisk主线程运行
			relsimdisk = 1;
		}
		string message = GetInfoFromSimdisk();
		//cout << "\n-------------------------\nMessage to Send is:\n" << message << endl;
		//cout << "Length of Message is-----------" << message.length() << endl;
		strcpy_s(szInfo, (message).c_str());	// 不能函数返回值const char* 否则会出现乱码现象，也不能先转c_str()再+'\n'，这样也会出现乱码
		strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);
		ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据

		is_Write_turn = false;
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
		if (getCount != 1)
		{
			ReleaseSemaphore(DoneSema_zero, 1, NULL);
		}
		else
			getCount = 0;
	}
	return true;
}
unsigned int __stdcall ReadSharedData(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Read[0], INFINITE);
	char ID[MAX_ID_LENGTH];
	char password[MAX_PWD_LENGTH];
	char nUid[MAX_ID_LENGTH];
	char strPwd[MAX_PWD_LENGTH];
	int process = 1;
	int nCount = 3;
	int proc = 1;//控制是输账号还是密码
	bool success = false;
	int nCountForDirectLog = 2;

	//设置在专用线程0中的登录模块
	while (true)
	{
		if (cur_Idsum == 0)
		{
			switch (process)
			{
			case 1:
			{
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				//3.从共享内存中读取数据
				WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]信号量由程序B增加

				char getInfo[BUF_SIZE] = { 0 };		// 新增
				strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);
				//cin_id()
				stringstream ss;
				ss << getInfo;
				int nlen = ss.str().length();
				strcpy_s(ID, MAX_ID_LENGTH, ss.str().c_str());
				ID[nlen] = '\0';
				cout << endl;
				sstream_toshell_zero << '\n';
				//cmd_useradd()
				Thread_useradd_p1(ID, ADMIN);

				process++;
				ReleaseSemaphore(m_Write[1], 1, NULL);
				is_Write_turn = true;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}
			case 2:
			{
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				//3.从共享内存中读取数据
				WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]信号量由程序B增加
				char getInfo[BUF_SIZE] = { 0 };		// 新增
				strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);

				//cin_pwd()
				stringstream ss;
				ss << getInfo;
				int nlen = MAX_PWD_LENGTH;
				strcpy_s(password, MAX_PWD_LENGTH, ss.str().c_str());
				stringstream ssStream;
				ssStream << password << " " << (int)ADMIN;
				char strBuffer[MAX_PWD_LENGTH + 5];//建立字符串缓冲区来接受用户输入的密码
				strcpy_s(strBuffer, 25, ssStream.str().c_str());
				ssStream.clear();
				//格式化完成，写入文件
				Dir dirUsr;
				analyse_path("/user", dirUsr);
				char* tem = new char[MAX_NAME_LENGTH];
				strcpy_s(tem, 10, ID);
				save_file(tem, strBuffer, MAX_PWD_LENGTH + 5, dirUsr, PROTECTED);
				//添加一个用户到内存
				cur_Idsum++;
				systemUsers[cur_Idsum - 1].set_user_info(ID, password, ADMIN);
				//读取用户信息
				get_user_info();
				memset(cur_user_Id, 0, MAX_ID_LENGTH);
				strcpy_s(cur_user_Id, 10, systemUsers[0].get_user_id());	//因为是注册的第一个用户所以放到[0]号位置
				cur_user_tType = systemUsers[0].get_user_type();
				//初始化完成，返回
				cout << "\n******************************\n";
				sstream_toshell_zero << "\n******************************\n";
				cout << "初始化完成！正在转入登录界面……\n";
				sstream_toshell_zero << "初始化完成！正在转入登录界面……\n";
				cout << "**********************磁盘初始化完成**********************" << endl;
				sstream_toshell_zero << "**********************磁盘初始化完成**********************\n";
				get_user_info();

				//进入cmd_logon()
				nCount--;
				cout << "模拟Linux文件系统\n请先登录账号\n\n";
				sstream_toshell_zero << "模拟Linux文件系统\n请先登录账号\n\n";
				//输入帐号
				cout << "账号：";
				sstream_toshell_zero << "账号：";

				process++;
				ReleaseSemaphore(m_Write[1], 1, NULL);
				is_Write_turn = true;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}		//Load()过完了
			default:
			{
				if (nCount >= 0)
				{
					EnterCriticalSection(&critical_section);
					// 如果不是写数据的轮次，则等待
					while (is_Write_turn)
					{
						SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
					}
					//3.从共享内存中读取数据
					WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]信号量由程序B增加
					char getInfo[BUF_SIZE] = { 0 };		// 新增
					strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);

					if (proc % 2 == 1)
					{
						//cin_id()
						stringstream ss;
						ss << getInfo;
						int nlen = ss.str().length();
						strcpy_s(nUid, MAX_ID_LENGTH, ss.str().c_str());
						nUid[nlen] = '\0';
						cout << endl;
						sstream_toshell_zero << '\n';
						//输入密码，*处理
						cout << "密码：";
						sstream_toshell_zero << "密码：";
						proc++;
					}
					else
					{
						//cin_pwd()
						stringstream ss;
						ss << getInfo;
						int nlen = ss.str().length();
						strcpy_s(strPwd, MAX_ID_LENGTH, ss.str().c_str());

						for (unsigned int i = 0; i < cur_Idsum; i++)
						{
							// 比较密码是否正确
							//cout << systemUsers[i].get_user_id() << endl;
							//cout << systemUsers[i].UserPWD << endl;
							if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
								&& systemUsers[i].cmp_pwd(strPwd))
							{
								//未进行多用户并发的cur_user_Id!!!
								memset(cur_user_Id, 0, MAX_ID_LENGTH);
								strcpy_s(cur_user_Id, 10, nUid);
								cur_user_tType = systemUsers[i].get_user_type();
								cout << "\n\n******************************\n";
								sstream_toshell_zero << "\n\n******************************\n";
								cout << "登录成功！请稍候……\n";
								sstream_toshell_zero << "登录成功！请稍候……\n";
								Sleep(800);

								WaitForSingleObject(tuSema, INFINITE);
								strcpy_s(ThreadID_UserID[0].userID, cur_user_Id);
								ReleaseSemaphore(tuSema, 1, NULL);

								//stringstream ssStream;
								//ssStream << "title simdisk [已登录：" << cur_user_Id << "(";
								//if (cur_user_tType == USER)
								//	ssStream << "普通用户";
								//else
								//	ssStream << "管理员";
								//ssStream << ")]";
								//ssStream.clear();
								//system(ssStream.str().c_str());
								success = true;
								WaitForSingleObject(succeedSema, INFINITE);
								succeed = true;
								ReleaseSemaphore(succeedSema, 1, NULL);
								break;
								/*return true;*/
							}
						}
						if (!success)
						{
							cout << "\n\n******************************" << endl;
							sstream_toshell_zero << "\n\n******************************\n";
							cout << "帐号不存在或密码错误！\n你还有" << nCount << "次重试机会。" << endl;
							sstream_toshell_zero << "帐号不存在或密码错误！\n你还有" << nCount << "次重试机会。\n";
							//system("cls");
							nCount--;
							cout << "模拟Linux文件系统\n请先登录账号\n\n";
							sstream_toshell_zero << "模拟Linux文件系统\n请先登录账号\n\n";
							//输入帐号
							cout << "账号：";
							sstream_toshell_zero << "账号：";
							proc++;
						}
					}

					process++;
					ReleaseSemaphore(m_Write[1], 1, NULL);
					is_Write_turn = true;
					WakeConditionVariable(&condition_variable);
					LeaveCriticalSection(&critical_section);
					break;
				}
				else
				{
					error(WRONG_PWD);
					return 0;
				}
			}
			}
			if (success)
			{
				//cout << "读线程进入睡眠！期望写线程进入下一个环节！" << endl;
				//Sleep(10000000);
				break;
			}
		}
		else
		{
			if (nCountForDirectLog >= 0)
			{
				EnterCriticalSection(&critical_section);
				// 如果不是写数据的轮次，则等待
				while (is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				//3.从共享内存中读取数据
				WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]信号量由程序B增加
				char getInfo[BUF_SIZE] = { 0 };		// 新增
				strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);

				if (proc % 2 == 1)
				{
					//cin_id()
					stringstream ss;
					ss << getInfo;
					int nlen = ss.str().length();
					strcpy_s(nUid, MAX_ID_LENGTH, ss.str().c_str());
					nUid[nlen] = '\0';
					cout << endl;
					sstream_toshell_zero << '\n';
					//输入密码，*处理
					cout << "密码：";
					sstream_toshell_zero << "密码：";
					proc++;
				}
				else
				{
					//cin_pwd()
					stringstream ss;
					ss << getInfo;
					int nlen = ss.str().length();
					strcpy_s(strPwd, MAX_ID_LENGTH, ss.str().c_str());

					for (unsigned int i = 0; i < cur_Idsum; i++)
					{
						// 比较密码是否正确
						//cout << systemUsers[i].get_user_id() << endl;
						//cout << systemUsers[i].UserPWD << endl;
						if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
							&& systemUsers[i].cmp_pwd(strPwd))
						{
							//未进行多用户并发的cur_user_Id!!!
							memset(cur_user_Id, 0, MAX_ID_LENGTH);
							strcpy_s(cur_user_Id, 10, nUid);
							cur_user_tType = systemUsers[i].get_user_type();
							cout << "\n\n******************************\n";
							sstream_toshell_zero << "\n\n******************************\n";
							cout << "登录成功！请稍候……\n";
							sstream_toshell_zero << "登录成功！请稍候……\n";
							Sleep(800);

							WaitForSingleObject(tuSema, INFINITE);
							strcpy_s(ThreadID_UserID[0].userID, cur_user_Id);
							ReleaseSemaphore(tuSema, 1, NULL);

							success = true;
							WaitForSingleObject(succeedSema, INFINITE);
							succeed = true;
							ReleaseSemaphore(succeedSema, 1, NULL);
							break;
							/*return true;*/
						}
					}
					if (!success)
					{
						cout << "\n\n******************************" << endl;
						sstream_toshell_zero << "\n\n******************************\n";
						cout << "帐号不存在或密码错误！\n你还有" << nCountForDirectLog << "次重试机会。" << endl;
						sstream_toshell_zero << "帐号不存在或密码错误！\n你还有" << nCountForDirectLog << "次重试机会。\n";
						//system("cls");
						nCountForDirectLog--;
						cout << "模拟Linux文件系统\n请先登录账号\n\n";
						sstream_toshell_zero << "模拟Linux文件系统\n请先登录账号\n\n";
						//输入帐号
						cout << "账号：";
						sstream_toshell_zero << "账号：";
						proc++;
					}
				}

				ReleaseSemaphore(m_Write[1], 1, NULL);
				is_Write_turn = true;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				if (success)break;
			}
			else
			{
				error(WRONG_PWD);
				return 0;
			}
		}
	}
	while (true)
	{
		EnterCriticalSection(&critical_section);
		// 如果不是写数据的轮次，则等待
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}

		//3.从共享内存中读取数据
		WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]信号量由程序B增加
		//cout << "Have got m_Read[1]!" << endl;
		char getInfo[BUF_SIZE] = { 0 };		// 新增
		strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);
		cout << "\n从Shell1读到的信息是：" << getInfo << endl;
		SendInfoToSimdisk(getInfo);
		ReleaseSemaphore(m_Write[1], 1, NULL);
		is_Write_turn = true;
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
	}
	return true;
}
void SendInfoToSimdisk(char getInfo[BUF_SIZE])
{
	//将信息写入sstream_todisk_zero流
	WaitForSingleObject(mutexSim_zero[1], INFINITE);
	sstream_todisk_zero.str(string());
	sstream_todisk_zero.clear();
	sstream_todisk_zero << getInfo;
	ReleaseSemaphore(mutexSim_zero[1], 1, NULL);

	//写入请求队列
	QueuedNum tmp;
	tmp.Requester = 0;

	//读取命令
	stringstream stmp;
	stmp.str(getInfo);
	char cmd[512], strArgv1[128];
	stmp >> cmd >> strArgv1;

	//根据命令类型判断需要请求几次读写循环，并设置对应的隐藏目录状态位与运行步数状态位
	if (strlen(cmd) != 0)
	{
		int unsigned i;
		for (i = 0; i < COMMAND_COUNT; i++) {
			//cout << "\n-------------------\n要比较的指令是：" << commondName << "\t和\t" << strCommands[i] << endl;
			if (strcmp(cmd, strCommands[i]) == 0)
				break;
		}
		if (strcmp(strArgv1, "/?") == 0) {
			tmp.ReqCount = 1;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if (i == 5 || i == 7 || i == 15)
		{
			tmp.ReqCount = 2;
			hideDir_zero = true;
			twoStep_zero = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if (i == 14)
		{
			tmp.ReqCount = 3;
			hideDir_zero = true;
			hideDir_twice_zero = true;
			twoStep_zero = true;
			threeStep_zero = true;
			fourStep_zero = true;
			fiveStep_zero = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if (i < COMMAND_COUNT)
		{
			tmp.ReqCount = 1;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if (!twoStep_zero && !threeStep_zero && !fourStep_zero && !fiveStep_zero)
		{
			tmp.ReqCount = 1;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else
		{
			if (twoStep_zero)
				twoStep_zero = false;
			else if (threeStep_zero)
				threeStep_zero = false;
			else if (fourStep_zero)
				fourStep_zero = false;
			else if (fiveStep_zero)
				fiveStep_zero = false;
		}
		if (i == COMMAND_COUNT - 1)
		{
			WaitForSingleObject(exitSema, INFINITE);
			curShellCount--;
			ReleaseSemaphore(exitSema, 1, NULL);
		}
	}
	else
	{
		WaitForSingleObject(mutexSim_zero[1], INFINITE);
		sstream_todisk_zero.str(string());
		sstream_todisk_zero.clear();
		sstream_todisk_zero << (int)0;
		ReleaseSemaphore(mutexSim_zero[1], 1, NULL);
		tmp.ReqCount = 1;
		WaitForSingleObject(reqSema, INFINITE);
		RequestQueue.push(tmp);
		ReleaseSemaphore(reqSema, 1, NULL);
	}

	//等待Dispatcher放行
	WaitForSingleObject(mutexSim_zero[2], INFINITE);
	WaitForSingleObject(mutexSim[2], INFINITE);

	//将存储的用户信息写入Simdisk主线程的当前用户变量中
	WaitForSingleObject(tuSema, INFINITE);
	cur_user_tType = ThreadID_UserID[0].user_tType;
	cur_dir = ThreadID_UserID[0].user_dir;
	strcpy_s(cur_user_Id, ThreadID_UserID[0].userID);
	strcpy_s(cur_dirName, ThreadID_UserID[0].cur_dirN);
	ReleaseSemaphore(tuSema, 1, NULL);

	//将信息发送给Simdisk主线程
	sstream_todisk.str(string());
	sstream_todisk.clear();
	sstream_todisk << getInfo;

	ReleaseSemaphore(mutexSim[0], 1, NULL);
}

string GetInfoFromSimdisk()
{
	if (CountSim_zero != 0)
	{
		WaitForSingleObject(mutexSim[1], INFINITE);

		//用当前用户信息更新存储的用户信息
		WaitForSingleObject(tuSema, INFINITE);
		strcpy_s(ThreadID_UserID[0].userID, cur_user_Id);
		ThreadID_UserID[0].user_dir = cur_dir;
		strcpy_s(ThreadID_UserID[0].cur_dirN, cur_dirName);
		ThreadID_UserID[0].user_tType = cur_user_tType;
		ReleaseSemaphore(tuSema, 1, NULL);

		if (!hideDir_zero)
		{
			if (!hideDir_twice_zero)
			{
				sstream_toshell << '\n' << cur_dirName << '>';
			}
			else
				hideDir_twice_zero = false;
		}
		else
			hideDir_zero = false;

		//写入请求队列
		QueuedNum tmp;
		tmp.Requester = 0;
		if (logfail)	//如果登录失败
		{
			tmp.ReqCount = 2;
			hideDir_zero = true;
			twoStep_zero = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
			logfail = false;
		}

		//写入sstream_toshell_zero流
		sstream_toshell_zero.str(string());
		sstream_toshell_zero.clear();
		sstream_toshell_zero.str(sstream_toshell.str());
		sstream_toshell.str(string());
		sstream_toshell.clear();

		ReleaseSemaphore(mutexSim[2], 1, NULL);
	}
	else
	{
		WaitForSingleObject(tuSema, INFINITE);
		strcpy_s(ThreadID_UserID[0].userID, cur_user_Id);
		ThreadID_UserID[0].user_tType = cur_user_tType;
		ThreadID_UserID[0].user_dir = cur_dir;
		strcpy_s(ThreadID_UserID[0].cur_dirN, cur_dirName);
		ReleaseSemaphore(tuSema, 1, NULL);
		sstream_toshell_zero << '\n' << cur_dirName << '>';
		CountSim_zero = 1;
	}
	string msg = sstream_toshell_zero.str();
	sstream_toshell_zero.str(string());
	sstream_toshell_zero.clear();
	return msg;
}

unsigned int __stdcall WriteSharedData_portal(void* pPM)
{
	while (true)
	{
		EnterCriticalSection(&critical_section_portal);
		// 如果不是写数据的轮次，则等待
		while (!is_Write_turn_portal)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}

		//3.将数据放到共享内存
			//通过m_pBuf[0]向共享内存m_hMapFile[0]发送数据
		WaitForSingleObject(m_Write_portal[0], INFINITE);
		char szInfo_portal[BUF_SIZE] = { 0 };	// 新增
		WaitForSingleObject(mutexSim_portal[1], INFINITE);	//等待Manager允许发送
		string message = to_string(userCount);				//发给shell的是从 1 开始的userCount
		ReleaseSemaphore(mutexSim_portal[2], 1, NULL);
		strcpy_s(szInfo_portal, message.c_str());	// 不能函数返回值const char* 否则会出现乱码现象，也不能先转c_str()再+'\n'，这样也会出现乱码
		strcpy_s(m_pBuf_portal[0], BUF_SIZE, szInfo_portal);
		ReleaseSemaphore(m_Read_portal[0], 1, NULL); //m_Read[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read[0], INFINITE);获取到该信号量之后输出m_pBuf[1]内存块的数据

		is_Write_turn_portal = false;
		WakeConditionVariable(&condition_variable_portal);
		LeaveCriticalSection(&critical_section_portal);
	}
	return true;
}
unsigned int __stdcall ReadSharedData_portal(void* pPM)
{
	while (true)
	{
		EnterCriticalSection(&critical_section_portal);
		// 如果不是写数据的轮次，则等待
		while (is_Write_turn_portal)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}
		//3.从共享内存中读取数据
		WaitForSingleObject(m_Read_portal[1], INFINITE); //m_Read[1]信号量由程序B增加
		char getInfo_portal[BUF_SIZE] = { 0 };		// 新增
		strcpy_s(getInfo_portal, BUF_SIZE, m_pBuf_portal[1]);
		// 处理收到的用户识别码！
		SendInfoToManager(m_pBuf_portal[1]);
		ReleaseSemaphore(m_Write_portal[1], 1, NULL);
		is_Write_turn_portal = true;
		WakeConditionVariable(&condition_variable_portal);
		LeaveCriticalSection(&critical_section_portal);
	}
	return true;
}
void SendInfoToManager(char getInfo[BUF_SIZE])
{
	WaitForSingleObject(mutexSim_portal[2], INFINITE);
	sstream_toManager_portal.str(string());
	sstream_toManager_portal.clear();
	sstream_toManager_portal << getInfo;
	ReleaseSemaphore(mutexSim_portal[0], 1, NULL);
}

unsigned int __stdcall WriteSharedData_one(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Write[1], INFINITE);
	int process = 1;
	char ID[MAX_ID_LENGTH];
	unsigned int nCount = 3;
	int tmp = 0;
	bool exitting = false;
	//设置在专用线程1中的登录模块
	while (true)
	{
		if (cur_Idsum == 0)			// 进行管理员账户的注册和初始化（意味着如果有线程在登录认证（特别是第一个管理员账户注册）
			//的时候，其他线程接入都必须要被阻塞等待，或者至少是推迟修改userCount！）
		{
			switch (process) {
			case 1: {				//发送的第一个信息：先读入账户信息
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				WaitForSingleObject(m_Write_one[0], INFINITE);


				cout << "\n\n******************************\n";
				sstream_toshell_one << "\n\n******************************\n";
				cout << "正在初始化用户信息……\n";
				sstream_toshell_one << "正在初始化用户信息……\n";
				cout << "注册管理员账户中……\n";
				sstream_toshell_one << "注册管理员账户中……\n";
				//register_user()
				creatingUser = true;
				cmd_md("user", READ_ONLY);
				creatingUser = false;
				cout << "账号(最长10位)：";
				sstream_toshell_one << "账号(最长10位)：";

				char szInfo_one[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());	// 不能函数返回值const char* 否则会出现乱码现象，也不能先转c_str()再+'\n'，这样也会出现乱码
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//输出完成必须要重置流对象!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;							//让下一次循环进入登录认证的第二个步骤
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read_one[0], INFINITE);获取到该信号量之后输出m_pBuf_one[1]内存块的数据
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}
			case 2: {				//发送的第二个信息：
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				WaitForSingleObject(m_Write_one[0], INFINITE);

				char szInfo_one[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//输出完成必须要重置流对象!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;							//让下一次循环进入登录认证的第二个步骤
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read_one[0], INFINITE);获取到该信号量之后输出m_pBuf_one[1]内存块的数据
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}	//Load()过完了
			default: {				//发送剩余的信息
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				WaitForSingleObject(succeedSema_one, INFINITE);
				if (succeed_one)exitting = true;
				ReleaseSemaphore(succeedSema_one, 1, NULL);
				if (exitting)
				{
					LeaveCriticalSection(&critical_section_one);
					break;
				}

				WaitForSingleObject(m_Write_one[0], INFINITE);

				char szInfo_one[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//输出完成必须要重置流对象!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();
				//exitting = true;

				process++;
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read_one[0], INFINITE);获取到该信号量之后输出m_pBuf_one[1]内存块的数据
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}
			}
			if (exitting)break;
		}
		else
		{
			switch (process)
			{
			case 1:
			{
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}

				//进入cmd_logon()
				cout << "当前还有 " << nCount << " 次机会" << endl;
				cout << "模拟Linux文件系统\n请先登录账号\n\n";
				sstream_toshell_one << "模拟Linux文件系统\n请先登录账号\n\n";
				//输入帐号
				cout << "账号：";
				sstream_toshell_one << "账号：";

				char szInfo_one[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//输出完成必须要重置流对象!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read_one[0], INFINITE);获取到该信号量之后输出m_pBuf_one[1]内存块的数据
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}
			default:
			{
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				WaitForSingleObject(succeedSema_one, INFINITE);
				if (succeed_one)exitting = true;
				ReleaseSemaphore(succeedSema_one, 1, NULL);
				if (exitting)
				{
					LeaveCriticalSection(&critical_section_one);
					break;
				}

				WaitForSingleObject(m_Write_one[0], INFINITE);

				char szInfo_one[BUF_SIZE] = { 0 };
				// 写一个功能函数，从主程序中读取输出
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//输出完成必须要重置流对象!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read_one[0], INFINITE);获取到该信号量之后输出m_pBuf_one[1]内存块的数据
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}
			}
			if (exitting)break;
		}
	}
	int relsimdisk = 0;
	int getCount_one = 1;
	while (true)
	{
		EnterCriticalSection(&critical_section_one);
		// 如果不是写数据的轮次，则等待
		while (!is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}
		//3.将数据放到共享内存
			//通过m_pBuf_one[0]向共享内存m_hMapFile_one[0]发送数据
		WaitForSingleObject(m_Write_one[0], INFINITE);
		char szInfo_one[BUF_SIZE] = { 0 };	// 新增
		// 写一个功能函数，从主程序中读取输出
		if (relsimdisk == 0 && succeed == false)
		{
			ReleaseSemaphore(m_ThreadControl_Simdisk, 1, NULL);		//释放信号量让Simdisk主线程运行
			relsimdisk = 1;
		}
		string message = GetInfoFromSimdisk_one();
		//cout << "\n-------------------------\nMessage to Send is:\n" << message << endl;
		//cout << "Length of Message is-----------" << message.length() << endl;
		strcpy_s(szInfo_one, (message).c_str());	// 不能函数返回值const char* 否则会出现乱码现象，也不能先转c_str()再+'\n'，这样也会出现乱码
		strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);
		ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]增加一个信号量，由程序A中的WaitForSingleObject(m_Read_one[0], INFINITE);获取到该信号量之后输出m_pBuf_one[1]内存块的数据

		is_Write_turn_one = false;
		WakeConditionVariable(&condition_variable_one);
		LeaveCriticalSection(&critical_section_one);
		if (getCount_one != 1)
		{
			ReleaseSemaphore(DoneSema_one, 1, NULL);
		}
		else
			getCount_one = 0;
	}
	return true;
}
unsigned int __stdcall ReadSharedData_one(void* pPM)
{
	WaitForSingleObject(m_ThreadControl_Read[1], INFINITE);
	char ID[MAX_ID_LENGTH];
	char password[MAX_PWD_LENGTH];
	char nUid[MAX_ID_LENGTH];
	char strPwd[MAX_PWD_LENGTH];
	int process = 1;
	int nCount = 3;
	int proc = 1;//控制是输账号还是密码
	bool success = false;
	int nCountForDirectLog = 2;
	//设置在专用线程1中的登录模块
	while (true)
	{
		if (cur_Idsum == 0)
		{
			switch (process)
			{
			case 1:
			{
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				//3.从共享内存中读取数据
				WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]信号量由程序B增加

				char getInfo_one[BUF_SIZE] = { 0 };		// 新增
				strcpy_s(getInfo_one, BUF_SIZE, m_pBuf_one[1]);
				//cin_id()
				stringstream ss;
				ss << getInfo_one;
				int nlen = ss.str().length();
				strcpy_s(ID, MAX_ID_LENGTH, ss.str().c_str());
				ID[nlen] = '\0';
				cout << endl;
				sstream_toshell_one << '\n';
				//cmd_useradd()
				Thread_useradd_p2(ID, ADMIN);

				process++;
				ReleaseSemaphore(m_Write_one[1], 1, NULL);
				is_Write_turn_one = true;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}
			case 2:
			{
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				//3.从共享内存中读取数据
				WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]信号量由程序B增加
				char getInfo_one[BUF_SIZE] = { 0 };		// 新增
				strcpy_s(getInfo_one, BUF_SIZE, m_pBuf_one[1]);

				//cin_pwd()
				stringstream ss;
				ss << getInfo_one;
				int nlen = MAX_PWD_LENGTH;
				strcpy_s(password, MAX_PWD_LENGTH, ss.str().c_str());
				stringstream ssStream;
				ssStream << password << " " << (int)ADMIN;
				char strBuffer[MAX_PWD_LENGTH + 5];//建立字符串缓冲区来接受用户输入的密码
				strcpy_s(strBuffer, 25, ssStream.str().c_str());
				ssStream.clear();
				//格式化完成，写入文件
				Dir dirUsr;
				analyse_path("/user", dirUsr);
				char* tem = new char[MAX_NAME_LENGTH];
				strcpy_s(tem, 10, ID);
				save_file(tem, strBuffer, MAX_PWD_LENGTH + 5, dirUsr, PROTECTED);
				//添加一个用户到内存
				cur_Idsum++;
				systemUsers[cur_Idsum - 1].set_user_info(ID, password, ADMIN);
				//读取用户信息
				get_user_info();
				memset(cur_user_Id, 0, MAX_ID_LENGTH);
				strcpy_s(cur_user_Id, 10, systemUsers[0].get_user_id());	//因为是注册的第一个用户所以放到[0]号位置
				cur_user_tType = systemUsers[0].get_user_type();
				//初始化完成，返回
				cout << "\n******************************\n";
				sstream_toshell_one << "\n******************************\n";
				cout << "初始化完成！正在转入登录界面……\n";
				sstream_toshell_one << "初始化完成！正在转入登录界面……\n";
				cout << "**********************磁盘初始化完成**********************" << endl;
				sstream_toshell_one << "**********************磁盘初始化完成**********************\n";
				get_user_info();

				//进入cmd_logon()
				nCount--;
				cout << "模拟Linux文件系统\n请先登录账号\n\n";
				sstream_toshell_one << "模拟Linux文件系统\n请先登录账号\n\n";
				//输入帐号
				cout << "账号：";
				sstream_toshell_one << "账号：";

				process++;
				ReleaseSemaphore(m_Write_one[1], 1, NULL);
				is_Write_turn_one = true;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}		//Load()过完了
			default:
			{
				if (nCount >= 0)
				{
					EnterCriticalSection(&critical_section_one);
					// 如果不是写数据的轮次，则等待
					while (is_Write_turn_one)
					{
						SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
					}
					//3.从共享内存中读取数据
					WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]信号量由程序B增加
					char getInfo_one[BUF_SIZE] = { 0 };		// 新增
					strcpy_s(getInfo_one, BUF_SIZE, m_pBuf_one[1]);

					if (proc % 2 == 1)
					{
						//cin_id()
						stringstream ss;
						ss << getInfo_one;
						int nlen = ss.str().length();
						strcpy_s(nUid, MAX_ID_LENGTH, ss.str().c_str());
						nUid[nlen] = '\0';
						cout << endl;
						sstream_toshell_one << '\n';
						//输入密码，*处理
						cout << "密码：";
						sstream_toshell_one << "密码：";
						proc++;
					}
					else
					{
						//cin_pwd()
						stringstream ss;
						ss << getInfo_one;
						int nlen = ss.str().length();
						strcpy_s(strPwd, MAX_ID_LENGTH, ss.str().c_str());

						for (unsigned int i = 0; i < cur_Idsum; i++)
						{
							// 比较密码是否正确
							//cout << systemUsers[i].get_user_id() << endl;
							//cout << systemUsers[i].UserPWD << endl;
							if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
								&& systemUsers[i].cmp_pwd(strPwd))
							{
								//未进行多用户并发的cur_user_Id!!!
								memset(cur_user_Id, 0, MAX_ID_LENGTH);
								strcpy_s(cur_user_Id, 10, nUid);
								cur_user_tType = systemUsers[i].get_user_type();
								cout << "\n\n******************************\n";
								sstream_toshell_one << "\n\n******************************\n";
								cout << "登录成功！请稍候……\n";
								sstream_toshell_one << "登录成功！请稍候……\n";
								Sleep(800);

								WaitForSingleObject(tuSema, INFINITE);
								strcpy_s(ThreadID_UserID[1].userID, cur_user_Id);
								ReleaseSemaphore(tuSema, 1, NULL);

								//stringstream ssStream;
								//ssStream << "title simdisk [已登录：" << cur_user_Id << "(";
								//if (cur_user_tType == USER)
								//	ssStream << "普通用户";
								//else
								//	ssStream << "管理员";
								//ssStream << ")]";
								//ssStream.clear();
								//system(ssStream.str().c_str());
								success = true;
								WaitForSingleObject(succeedSema_one, INFINITE);
								succeed_one = true;
								ReleaseSemaphore(succeedSema_one, 1, NULL);
								break;
								/*return true;*/
							}
						}
						if (!success)
						{
							cout << "\n\n******************************" << endl;
							sstream_toshell_one << "\n\n******************************\n";
							cout << "帐号不存在或密码错误！\n你还有" << nCount << "次重试机会。" << endl;
							sstream_toshell_one << "帐号不存在或密码错误！\n你还有" << nCount << "次重试机会。\n";
							//system("cls");
							nCount--;
							cout << "模拟Linux文件系统\n请先登录账号\n\n";
							sstream_toshell_one << "模拟Linux文件系统\n请先登录账号\n\n";
							//输入帐号
							cout << "账号：";
							sstream_toshell_one << "账号：";
							proc++;
						}
					}

					process++;
					ReleaseSemaphore(m_Write_one[1], 1, NULL);
					is_Write_turn_one = true;
					WakeConditionVariable(&condition_variable_one);
					LeaveCriticalSection(&critical_section_one);
					break;
				}
				else
				{
					error(WRONG_PWD);
					return 0;
				}
			}
			}
			if (success)
			{
				break;
			}
		}
		else
		{
			if (nCountForDirectLog >= 0)
			{
				EnterCriticalSection(&critical_section_one);
				// 如果不是写数据的轮次，则等待
				while (is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				//3.从共享内存中读取数据
				WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]信号量由程序B增加
				char getInfo_one[BUF_SIZE] = { 0 };		// 新增
				strcpy_s(getInfo_one, BUF_SIZE, m_pBuf_one[1]);

				if (proc % 2 == 1)
				{
					//cin_id()
					stringstream ss;
					ss << getInfo_one;
					int nlen = ss.str().length();
					strcpy_s(nUid, MAX_ID_LENGTH, ss.str().c_str());
					nUid[nlen] = '\0';
					cout << endl;
					sstream_toshell_one << '\n';
					//输入密码，*处理
					cout << "密码：";
					sstream_toshell_one << "密码：";
					proc++;
				}
				else
				{
					//cin_pwd()
					stringstream ss;
					ss << getInfo_one;
					int nlen = ss.str().length();
					strcpy_s(strPwd, MAX_ID_LENGTH, ss.str().c_str());

					for (unsigned int i = 0; i < cur_Idsum; i++)
					{
						// 比较密码是否正确
						//cout << systemUsers[i].get_user_id() << endl;
						//cout << systemUsers[i].UserPWD << endl;
						if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
							&& systemUsers[i].cmp_pwd(strPwd))
						{
							//未进行多用户并发的cur_user_Id!!!
							memset(cur_user_Id, 0, MAX_ID_LENGTH);
							strcpy_s(cur_user_Id, 10, nUid);
							cur_user_tType = systemUsers[i].get_user_type();
							cout << "\n\n******************************\n";
							sstream_toshell_one << "\n\n******************************\n";
							cout << "登录成功！请稍候……\n";
							sstream_toshell_one << "登录成功！请稍候……\n";
							Sleep(800);

							WaitForSingleObject(tuSema, INFINITE);
							strcpy_s(ThreadID_UserID[1].userID, cur_user_Id);
							ReleaseSemaphore(tuSema, 1, NULL);

							success = true;
							WaitForSingleObject(succeedSema_one, INFINITE);
							succeed_one = true;
							ReleaseSemaphore(succeedSema_one, 1, NULL);
							break;
							/*return true;*/
						}
					}
					if (!success)
					{
						cout << "\n\n******************************" << endl;
						sstream_toshell_one << "\n\n******************************\n";
						cout << "帐号不存在或密码错误！\n你还有" << nCountForDirectLog << "次重试机会。" << endl;
						sstream_toshell_one << "帐号不存在或密码错误！\n你还有" << nCountForDirectLog << "次重试机会。\n";
						//system("cls");
						nCountForDirectLog--;
						cout << "模拟Linux文件系统\n请先登录账号\n\n";
						sstream_toshell_one << "模拟Linux文件系统\n请先登录账号\n\n";
						//输入帐号
						cout << "账号：";
						sstream_toshell_one << "账号：";
						proc++;
					}
				}

				ReleaseSemaphore(m_Write_one[1], 1, NULL);
				is_Write_turn_one = true;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				if (success)break;
			}
			else
			{
				error(WRONG_PWD);
				return 0;
			}
		}
	}
	//WaitForSingleObject(, INFINITE);
	while (true)
	{
		EnterCriticalSection(&critical_section_one);
		// 如果不是写数据的轮次，则等待
		while (is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}

		//3.从共享内存中读取数据
		WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]信号量由程序B增加
		char getInfo_one[BUF_SIZE] = { 0 };		// 新增
		strcpy_s(getInfo_one, BUF_SIZE, m_pBuf_one[1]);
		cout << "\n从Shell2读到的信息是：" << getInfo_one << endl;
		SendInfoToSimdisk_one(getInfo_one);
		ReleaseSemaphore(m_Write_one[1], 1, NULL);
		is_Write_turn_one = true;
		WakeConditionVariable(&condition_variable_one);
		LeaveCriticalSection(&critical_section_one);
	}
	return true;
}

void SendInfoToSimdisk_one(char getInfo[BUF_SIZE])
{
	//将信息写入sstream_todisk_one流
	WaitForSingleObject(mutexSim_one[1], INFINITE);
	sstream_todisk_one.str(string());
	sstream_todisk_one.clear();
	sstream_todisk_one << getInfo;
	ReleaseSemaphore(mutexSim_one[1], 1, NULL);

	//写入请求队列
	QueuedNum tmp;
	tmp.Requester = 1;

	//读取命令
	stringstream stmp;
	stmp.str(getInfo);
	char cmd[512], strArgv1[128];
	stmp >> cmd >> strArgv1;

	//根据命令类型判断需要请求几次读写循环，并设置对应的隐藏目录状态位与运行步数状态位
	if (strlen(cmd) != 0)
	{
		int unsigned i;
		for (i = 0; i < COMMAND_COUNT; i++) {
			if (strcmp(cmd, strCommands[i]) == 0)
				break;
		}
		if (strcmp(strArgv1, "/?") == 0) {
			tmp.ReqCount = 1;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if (i == 5 || i == 7 || i == 15)
		{
			tmp.ReqCount = 2;
			hideDir_one = true;
			twoStep_one = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if (i == 14)
		{
			tmp.ReqCount = 3;
			hideDir_one = true;
			hideDir_twice_one = true;
			twoStep_one = true;
			threeStep_one = true;
			fourStep_one = true;
			fiveStep_one = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if(i < COMMAND_COUNT)
		{
			tmp.ReqCount = 1;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else if (!twoStep_one && !threeStep_one && !fourStep_one && !fiveStep_one)
		{
			tmp.ReqCount = 1;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
		}
		else
		{
			if (twoStep_one)
				twoStep_one = false;
			else if (threeStep_one)
				threeStep_one = false;
			else if (fourStep_one)
				fourStep_one = false;
			else if (fiveStep_one)
				fiveStep_one = false;
		}
		if (i == COMMAND_COUNT - 1)
		{
			WaitForSingleObject(exitSema, INFINITE);
			curShellCount--;
			ReleaseSemaphore(exitSema, 1, NULL);
		}
	}
	else
	{
		WaitForSingleObject(mutexSim_one[1], INFINITE);
		sstream_todisk_one.str(string());
		sstream_todisk_one.clear();
		sstream_todisk_one << (int)0;
		ReleaseSemaphore(mutexSim_one[1], 1, NULL);
		tmp.ReqCount = 1;
		WaitForSingleObject(reqSema, INFINITE);
		RequestQueue.push(tmp);
		ReleaseSemaphore(reqSema, 1, NULL);
	}

	//等待Dispatcher放行
	WaitForSingleObject(mutexSim_one[2], INFINITE);
	WaitForSingleObject(mutexSim[2], INFINITE);

	//将存储的用户信息写入Simdisk主线程的当前用户变量中
	WaitForSingleObject(tuSema, INFINITE);
	strcpy_s(cur_user_Id, ThreadID_UserID[1].userID);
	strcpy_s(cur_dirName, ThreadID_UserID[1].cur_dirN);
	cur_dir = ThreadID_UserID[1].user_dir;
	cur_user_tType = ThreadID_UserID[1].user_tType;
	ReleaseSemaphore(tuSema, 1, NULL);

	//将信息发送给Simdisk主线程
	sstream_todisk.str(string());
	sstream_todisk.clear();
	sstream_todisk << getInfo;

	ReleaseSemaphore(mutexSim[0], 1, NULL);
}
string GetInfoFromSimdisk_one()
{
	if (CountSim_one != 0)
	{
		WaitForSingleObject(mutexSim[1], INFINITE);

		//用当前用户信息更新存储的用户信息
		WaitForSingleObject(tuSema, INFINITE);
		strcpy_s(ThreadID_UserID[1].cur_dirN, cur_dirName);
		ThreadID_UserID[1].user_dir = cur_dir;
		strcpy_s(ThreadID_UserID[1].userID, cur_user_Id);
		ThreadID_UserID[1].user_tType = cur_user_tType;
		ReleaseSemaphore(tuSema, 1, NULL);
		if (!hideDir_one)
		{
			if (!hideDir_twice_one)
			{
				sstream_toshell << '\n' << cur_dirName << '>';
			}
			else
				hideDir_twice_one = false;
		}
		else
			hideDir_one = false;

		//写入请求队列
		QueuedNum tmp;
		tmp.Requester = 1;
		if (logfail)	//如果登陆失败
		{
			tmp.ReqCount = 2;
			hideDir_one = true;
			twoStep_one = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
			logfail = false;
		}

		//写入sstream_toshell_one流
		sstream_toshell_one.str(string());
		sstream_toshell_one.clear();
		sstream_toshell_one.str(sstream_toshell.str());
		sstream_toshell.str(string());
		sstream_toshell.clear();

		ReleaseSemaphore(mutexSim[2], 1, NULL);
	}
	else
	{
		CountSim_one = 1;
		sstream_toshell_one << '\n' << cur_dirName << '>';
		WaitForSingleObject(tuSema, INFINITE);
		ThreadID_UserID[1].user_tType = cur_user_tType;
		ThreadID_UserID[1].user_dir = cur_dir;
		strcpy_s(ThreadID_UserID[1].cur_dirN, cur_dirName);
		ReleaseSemaphore(tuSema, 1, NULL);
	}
	string msg = sstream_toshell_one.str();
	sstream_toshell_one.str(string());
	sstream_toshell_one.clear();
	return msg;
}

void Thread_useradd_p2(const char* strUserId, user_type user_tType)
{
	//如果当前用户类型不是系统或者管理员则报错没有权限创建账户
	if (cur_user_tType != SYSTEM && cur_user_tType != ADMIN) {
		error(NO_PRIVILEGE, "user");
		return;
	}
	if (cur_Idsum < MAX_USER_NUM)	//没有超过限制
	{
		//查找是否存在同名账号
		for (unsigned int i = 0; i < cur_Idsum; i++)
		{
			//如果账号同名则报错用户已存在
			if (strcmp(systemUsers[i].get_user_id(), strUserId) == 0)
			{
				error(USER_EXIST, strUserId);
				return;
			}
		}
		//初始化
		cout << "密码(最长20位)：";
		sstream_toshell_one << "密码(最长20位)：";
	}
	else
	{
		error(USERS_MAX);
	}
}