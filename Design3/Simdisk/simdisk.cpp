/*
simdisk.cpp
执行程序
*/
#include"simdisk.h"
#include"func.h"
#include<sstream>
#include"ipc.h"

#define BUF_SIZE 1024

//Manager用来控制线程阻塞与运行的信号量数组
HANDLE m_ThreadControl_Read[2];				//用于专用读线程运行控制的信号量句柄组
HANDLE m_ThreadControl_Write[2];			//用于专用写线程运行控制的信号量句柄组
HANDLE m_ThreadControl_Simdisk;				//用于Simdisk主线程运行控制的信号量句柄组
int userCount;								//当前连接用户数
vector<UserInfo> ThreadID_UserID;
bool createError;							//创建错误状态位
HANDLE errorSema;							//保证互斥访问错误状态位的信号量句柄
int curShellCount;							//当前连接上Simdisk的Shell程序个数
HANDLE exitSema;							//让exit命令生效的信号量句柄

HANDLE tuSema;								//对ThreadID_UserID的读写访问保护信号量句柄
bool creatingDisk = true;					//md命令使用的创盘状态位
bool creatingUser;							//md命令使用的创建用户状态位
HANDLE hDSema;								//保证互斥访问隐藏目录名状态位的信号量句柄
int curShellNum;							//当前连接上Simdisk的Shell数量
bool hideDir_zero = false;					//隐藏目录名状态位（专用线程0）
bool hideDir_one = false;					//隐藏目录名状态位（专用线程1）
bool logfail = false;						//登陆失败状态位

HANDLE mutexSim_zero[3];

stringstream sstream_toshell;					//Simdisk主线程实际操作的发送给Shell程序的字符串流
stringstream sstream_todisk;					///Simdisk主线程实际操作的发送给Simdisk主线程的字符串流

stringstream sstream_toshell_zero;				//专用线程0发送给Shell程序的字符串流
stringstream sstream_todisk_zero;				//专用线程0发送给Simdisk主线程的字符串流
wstring   m_cstrSzName[2];						//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName[2];				//读数据的信号量名
wstring   m_WriteSemaphoreName[2];				//写数据的信号量名
wstring   m_ReadBeforeWrite;					//读后写的信号量名
HANDLE  m_hMapFile[2];							//共享内存句柄
char* m_pBuf[2];								//读取数据的缓存
HANDLE	m_Read[2];								//读数据的信号量句柄
HANDLE  m_Write[2];								//写数据的信号量句柄
CRITICAL_SECTION critical_section;				//ReadSharedData和WriteSharedData之间的临界区
bool is_Write_turn;								//设置当前是否为WriteSharedData线程运行环节的布尔量
CONDITION_VARIABLE condition_variable;			//ReadSharedData和WriteSharedData之间的条件变量
HANDLE mutexSim[3];

stringstream sstream_toshell_portal;			//公共读写线程发送给Shell程序的字符串流
stringstream sstream_toManager_portal;			//公共读写线程发送给Manager线程的字符串流
wstring   m_cstrSzName_portal[2];				//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName_portal[2];		//读数据的信号量名
wstring   m_WriteSemaphoreName_portal[2];		//写数据的信号量名
HANDLE  m_hMapFile_portal[2];					//共享内存句柄
char* m_pBuf_portal[2];							//读取数据缓存
char szInfo_portal[BUF_SIZE];					//先存储系统返回信息，后再传入m_pBuf[0]
char getInfo_portal[BUF_SIZE];
HANDLE	m_Read_portal[2];						//读数据的信号量句柄
HANDLE  m_Write_portal[2];						//写数据的信号量句柄
//用于两个线程间的同步
CRITICAL_SECTION critical_section_portal;		//ReadSharedData_portal和WriteSharedData_portal之间的临界区
bool is_Write_turn_portal;						//设置当前是否为WriteSharedData_portal线程运行环节的布尔量
CONDITION_VARIABLE condition_variable_portal;	//ReadSharedData_portal和WriteSharedData_portal之间的条件变量
//用于ReadSharedData_portal、WriteSharedData_portal与Manager主线程之间的互斥访问
HANDLE mutexSim_portal[3];

stringstream sstream_toshell_one;			//专用线程1发送给Shell程序的字符串流
stringstream sstream_todisk_one;			//专用线程1发送给Simdisk主线程的字符串流
wstring   m_cstrSzName_one[2];				//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName_one[2];		//读数据的信号量名
wstring   m_WriteSemaphoreName_one[2];		//写数据的信号量名
HANDLE  m_hMapFile_one[2];					//共享内存句柄
char* m_pBuf_one[2];						//读取数据缓存
char szInfo_one[BUF_SIZE];					//先存储系统返回信息，后再传入m_pBuf[0]
char getInfo_one[BUF_SIZE];
HANDLE	m_Read_one[2];						//读数据的信号量句柄
HANDLE  m_Write_one[2];						//写数据的信号量句柄
//用于两个线程间的同步
CRITICAL_SECTION critical_section_one;		//ReadSharedData_one和WriteSharedData_one之间的临界区
bool is_Write_turn_one;						//设置当前是否为WriteSharedData_one线程运行环节的布尔量
CONDITION_VARIABLE condition_variable_one;	//ReadSharedData_one和WriteSharedData_one之间的条件变量
//用于两个线程与Simdisk主线程之间的互斥访问
HANDLE mutexSim_one[3];

char cur_user_Id[MAX_ID_LENGTH] = "000000";		//当前用户账号ID
user_type cur_user_tType = SYSTEM;					//当前用户类型
unsigned int cur_Idsum = 0;						//当前用户数量

char cur_dirName[MAX_NAME_LENGTH];				//当前目录名称
Dir cur_dir;									//当前目录

fstream fs;									//虚拟Linux磁盘文件
User systemUsers[MAX_USER_NUM];					//系统用户数组

SuperBlock sbGroups[BLOCK_GROUPS_NUM];			//数据块组（超级块组）
Inode indTable[INODES_NUM];						//i-结点表
bmp_status bsBlockBmp[BLOCKS_NUM];				//数据块位图数组
bmp_status bsInodeBmp[INODES_NUM];				//i-结点位图数组

//主函数
int main(int argc, char* argv[]) {
	HWND hwnd = GetConsoleWindow();
	//ShowWindow(hwnd, SW_HIDE);
	CreateFileMap();//创建共享内存
	ManagerInit();//管理员线程变量初始化

	system("title simdisk");
	load();			//现在load()里面没有向sstream_toshell的输入了，不会再影响到登陆操作了
	creatingDisk = false;
	cout << "正在等待Shell连接.........." << endl;
	UINT threadId;

	HANDLE hThread_Manager = (HANDLE)_beginthreadex(NULL, 0, Manager, 0, 0, &threadId);//创建管理者线程
	if (hThread_Manager == NULL)
	{
		cout << "Starting Manager Thread Failed!" << endl;
	}
	HANDLE hThread_Dispatcher = (HANDLE)_beginthreadex(NULL, 0, Dispatcher, 0, 0, &threadId);//创建管理者线程
	if (hThread_Dispatcher == NULL)
	{
		cout << "Starting Dispatcher Thread Failed!" << endl;
	}

	HANDLE hThread3 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData_portal, 0, 0, &threadId);//创建读数据的线程
	if (hThread3 == NULL)
	{
		cout << "Starting ReadSharedData_portal Thread  Failed!" << endl;
	}
	HANDLE hThread4 = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData_portal, 0, 0, &threadId);//创建写数据的线程
	if (hThread4 == NULL)
	{
		cout << "Starting WriteSharedData_portal Thread  Failed!" << endl;
	}

	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData, 0, 0, &threadId);//创建读数据的线程
	if (hThread2 == NULL)
	{
		cout << "Starting ReadSharedData Thread  Failed!" << endl;
	}
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData, 0, 0, &threadId);//创建写数据的线程
	if (hThread == NULL)
	{
		cout << "Starting WriteSharedData Thread  Failed!" << endl;
	}

	HANDLE hThread5 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData_one, 0, 0, &threadId);//创建读数据的线程
	if (hThread5 == NULL)
	{
		cout << "Starting ReadSharedData_one Thread  Failed!" << endl;
	}
	HANDLE hThread6 = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData_one, 0, 0, &threadId);//创建写数据的线程
	if (hThread6 == NULL)
	{
		cout << "Starting WriteSharedData_one Thread  Failed!" << endl;
	}
	//在没有用户进来的情况下，阻塞下面的创建读写线程和Simdisk主程序的所有语句！！！（在Manager中释放这里的信号量）
	WaitForSingleObject(m_ThreadControl_Simdisk, INFINITE);
	char commandLine[MAX_COMMAND_LENGTH + MAX_PATH_LENGTH * 2 + 2];
	char cmd[MAX_COMMAND_LENGTH], strArgv1[MAX_PATH_LENGTH], strArgv2[MAX_PATH_LENGTH];
	stringstream ss;
	WaitForSingleObject(mutexSim[0], INFINITE);
	while (1) {
		cout << '\n' << cur_dirName << '>';
		//sstream_toshell << '\n' << cur_dirName << '>';
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		ss.str(sstream_todisk.str());
		ss >> cmd >> strArgv1 >> strArgv2;
		mytolower(cmd);
		if (strlen(cmd) == 0) {
			ss.clear();
			continue;
		}
		//sstream未读到最后，即参数过多
		if (!ss.eof()) {
			error(TOO_MANY_ARGS);
			ss.str("");//清空数据
			ss.clear();//重置sstream的标志状态
			continue;
		}
		//根据接收参数调用函数
		ss.clear();
		cmd_execute(cmd, strArgv1, strArgv2);
	}
	CloseFileMap();
}
