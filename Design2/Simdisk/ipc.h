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
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

constexpr auto BUF_SIZE = 1024;

extern stringstream sstream_toshell;
extern stringstream sstream_todisk;
extern wstring   m_cstrSzName[2];//�����ڴ�����һ��������һ����A�����ݣ���Bȡ���ݣ�һ����B�����ݣ���Aȡ���ݣ�
extern wstring   m_ReadSemaphoreName[2]; //�����ݵ��ź�����
extern wstring   m_WriteSemaphoreName[2];//д���ݵ��ź�����
extern wstring   m_ReadBeforeWrite;		//����д���ź�����
extern HANDLE  m_hMapFile[2];//�����ڴ���
extern char* m_pBuf[2]; //��ȡ���ݻ���
extern char szInfo[BUF_SIZE];//�ȴ洢ϵͳ������Ϣ�����ٴ���m_pBuf[0]
extern char getInfo[BUF_SIZE];
extern HANDLE	m_Read[2];//�����ݵ��ź������
extern HANDLE  m_Write[2];//д���ݵ��ź������

//���������̼߳����������
extern CRITICAL_SECTION critical_section;		//ReadSharedData��WriteSharedData֮����ٽ���
extern bool is_Write_turn;						//���õ�ǰ�Ƿ�ΪWriteSharedData�߳����л��ڵĲ�����
extern CONDITION_VARIABLE condition_variable;	//ReadSharedData��WriteSharedData֮�����������

//���������߳���Simdisk���߳�֮��Ļ������
extern HANDLE mutexSim[3];

//����simdisk��������д�߳�ͬ��
//extern HANDLE mutexSimdisk;

void CloseFileMap();
void CreateFileMap();
// ���ڿ��� Windows ƽ̨��Ӧ�ó���__stdcall ��Ĭ�ϵĺ�������Լ��
unsigned int __stdcall WriteSharedData(void* pPM);
unsigned int __stdcall ReadSharedData(void* pPM);

//��Simdisk���߳̽���ͨ�ŵĺ���
void SendInfoToSimdisk(char getInfo[BUF_SIZE]);
string GetInfoFromSimdisk();
