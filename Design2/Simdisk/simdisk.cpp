/*
simdisk.cpp
执行程序
*/
#include"simdisk.h"
#include"func.h"
#include<sstream>

#include"ipc.h"

stringstream sstream_toshell;
stringstream sstream_todisk;
wstring   m_cstrSzName[2];//共享内存名（一共两个，一个给A放数据，由B取数据；一个给B放数据，由A取数据）
wstring   m_ReadSemaphoreName[2];//读数据的信号量名
wstring   m_WriteSemaphoreName[2];//写数据的信号量名
wstring   m_ReadBeforeWrite;//读后写的信号量名
HANDLE  m_hMapFile[2];//共享内存句柄
char* m_pBuf[2];//读取数据的缓存
HANDLE	m_Read[2];//读数据的信号量句柄
HANDLE  m_Write[2];//写数据的信号量句柄
HANDLE  readBeforeWrite;//读后写的信号量句柄

bool is_Write_turn = true;//设置当前是否为WriteSharedData线程运行环节的布尔量
CRITICAL_SECTION critical_section;//ReadSharedData和WriteSharedData之间的临界区
CONDITION_VARIABLE condition_variable;//ReadSharedData和WriteSharedData之间的条件变量

HANDLE mutexSim[3];

//HANDLE mutexSimdisk;

#define BUF_SIZE 1024*1024*100+1
char input[] = "input";    // 共享内存的名字
char output[] = "output";    // 共享内存的名字


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
	//Sleep(5000);//等待连接上Shell
	CreateFileMap();//创建共享内存
	UINT threadId;
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
	system("title simdisk");
	WaitForSingleObject(mutexSim[0], INFINITE);
	load();
	if (!cmd_logon())return 0;
	system("cls");
	cin.clear();
	cin.sync();
	char commandLine[MAX_COMMAND_LENGTH + MAX_PATH_LENGTH * 2 + 2];
	char cmd[MAX_COMMAND_LENGTH], strArgv1[MAX_PATH_LENGTH], strArgv2[MAX_PATH_LENGTH];
	stringstream ss;
	while (1) {
		cout << '\n' << cur_dirName << '>';
		sstream_toshell << '\n' << cur_dirName << '>';
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		ss.str(sstream_todisk.str());
		ss >> cmd >> strArgv1 >> strArgv2;
		mytolower(cmd);
		if (strlen(cmd)==0) {
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
