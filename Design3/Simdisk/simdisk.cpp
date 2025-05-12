/*
simdisk.cpp
ִ�г���
*/
#include"simdisk.h"
#include"func.h"
#include<sstream>
#include"ipc.h"

#define BUF_SIZE 1024

//Manager���������߳����������е��ź�������
HANDLE m_ThreadControl_Read[2];				//����ר�ö��߳����п��Ƶ��ź��������
HANDLE m_ThreadControl_Write[2];			//����ר��д�߳����п��Ƶ��ź��������
HANDLE m_ThreadControl_Simdisk;				//����Simdisk���߳����п��Ƶ��ź��������
int userCount;								//��ǰ�����û���
vector<UserInfo> ThreadID_UserID;
bool createError;							//��������״̬λ
HANDLE errorSema;							//��֤������ʴ���״̬λ���ź������
int curShellCount;							//��ǰ������Simdisk��Shell�������
HANDLE exitSema;							//��exit������Ч���ź������

HANDLE tuSema;								//��ThreadID_UserID�Ķ�д���ʱ����ź������
bool creatingDisk = true;					//md����ʹ�õĴ���״̬λ
bool creatingUser;							//md����ʹ�õĴ����û�״̬λ
HANDLE hDSema;								//��֤�����������Ŀ¼��״̬λ���ź������
int curShellNum;							//��ǰ������Simdisk��Shell����
bool hideDir_zero = false;					//����Ŀ¼��״̬λ��ר���߳�0��
bool hideDir_one = false;					//����Ŀ¼��״̬λ��ר���߳�1��
bool logfail = false;						//��½ʧ��״̬λ

HANDLE mutexSim_zero[3];

stringstream sstream_toshell;					//Simdisk���߳�ʵ�ʲ����ķ��͸�Shell������ַ�����
stringstream sstream_todisk;					///Simdisk���߳�ʵ�ʲ����ķ��͸�Simdisk���̵߳��ַ�����

stringstream sstream_toshell_zero;				//ר���߳�0���͸�Shell������ַ�����
stringstream sstream_todisk_zero;				//ר���߳�0���͸�Simdisk���̵߳��ַ�����
wstring   m_cstrSzName[2];						//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
wstring   m_ReadSemaphoreName[2];				//�����ݵ��ź�����
wstring   m_WriteSemaphoreName[2];				//д���ݵ��ź�����
wstring   m_ReadBeforeWrite;					//����д���ź�����
HANDLE  m_hMapFile[2];							//�����ڴ���
char* m_pBuf[2];								//��ȡ���ݵĻ���
HANDLE	m_Read[2];								//�����ݵ��ź������
HANDLE  m_Write[2];								//д���ݵ��ź������
CRITICAL_SECTION critical_section;				//ReadSharedData��WriteSharedData֮����ٽ���
bool is_Write_turn;								//���õ�ǰ�Ƿ�ΪWriteSharedData�߳����л��ڵĲ�����
CONDITION_VARIABLE condition_variable;			//ReadSharedData��WriteSharedData֮�����������
HANDLE mutexSim[3];

stringstream sstream_toshell_portal;			//������д�̷߳��͸�Shell������ַ�����
stringstream sstream_toManager_portal;			//������д�̷߳��͸�Manager�̵߳��ַ�����
wstring   m_cstrSzName_portal[2];				//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
wstring   m_ReadSemaphoreName_portal[2];		//�����ݵ��ź�����
wstring   m_WriteSemaphoreName_portal[2];		//д���ݵ��ź�����
HANDLE  m_hMapFile_portal[2];					//�����ڴ���
char* m_pBuf_portal[2];							//��ȡ���ݻ���
char szInfo_portal[BUF_SIZE];					//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
char getInfo_portal[BUF_SIZE];
HANDLE	m_Read_portal[2];						//�����ݵ��ź������
HANDLE  m_Write_portal[2];						//д���ݵ��ź������
//���������̼߳��ͬ��
CRITICAL_SECTION critical_section_portal;		//ReadSharedData_portal��WriteSharedData_portal֮����ٽ���
bool is_Write_turn_portal;						//���õ�ǰ�Ƿ�ΪWriteSharedData_portal�߳����л��ڵĲ�����
CONDITION_VARIABLE condition_variable_portal;	//ReadSharedData_portal��WriteSharedData_portal֮�����������
//����ReadSharedData_portal��WriteSharedData_portal��Manager���߳�֮��Ļ������
HANDLE mutexSim_portal[3];

stringstream sstream_toshell_one;			//ר���߳�1���͸�Shell������ַ�����
stringstream sstream_todisk_one;			//ר���߳�1���͸�Simdisk���̵߳��ַ�����
wstring   m_cstrSzName_one[2];				//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
wstring   m_ReadSemaphoreName_one[2];		//�����ݵ��ź�����
wstring   m_WriteSemaphoreName_one[2];		//д���ݵ��ź�����
HANDLE  m_hMapFile_one[2];					//�����ڴ���
char* m_pBuf_one[2];						//��ȡ���ݻ���
char szInfo_one[BUF_SIZE];					//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
char getInfo_one[BUF_SIZE];
HANDLE	m_Read_one[2];						//�����ݵ��ź������
HANDLE  m_Write_one[2];						//д���ݵ��ź������
//���������̼߳��ͬ��
CRITICAL_SECTION critical_section_one;		//ReadSharedData_one��WriteSharedData_one֮����ٽ���
bool is_Write_turn_one;						//���õ�ǰ�Ƿ�ΪWriteSharedData_one�߳����л��ڵĲ�����
CONDITION_VARIABLE condition_variable_one;	//ReadSharedData_one��WriteSharedData_one֮�����������
//���������߳���Simdisk���߳�֮��Ļ������
HANDLE mutexSim_one[3];

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
	HWND hwnd = GetConsoleWindow();
	//ShowWindow(hwnd, SW_HIDE);
	CreateFileMap();//���������ڴ�
	ManagerInit();//����Ա�̱߳�����ʼ��

	system("title simdisk");
	load();			//����load()����û����sstream_toshell�������ˣ�������Ӱ�쵽��½������
	creatingDisk = false;
	cout << "���ڵȴ�Shell����.........." << endl;
	UINT threadId;

	HANDLE hThread_Manager = (HANDLE)_beginthreadex(NULL, 0, Manager, 0, 0, &threadId);//�����������߳�
	if (hThread_Manager == NULL)
	{
		cout << "Starting Manager Thread Failed!" << endl;
	}
	HANDLE hThread_Dispatcher = (HANDLE)_beginthreadex(NULL, 0, Dispatcher, 0, 0, &threadId);//�����������߳�
	if (hThread_Dispatcher == NULL)
	{
		cout << "Starting Dispatcher Thread Failed!" << endl;
	}

	HANDLE hThread3 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData_portal, 0, 0, &threadId);//���������ݵ��߳�
	if (hThread3 == NULL)
	{
		cout << "Starting ReadSharedData_portal Thread  Failed!" << endl;
	}
	HANDLE hThread4 = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData_portal, 0, 0, &threadId);//����д���ݵ��߳�
	if (hThread4 == NULL)
	{
		cout << "Starting WriteSharedData_portal Thread  Failed!" << endl;
	}

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

	HANDLE hThread5 = (HANDLE)_beginthreadex(NULL, 0, ReadSharedData_one, 0, 0, &threadId);//���������ݵ��߳�
	if (hThread5 == NULL)
	{
		cout << "Starting ReadSharedData_one Thread  Failed!" << endl;
	}
	HANDLE hThread6 = (HANDLE)_beginthreadex(NULL, 0, WriteSharedData_one, 0, 0, &threadId);//����д���ݵ��߳�
	if (hThread6 == NULL)
	{
		cout << "Starting WriteSharedData_one Thread  Failed!" << endl;
	}
	//��û���û�����������£���������Ĵ�����д�̺߳�Simdisk�������������䣡��������Manager���ͷ�������ź�����
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
