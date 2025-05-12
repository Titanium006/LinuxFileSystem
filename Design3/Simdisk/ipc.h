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

constexpr auto UserCount = 2;	//�������ͬʱ���е�Shell�������

//���ڴ洢Shell���û���Ϣ�Ľṹ��
struct UserInfo
{
	string threadID;
	int id_to_WandR;
	char userID[10];
	char cur_dirN[128];
	Dir user_dir;
	user_type user_tType;
};
//���ڴ洢�������ӵ�Shell���û���Ϣ������
extern vector<UserInfo> ThreadID_UserID;

//���ڡ������ȷ��񡱵��Ŷ���Ϣ�ṹ��
struct QueuedNum
{
	int Requester;
	int ReqCount;
};

unsigned int __stdcall Dispatcher(void* pPM);	// �������߳�
extern bool createError;		//��������״̬λ
extern HANDLE errorSema;		//��֤������ʴ���״̬λ���ź������
extern int curShellCount;		//��ǰ������Simdisk��Shell�������
extern HANDLE exitSema;			//��exit������Ч���ź������

extern HANDLE tuSema;			//��ThreadID_UserID�Ķ�д���ʱ����ź������
extern bool creatingDisk;		//md����ʹ�õĴ���״̬λ
extern bool creatingUser;		//md����ʹ�õĴ����û�״̬λ
extern HANDLE hDSema;			//��֤�����������Ŀ¼��״̬λ���ź������
extern int curShellNum;			//��ǰ������Simdisk��Shell����
extern bool hideDir_zero;		//����Ŀ¼��״̬λ��ר���߳�0��
extern bool hideDir_one;		//����Ŀ¼��״̬λ��ר���߳�1��
extern bool logfail;			//��½ʧ��״̬λ

extern HANDLE mutexSim_zero[3];	//��д�̣߳�ר���߳�0����Dispatcher�߳�ʵ��˳�����е��ź��������

unsigned int __stdcall Manager(void* pPM);	// �������߳�
void ManagerInit();				//����Ա�̵߳ĳ�ʼ������
extern HANDLE m_ThreadControl_Read[2];		//����ר�ö��߳����п��Ƶ��ź��������
extern HANDLE m_ThreadControl_Write[2];		//����ר��д�߳����п��Ƶ��ź��������
extern HANDLE m_ThreadControl_Simdisk;		//����Simdisk���߳����п��Ƶ��ź��������
extern int userCount;			//��ǰ�����û���
void Thread_useradd_p1(const char* strUserId, user_type user_tType);	//ר���߳�0���û���Ӻ���
void Thread_useradd_p2(const char* strUserId, user_type user_tType);	//ר���߳�1���û���Ӻ���
//���������̡߳�Dispatcher�߳���Simdisk���߳�֮��Թ��������Ļ������
extern HANDLE mutexSim[3];


void CloseFileMap();
void CreateFileMap();

extern stringstream sstream_toshell;		//Simdisk���߳�ʵ�ʲ����ķ��͸�Shell������ַ�����
extern stringstream sstream_todisk;			///Simdisk���߳�ʵ�ʲ����ķ��͸�Simdisk���̵߳��ַ�����

extern stringstream sstream_toshell_zero;	//ר���߳�0���͸�Shell������ַ�����
extern stringstream sstream_todisk_zero;	//ר���߳�0���͸�Simdisk���̵߳��ַ�����
extern wstring   m_cstrSzName[2];			//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
extern wstring   m_ReadSemaphoreName[2];	//�����ݵ��ź�����
extern wstring   m_WriteSemaphoreName[2];	//д���ݵ��ź�����
extern HANDLE  m_hMapFile[2];				//�����ڴ���
extern char* m_pBuf[2];						//��ȡ���ݻ���
extern char szInfo[BUF_SIZE];				//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
extern char getInfo[BUF_SIZE];
extern HANDLE	m_Read[2];					//�����ݵ��ź������
extern HANDLE  m_Write[2];					//д���ݵ��ź������

//���������̼߳��˳������
extern CRITICAL_SECTION critical_section;		//ReadSharedData��WriteSharedData֮����ٽ���
extern bool is_Write_turn;						//���õ�ǰ�Ƿ�ΪWriteSharedData�߳����л��ڵĲ�����
extern CONDITION_VARIABLE condition_variable;	//ReadSharedData��WriteSharedData֮�����������


// ���ڿ��� Windows ƽ̨��Ӧ�ó���__stdcall ��Ĭ�ϵĺ�������Լ��
void SendInfoToSimdisk(char getInfo[BUF_SIZE]);
string GetInfoFromSimdisk();
unsigned int __stdcall WriteSharedData(void* pPM);
unsigned int __stdcall ReadSharedData(void* pPM);

extern stringstream sstream_toshell_portal;		//������д�̷߳��͸�Shell������ַ�����
extern stringstream sstream_toManager_portal;	//������д�̷߳��͸�Manager�̵߳��ַ�����
extern wstring   m_cstrSzName_portal[2];		//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
extern wstring   m_ReadSemaphoreName_portal[2]; //�����ݵ��ź�����
extern wstring   m_WriteSemaphoreName_portal[2];//д���ݵ��ź�����
extern HANDLE  m_hMapFile_portal[2];			//�����ڴ���
extern char* m_pBuf_portal[2];					//��ȡ���ݻ���
extern char szInfo_portal[BUF_SIZE];			//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
extern char getInfo_portal[BUF_SIZE];
extern HANDLE	m_Read_portal[2];				//�����ݵ��ź������
extern HANDLE  m_Write_portal[2];				//д���ݵ��ź������

//���������̼߳��ͬ��
extern CRITICAL_SECTION critical_section_portal;		 //ReadSharedData_portal��WriteSharedData_portal֮����ٽ���
extern bool is_Write_turn_portal;						 //���õ�ǰ�Ƿ�ΪWriteSharedData_portal�߳����л��ڵĲ�����
extern CONDITION_VARIABLE condition_variable_portal;	 //ReadSharedData_portal��WriteSharedData_portal֮�����������
//����ReadSharedData_portal��WriteSharedData_portal��Manager���߳�֮��Ļ������
extern HANDLE mutexSim_portal[3];

//�����յ�����Ϣ���͸�Manager�߳�
void SendInfoToManager(char getInfo[BUF_SIZE]);
// ���ڿ��� Windows ƽ̨��Ӧ�ó���__stdcall ��Ĭ�ϵĺ�������Լ��
unsigned int __stdcall WriteSharedData_portal(void* pPM);
unsigned int __stdcall ReadSharedData_portal(void* pPM);

extern stringstream sstream_toshell_one;		//ר���߳�1���͸�Shell������ַ�����
extern stringstream sstream_todisk_one;			//ר���߳�1���͸�Simdisk���̵߳��ַ�����
extern wstring   m_cstrSzName_one[2];			//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
extern wstring   m_ReadSemaphoreName_one[2];	//�����ݵ��ź�����
extern wstring   m_WriteSemaphoreName_one[2];	//д���ݵ��ź�����
extern HANDLE  m_hMapFile_one[2];				//�����ڴ���
extern char* m_pBuf_one[2];						//��ȡ���ݻ���
extern char szInfo_one[BUF_SIZE];				//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf_one[0]
extern char getInfo_one[BUF_SIZE];
extern HANDLE	m_Read_one[2];					//�����ݵ��ź������
extern HANDLE  m_Write_one[2];					//д���ݵ��ź������

//���������̼߳��ͬ��
extern CRITICAL_SECTION critical_section_one;		//ReadSharedData_one��WriteSharedData_one֮����ٽ���
extern bool is_Write_turn_one;						//���õ�ǰ�Ƿ�ΪWriteSharedData_one�߳����л��ڵĲ�����
extern CONDITION_VARIABLE condition_variable_one;	//ReadSharedData_one��WriteSharedData_one֮�����������
//���������߳���Simdisk���߳�֮��Ļ������
extern HANDLE mutexSim_one[3];
void SendInfoToSimdisk_one(char getInfo[BUF_SIZE]);
string GetInfoFromSimdisk_one();
// ���ڿ��� Windows ƽ̨��Ӧ�ó���__stdcall ��Ĭ�ϵĺ�������Լ��
unsigned int __stdcall WriteSharedData_one(void* pPM);
unsigned int __stdcall ReadSharedData_one(void* pPM);