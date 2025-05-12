/*
simdisk.cpp
ִ�г���
*/
#include"simdisk.h"
#include"func.h"
#include<sstream>

#include"ipc.h"

stringstream sstream_toshell;
stringstream sstream_todisk;
wstring   m_cstrSzName[2];//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
wstring   m_ReadSemaphoreName[2];//�����ݵ��ź�����
wstring   m_WriteSemaphoreName[2];//д���ݵ��ź�����
wstring   m_ReadBeforeWrite;//����д���ź�����
HANDLE  m_hMapFile[2];//�����ڴ���
char* m_pBuf[2];//��ȡ���ݵĻ���
HANDLE	m_Read[2];//�����ݵ��ź������
HANDLE  m_Write[2];//д���ݵ��ź������
HANDLE  readBeforeWrite;//����д���ź������

bool is_Write_turn = true;//���õ�ǰ�Ƿ�ΪWriteSharedData�߳����л��ڵĲ�����
CRITICAL_SECTION critical_section;//ReadSharedData��WriteSharedData֮����ٽ���
CONDITION_VARIABLE condition_variable;//ReadSharedData��WriteSharedData֮�����������

HANDLE mutexSim[3];

//HANDLE mutexSimdisk;

#define BUF_SIZE 1024*1024*100+1
char input[] = "input";    // �����ڴ������
char output[] = "output";    // �����ڴ������


char cur_user_Id[MAX_ID_LENGTH] = "000000";		//��ǰ�û��˺�ID
user_type cur_user_tType = SYSTEM;					//��ǰ�û�����
unsigned int cur_Idsum = 0;						//��ǰ�û�����

char cur_dirName[MAX_NAME_LENGTH];				//��ǰĿ¼����
Dir cur_dir;									//��ǰĿ¼

fstream fs;									//����Linux�����ļ�
User systemUsers[MAX_USER_NUM];					//ϵͳ�û�����

SuperBlock sbGroups[BLOCK_GROUPS_NUM];			//���ݿ��飨�������飩
Inode indTable[INODES_NUM];						//i-����
bmp_status bsBlockBmp[BLOCKS_NUM];				//���ݿ�λͼ����
bmp_status bsInodeBmp[INODES_NUM];				//i-���λͼ����

//������
int main(int argc, char* argv[]) {
	//Sleep(5000);//�ȴ�������Shell
	CreateFileMap();//���������ڴ�
	UINT threadId;
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData, 0, 0, &threadId);//���������ݵ��߳�
	if (hThread2 == NULL)
	{
		cout << "Starting ReadSharedData Thread  Failed!" << endl;
	}
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData, 0, 0, &threadId);//����д���ݵ��߳�
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
		//sstreamδ������󣬼���������
		if (!ss.eof()) {
			error(TOO_MANY_ARGS);
			ss.str("");//�������
			ss.clear();//����sstream�ı�־״̬
			continue;
		}
		//���ݽ��ղ������ú���
		ss.clear();
		cmd_execute(cmd, strArgv1, strArgv2);
	}
	 CloseFileMap();
}
