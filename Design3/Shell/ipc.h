#pragma once
#pragma once
/*
ipc.h
���ڹ����ڴ�ͨ�ŵĺ�������
*/
#include <windows.h>
#include <process.h>
#include<iostream>
#include<string>
#include<sstream>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

constexpr auto BUF_SIZE = 1024;

extern int UserCount;					//��ǰ�û�����
extern int identification;				//Simdisk������򷵻ص�ͨ��ʹ�õ�ר���̵߳��̺߳�
extern HANDLE m_ThreadControl_Read[2];	//����ר�ö��߳����п��Ƶ��ź��������
extern HANDLE m_ThreadControl_Write[2];	//����ר��д�߳����п��Ƶ��ź��������

extern wstring   m_cstrSzName[2];//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
extern wstring   m_ReadSemaphoreName[2]; //�����ݵ��ź�����
extern wstring   m_WriteSemaphoreName[2];//д���ݵ��ź�����
extern HANDLE  m_hMapFile[2];//�����ڴ���
extern char* m_pBuf[2]; //��ȡ���ݻ���
extern char szInfo[BUF_SIZE];//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
extern char getInfo[BUF_SIZE];//�洢�ӹ����ڴ��ȡ������
extern HANDLE	m_Read[2];//�����ݵ��ź������
extern HANDLE  m_Write[2];//д���ݵ��ź������
extern CRITICAL_SECTION critical_section;					//ReadSharedData��WriteSharedData֮����ٽ���
extern bool is_Write_turn;									//���õ�ǰ�Ƿ�ΪWriteSharedData�߳����л��ڵĲ�����
extern CONDITION_VARIABLE condition_variable;				//ReadSharedData��WriteSharedData֮�����������
extern HANDLE exitSemaphore;//�˳��ź���
unsigned int __stdcall WriteSharedData(void* pPM);
unsigned int __stdcall ReadSharedData(void* pPM);

extern wstring   m_cstrSzName_portal[2];//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
extern wstring   m_ReadSemaphoreName_portal[2]; //�����ݵ��ź�����
extern wstring   m_WriteSemaphoreName_portal[2];//д���ݵ��ź�����
extern HANDLE  m_hMapFile_portal[2];//�����ڴ���
extern char* m_pBuf_portal[2]; //��ȡ���ݻ���
extern char szInfo_portal[BUF_SIZE];//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
extern char getInfo_portal[BUF_SIZE];
extern HANDLE	m_Read_portal[2];//�����ݵ��ź������
extern HANDLE  m_Write_portal[2];//д���ݵ��ź������
//���������̼߳��ͬ��
extern CRITICAL_SECTION critical_section_portal;			//ReadSharedData_portal��WriteSharedData_portal֮����ٽ���
extern int is_Write_turn_portal;							//���õ�ǰ�Ƿ�ΪWriteSharedData_portal�߳����л��ڵĲ�����
extern CONDITION_VARIABLE condition_variable_portal;		//ReadSharedData_portal��WriteSharedData_portal֮�����������
// ���ڿ��� Windows ƽ̨��Ӧ�ó���__stdcall ��Ĭ�ϵĺ�������Լ��
unsigned int __stdcall WriteSharedData_portal(void* pPM);
unsigned int __stdcall ReadSharedData_portal(void* pPM);

extern wstring   m_cstrSzName_one[2];//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
extern wstring   m_ReadSemaphoreName_one[2]; //�����ݵ��ź�����
extern wstring   m_WriteSemaphoreName_one[2];//д���ݵ��ź�����
extern HANDLE  m_hMapFile_one[2];//�����ڴ���
extern char* m_pBuf_one[2]; //��ȡ���ݻ���
extern char szInfo_one[BUF_SIZE];//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
extern char getInfo_one[BUF_SIZE];
extern HANDLE	m_Read_one[2];//�����ݵ��ź������
extern HANDLE  m_Write_one[2];//д���ݵ��ź������
//���������̼߳��ͬ��
extern CRITICAL_SECTION critical_section_one;				//ReadSharedData_one��WriteSharedData_one֮����ٽ���
extern bool is_Write_turn_one;								//���õ�ǰ�Ƿ�ΪWriteSharedData_one�߳����л��ڵĲ�����
extern CONDITION_VARIABLE condition_variable_one;			//ReadSharedData_one��WriteSharedData_one֮�����������
// ���ڿ��� Windows ƽ̨��Ӧ�ó���__stdcall ��Ĭ�ϵĺ�������Լ��
unsigned int __stdcall WriteSharedData_one(void* pPM);
unsigned int __stdcall ReadSharedData_one(void* pPM);

void CloseFileMap();
void CreateFileMap();