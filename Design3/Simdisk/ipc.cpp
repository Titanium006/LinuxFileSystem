/*
ipc.cpp
���ڹ����ڴ�ͨ�ŵĺ�������
*/
#include"ipc.h"
#include"func.h"
#define _CRT_SECURE_NO_WARNINGS

// ��һ���߳���ı���
bool succeed = false;
HANDLE succeedSema = CreateSemaphore(NULL, 1, 1, NULL);

//�ж��Ƿ���ɵ�ǰ�߳�
HANDLE DoneSema_zero = CreateSemaphore(NULL, 0, 1, NULL);
HANDLE DoneSema_one = CreateSemaphore(NULL, 0, 1, NULL);

// �ڶ����߳���ı���
bool succeed_one = false;
HANDLE succeedSema_one = CreateSemaphore(NULL, 1, 1, NULL);

// ������м�����ʿ����ź���
queue<QueuedNum> RequestQueue;
HANDLE reqSema = CreateSemaphore(NULL, 1, 1, NULL);

bool twoStep_zero = false;				//�Ƿ���Ҫ����������״̬λ��ר���߳�0��
bool twoStep_one = false;				//�Ƿ���Ҫ����������״̬λ��ר���߳�1��
bool hideDir_twice_zero = false;		//�Ƿ���Ҫ���ص�ǰĿ¼�� 2 �ε�״̬λ��ר���߳�0��
bool hideDir_twice_one = false;			//�Ƿ���Ҫ���ص�ǰĿ¼�� 2 �ε�״̬λ��ר���߳�1��
bool threeStep_zero = false;			//�Ƿ���Ҫ����������״̬λ��ר���߳�0��
bool threeStep_one = false;				//�Ƿ���Ҫ����������״̬λ��ר���߳�1��
bool fourStep_zero = false;				//�Ƿ���Ҫ�Ĳ�������״̬λ��ר���߳�0��
bool fourStep_one = false;				//�Ƿ���Ҫ�Ĳ�������״̬λ��ר���߳�1��
bool fiveStep_zero = false;				//�Ƿ���Ҫ�岽������״̬λ��ר���߳�0��
bool fiveStep_one = false;				//�Ƿ���Ҫ�岽������״̬λ��ר���߳�1��

int CountSim_zero = 0;		//�ж��Ƿ��һ������Simdisk���߳�
int CountSim_one = 0;		//�ж��Ƿ��һ������Simdisk���߳�

//�������߳�
unsigned int __stdcall Dispatcher(void* pPM)
{
	int dCount_zero = 0;	//�ж��Ƿ��һ�γɹ�����Dispatcher�߳�
	while (true)
	{
		//����������RequestQueue�������Ƿ��������
		WaitForSingleObject(reqSema, INFINITE);
		if (!RequestQueue.empty())	//������
		{
			QueuedNum tmp;
			tmp = RequestQueue.front();
			RequestQueue.pop();
			ReleaseSemaphore(reqSema, 1, NULL);
			int i = 0;		//ʵ�����д���ͳ��
			while (i < tmp.ReqCount)
			{
				//�������������ר���̺߳ŷֿ�����
				if (tmp.Requester == 0)		//ר���߳�0
				{
					//���õ�ǰ�����Shell����Ӧר���̺߳�
					WaitForSingleObject(hDSema, INFINITE);
					curShellNum = 0;
					ReleaseSemaphore(hDSema, 1, NULL);
					//���sstream_todisk_zero���Ƿ�Ϊ��
					WaitForSingleObject(mutexSim_zero[1], INFINITE);
					if (!sstream_todisk_zero.str().empty())
					{
						sstream_todisk_zero.str(string());
						sstream_todisk_zero.clear();
						ReleaseSemaphore(mutexSim_zero[1], 1, NULL);
						//�����һ�γɹ�����Dispatcher�̣߳�����Ҫ��mutexSim�ź�����������һ�β�����ʹ���ܹ�˳������
						if (dCount_zero == 0)
						{
							WaitForSingleObject(mutexSim[1], INFINITE);
							ReleaseSemaphore(mutexSim[2], 1, NULL);
							dCount_zero = 1;
						}
						ReleaseSemaphore(mutexSim_zero[2], 1, NULL);
						//�ȴ����гɹ����ź���
						WaitForSingleObject(DoneSema_zero, INFINITE);
						//���Simdisk���߳��Ƿ񱨴�
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
				else if (tmp.Requester == 1)	//ר���߳�1
				{
					//���õ�ǰ�����Shell����Ӧר���̺߳�
					WaitForSingleObject(hDSema, INFINITE);
					curShellNum = 1;
					ReleaseSemaphore(hDSema, 1, NULL);
					//���sstream_todisk_one���Ƿ�Ϊ��
					WaitForSingleObject(mutexSim_one[1], INFINITE);
					if (!sstream_todisk_one.str().empty())
					{
						sstream_todisk_one.str(string());
						sstream_todisk_one.clear();
						ReleaseSemaphore(mutexSim_one[1], 1, NULL);
						//�����һ�γɹ�����Dispatcher�̣߳�����Ҫ��mutexSim�ź�����������һ�β�����ʹ���ܹ�˳������
						if (dCount_zero == 0)
						{
							WaitForSingleObject(mutexSim[1], INFINITE);
							ReleaseSemaphore(mutexSim[2], 1, NULL);
							dCount_zero = 1;
						}
						ReleaseSemaphore(mutexSim_one[2], 1, NULL);
						//�ȴ����гɹ����ź���
						WaitForSingleObject(DoneSema_one, INFINITE);
						//���Simdisk���߳��Ƿ񱨴�
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
	vector<string> threadID;		//���ڴ洢�û��̵߳�Ψһ��ʶ��id
	while (1)	//��ÿһ����Ч��ѭ��������������û�������������Դ�����õ���
	{
		WaitForSingleObject(mutexSim_portal[0], INFINITE);
		string id;
		sstream_toManager_portal >> id;
		if (find(threadID.begin(), threadID.end(), id) == threadID.end())	//û���ҵ��û��̵߳�id
		{
			userCount++;
			curShellCount++;
			UserInfo tmp;
			tmp.threadID = id;
			tmp.id_to_WandR = userCount;			//��1��ʼ�趨��Ӧ�Ķ�д�߳�

			// ����tuSema��ThreadID_UserID�������ж�д����
			WaitForSingleObject(tuSema, INFINITE);
			ThreadID_UserID.push_back(tmp);			//�洢�յ����̱߳�ʶ��
			ReleaseSemaphore(tuSema, 1, NULL);

			threadID.push_back(id);

			//�ͷ�Simdisk�����Ӧ���߳̽���ͨ��
			ReleaseSemaphore(m_ThreadControl_Read[userCount - 1], 1, NULL);
			ReleaseSemaphore(m_ThreadControl_Write[userCount - 1], 1, NULL);

			ReleaseSemaphore(mutexSim_portal[1], 1, NULL);	//��write�̸߳��û������û���ʶ����ȷ��ʹ���ĸ��߳̽���ͨ��
		}
		else
		{
			continue;	//�ȴ���һ���û�����
		}
	}
	return true;
}

void ManagerInit()
{
	// ��ʼ������״̬λ
	createError = false;
	errorSema = CreateSemaphore(NULL, 1, 1, NULL);
	//�жϵ�ǰ�û����Ƿ�Ӧ���˳�
	curShellCount = 0;
	exitSema = CreateSemaphore(NULL, 1, 1, NULL);

	hDSema = CreateSemaphore(NULL, 1, 1, NULL);
	curShellNum = -1;

	tuSema = CreateSemaphore(NULL, 1, 1, NULL);

	userCount = 0;	//��ʼ����ǰ�����û���Ϊ0
	for (int i = 0; i < UserCount; i++)
	{
		m_ThreadControl_Read[i] = CreateSemaphore(NULL, 0, 1, NULL);
		m_ThreadControl_Write[i] = CreateSemaphore(NULL, 0, 1, NULL);
	}
	m_ThreadControl_Simdisk = CreateSemaphore(NULL, 0, 1, NULL);		//������Բ�����(LPCWSTR)"ThreadControl_Simdisk"���������������������߳�ͨ���Ժ��ź���ֱ��ʧЧ�����⣡����

	m_cstrSzName_portal[0] = L"NameOfMappingObject0_portal";//�����ڴ�0�����֣�����A��B�жԸñ���������������ͬ��
	m_cstrSzName_portal[1] = L"NameOfMappingObject1_portal";//�����ڴ�1�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_portal[0] = L"ReadSemaphoreName0_portal";//���ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_portal[1] = L"ReadSemaphoreName1_portal";//���ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_portal[0] = L"WriteSemaphoreName0_portal"; //д�ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_portal[1] = L"WriteSemaphoreName1_portal"; //д�ź���1�����֣�����A��B�жԸñ���������������ͬ��

	for (int i = 0; i < 2; i++)
	{
		//1.���������ļ���� hMapFile��CreateFileMapping()��������һ���ļ�ӳ���ں˶���
		m_hMapFile_portal[i] = CreateFileMapping(
			INVALID_HANDLE_VALUE,     //�����ļ��������ΪINVALID_HANDLE_VALUE����Ч������Դ���һ�����̼乲��Ķ���
			NULL,				      //Ĭ�ϰ�ȫ����
			PAGE_READWRITE,           //Ȩ�޿ɶ���д
			0,						  //��λ�ļ���С
			BUF_SIZE,				  //��λ�ļ���С
			(LPCWSTR)m_cstrSzName_portal[i].c_str()			  //�����ڴ���
		);
		//2.��ȡָ���ļ���ͼ��ָ�� pBuf��MapViewOfFile()����������ļ�����ӳ�䵽���̵ĵ�ַ�ռ�
		m_pBuf_portal[i] = (char*)MapViewOfFile(
			m_hMapFile_portal[i],				  //�����ڴ�ľ��
			FILE_MAP_ALL_ACCESS,      //�ɶ�д
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
	is_Write_turn_portal = false;	//����ת�߳��ȶ���д
}

void Thread_useradd_p1(const char* strUserId, user_type user_tType)
{
	//�����ǰ�û����Ͳ���ϵͳ���߹���Ա�򱨴�û��Ȩ�޴����˻�
	if (cur_user_tType != SYSTEM && cur_user_tType != ADMIN) {
		error(NO_PRIVILEGE, "user");
		return;
	}
	if (cur_Idsum < MAX_USER_NUM)	//û�г�������
	{
		//�����Ƿ����ͬ���˺�
		for (unsigned int i = 0; i < cur_Idsum; i++)
		{
			//����˺�ͬ���򱨴��û��Ѵ���
			if (strcmp(systemUsers[i].get_user_id(), strUserId) == 0)
			{
				error(USER_EXIST, strUserId);
				return;
			}
		}
		//��ʼ��
		cout << "����(�20λ)��";
		sstream_toshell_zero << "����(�20λ)��";
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
		//4.�����ļ���ͼUnMapViewOfFile()
		UnmapViewOfFile(m_pBuf[i]);
		//5.�ر�ӳ���ļ����CloseHandle()
		CloseHandle(m_hMapFile[i]);

		//4.�����ļ���ͼUnMapViewOfFile()
		UnmapViewOfFile(m_pBuf_portal[i]);
		//5.�ر�ӳ���ļ����CloseHandle()
		CloseHandle(m_hMapFile_portal[i]);

		//4.�����ļ���ͼUnMapViewOfFile()
		UnmapViewOfFile(m_pBuf_one[i]);
		//5.�ر�ӳ���ļ����CloseHandle()
		CloseHandle(m_hMapFile_one[i]);
	}
}
void CreateFileMap()
{
	m_cstrSzName[0] = L"NameOfMappingObject0";//�����ڴ�0�����֣�����A��B�жԸñ���������������ͬ��
	m_cstrSzName[1] = L"NameOfMappingObject1";//�����ڴ�1�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName[0] = L"ReadSemaphoreName0";//���ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName[1] = L"ReadSemaphoreName1";//���ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName[0] = L"WriteSemaphoreName0"; //д�ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName[1] = L"WriteSemaphoreName1"; //д�ź���1�����֣�����A��B�жԸñ���������������ͬ��


	m_cstrSzName_one[0] = L"NameOfMappingObject0_one";//�����ڴ�0�����֣�����A��B�жԸñ���������������ͬ��
	m_cstrSzName_one[1] = L"NameOfMappingObject1_one";//�����ڴ�1�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_one[0] = L"ReadSemaphoreName0_one";//���ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_one[1] = L"ReadSemaphoreName1_one";//���ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_one[0] = L"WriteSemaphoreName0_one"; //д�ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_one[1] = L"WriteSemaphoreName1_one"; //д�ź���1�����֣�����A��B�жԸñ���������������ͬ��
	for (int i = 0; i < 2; i++)
	{
		//1.���������ļ���� hMapFile��CreateFileMapping()��������һ���ļ�ӳ���ں˶���
		m_hMapFile[i] = CreateFileMapping(
			INVALID_HANDLE_VALUE,     //�����ļ��������ΪINVALID_HANDLE_VALUE����Ч������Դ���һ�����̼乲��Ķ���
			NULL,				      //Ĭ�ϰ�ȫ����
			PAGE_READWRITE,           //Ȩ�޿ɶ���д
			0,						  //��λ�ļ���С
			BUF_SIZE,				  //��λ�ļ���С
			(LPCWSTR)m_cstrSzName[i].c_str()			  //�����ڴ���
		);
		//2.��ȡָ���ļ���ͼ��ָ�� pBuf��MapViewOfFile()����������ļ�����ӳ�䵽���̵ĵ�ַ�ռ�
		m_pBuf[i] = (char*)MapViewOfFile(
			m_hMapFile[i],				  //�����ڴ�ľ��
			FILE_MAP_ALL_ACCESS,      //�ɶ�д
			0,
			0,
			BUF_SIZE
		);

		//1.���������ļ���� hMapFile��CreateFileMapping()��������һ���ļ�ӳ���ں˶���
		m_hMapFile_one[i] = CreateFileMapping(
			INVALID_HANDLE_VALUE,     //�����ļ��������ΪINVALID_HANDLE_VALUE����Ч������Դ���һ�����̼乲��Ķ���
			NULL,				      //Ĭ�ϰ�ȫ����
			PAGE_READWRITE,           //Ȩ�޿ɶ���д
			0,						  //��λ�ļ���С
			BUF_SIZE,				  //��λ�ļ���С
			(LPCWSTR)m_cstrSzName_one[i].c_str()			  //�����ڴ���
		);
		//2.��ȡָ���ļ���ͼ��ָ�� pBuf��MapViewOfFile()����������ļ�����ӳ�䵽���̵ĵ�ַ�ռ�
		m_pBuf_one[i] = (char*)MapViewOfFile(
			m_hMapFile_one[i],				  //�����ڴ�ľ��
			FILE_MAP_ALL_ACCESS,      //�ɶ�д
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

	//������ר���߳�0�еĵ�¼ģ��
	while (true)
	{
		if (cur_Idsum == 0)			// ���й���Ա�˻���ע��ͳ�ʼ������ζ��������߳��ڵ�¼��֤���ر��ǵ�һ������Ա�˻�ע�ᣩ
			//��ʱ�������߳̽��붼����Ҫ�������ȴ��������������Ƴ��޸�userCount����
		{
			switch (process) {
			case 1: {				//���͵ĵ�һ����Ϣ���ȶ����˻���Ϣ
				EnterCriticalSection(&critical_section);
				// �������д���ݵ��ִΣ���ȴ�
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				WaitForSingleObject(m_Write[0], INFINITE);


				cout << "\n\n******************************\n";
				sstream_toshell_zero << "\n\n******************************\n";
				cout << "���ڳ�ʼ���û���Ϣ����\n";
				sstream_toshell_zero << "���ڳ�ʼ���û���Ϣ����\n";
				cout << "ע�����Ա�˻��С���\n";
				sstream_toshell_zero << "ע�����Ա�˻��С���\n";
				//register_user()
				creatingUser = true;
				cmd_md("user", READ_ONLY);
				creatingUser = false;
				cout << "�˺�(�10λ)��";
				sstream_toshell_zero << "�˺�(�10λ)��";

				char szInfo[BUF_SIZE] = { 0 };
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());	// ���ܺ�������ֵconst char* ����������������Ҳ������תc_str()��+'\n'������Ҳ���������
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;							//����һ��ѭ�������¼��֤�ĵڶ�������
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read[0], INFINITE);��ȡ�����ź���֮�����m_pBuf[1]�ڴ�������
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}
			case 2: {				//���͵ĵڶ�����Ϣ��
				EnterCriticalSection(&critical_section);
				// �������д���ݵ��ִΣ���ȴ�
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				WaitForSingleObject(m_Write[0], INFINITE);

				char szInfo[BUF_SIZE] = { 0 };
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;							//����һ��ѭ�������¼��֤�ĵڶ�������
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read[0], INFINITE);��ȡ�����ź���֮�����m_pBuf[1]�ڴ�������
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}	//Load()������
			default: {				//����ʣ�����Ϣ
				EnterCriticalSection(&critical_section);
				// �������д���ݵ��ִΣ���ȴ�
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
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();
				//exitting = true;

				process++;
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read[0], INFINITE);��ȡ�����ź���֮�����m_pBuf[1]�ڴ�������
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
				// �������д���ݵ��ִΣ���ȴ�
				while (!is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}

				//����cmd_logon()
				cout << "��ǰ���� " << nCount << " �λ���" << endl;
				cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				sstream_toshell_zero << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				//�����ʺ�
				cout << "�˺ţ�";
				sstream_toshell_zero << "�˺ţ�";

				char szInfo[BUF_SIZE] = { 0 };
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read[0], INFINITE);��ȡ�����ź���֮�����m_pBuf[1]�ڴ�������
				is_Write_turn = false;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}
			default:
			{
				EnterCriticalSection(&critical_section);
				// �������д���ݵ��ִΣ���ȴ�
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
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_zero.str();
				strcpy_s(szInfo, message.c_str());
				strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_zero.str(string());
				sstream_toshell_zero.clear();

				process++;
				ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read[0], INFINITE);��ȡ�����ź���֮�����m_pBuf[1]�ڴ�������
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
		// �������д���ݵ��ִΣ���ȴ�
		while (!is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}
		//3.�����ݷŵ������ڴ�
			//ͨ��m_pBuf[0]�����ڴ�m_hMapFile[0]��������
		WaitForSingleObject(m_Write[0], INFINITE);
		char szInfo[BUF_SIZE] = { 0 };	// ����
		// дһ�����ܺ��������������ж�ȡ���
		if (relsimdisk == 0 && succeed_one == false)
		{
			ReleaseSemaphore(m_ThreadControl_Simdisk, 1, NULL);		//�ͷ��ź�����Simdisk���߳�����
			relsimdisk = 1;
		}
		string message = GetInfoFromSimdisk();
		//cout << "\n-------------------------\nMessage to Send is:\n" << message << endl;
		//cout << "Length of Message is-----------" << message.length() << endl;
		strcpy_s(szInfo, (message).c_str());	// ���ܺ�������ֵconst char* ����������������Ҳ������תc_str()��+'\n'������Ҳ���������
		strcpy_s(m_pBuf[0], BUF_SIZE, szInfo);
		ReleaseSemaphore(m_Read[0], 1, NULL); //m_Read[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read[0], INFINITE);��ȡ�����ź���֮�����m_pBuf[1]�ڴ�������

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
	int proc = 1;//���������˺Ż�������
	bool success = false;
	int nCountForDirectLog = 2;

	//������ר���߳�0�еĵ�¼ģ��
	while (true)
	{
		if (cur_Idsum == 0)
		{
			switch (process)
			{
			case 1:
			{
				EnterCriticalSection(&critical_section);
				// �������д���ݵ��ִΣ���ȴ�
				while (is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				//3.�ӹ����ڴ��ж�ȡ����
				WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]�ź����ɳ���B����

				char getInfo[BUF_SIZE] = { 0 };		// ����
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
				// �������д���ݵ��ִΣ���ȴ�
				while (is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				//3.�ӹ����ڴ��ж�ȡ����
				WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]�ź����ɳ���B����
				char getInfo[BUF_SIZE] = { 0 };		// ����
				strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);

				//cin_pwd()
				stringstream ss;
				ss << getInfo;
				int nlen = MAX_PWD_LENGTH;
				strcpy_s(password, MAX_PWD_LENGTH, ss.str().c_str());
				stringstream ssStream;
				ssStream << password << " " << (int)ADMIN;
				char strBuffer[MAX_PWD_LENGTH + 5];//�����ַ����������������û����������
				strcpy_s(strBuffer, 25, ssStream.str().c_str());
				ssStream.clear();
				//��ʽ����ɣ�д���ļ�
				Dir dirUsr;
				analyse_path("/user", dirUsr);
				char* tem = new char[MAX_NAME_LENGTH];
				strcpy_s(tem, 10, ID);
				save_file(tem, strBuffer, MAX_PWD_LENGTH + 5, dirUsr, PROTECTED);
				//���һ���û����ڴ�
				cur_Idsum++;
				systemUsers[cur_Idsum - 1].set_user_info(ID, password, ADMIN);
				//��ȡ�û���Ϣ
				get_user_info();
				memset(cur_user_Id, 0, MAX_ID_LENGTH);
				strcpy_s(cur_user_Id, 10, systemUsers[0].get_user_id());	//��Ϊ��ע��ĵ�һ���û����Էŵ�[0]��λ��
				cur_user_tType = systemUsers[0].get_user_type();
				//��ʼ����ɣ�����
				cout << "\n******************************\n";
				sstream_toshell_zero << "\n******************************\n";
				cout << "��ʼ����ɣ�����ת���¼���桭��\n";
				sstream_toshell_zero << "��ʼ����ɣ�����ת���¼���桭��\n";
				cout << "**********************���̳�ʼ�����**********************" << endl;
				sstream_toshell_zero << "**********************���̳�ʼ�����**********************\n";
				get_user_info();

				//����cmd_logon()
				nCount--;
				cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				sstream_toshell_zero << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				//�����ʺ�
				cout << "�˺ţ�";
				sstream_toshell_zero << "�˺ţ�";

				process++;
				ReleaseSemaphore(m_Write[1], 1, NULL);
				is_Write_turn = true;
				WakeConditionVariable(&condition_variable);
				LeaveCriticalSection(&critical_section);
				break;
			}		//Load()������
			default:
			{
				if (nCount >= 0)
				{
					EnterCriticalSection(&critical_section);
					// �������д���ݵ��ִΣ���ȴ�
					while (is_Write_turn)
					{
						SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
					}
					//3.�ӹ����ڴ��ж�ȡ����
					WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]�ź����ɳ���B����
					char getInfo[BUF_SIZE] = { 0 };		// ����
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
						//�������룬*����
						cout << "���룺";
						sstream_toshell_zero << "���룺";
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
							// �Ƚ������Ƿ���ȷ
							//cout << systemUsers[i].get_user_id() << endl;
							//cout << systemUsers[i].UserPWD << endl;
							if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
								&& systemUsers[i].cmp_pwd(strPwd))
							{
								//δ���ж��û�������cur_user_Id!!!
								memset(cur_user_Id, 0, MAX_ID_LENGTH);
								strcpy_s(cur_user_Id, 10, nUid);
								cur_user_tType = systemUsers[i].get_user_type();
								cout << "\n\n******************************\n";
								sstream_toshell_zero << "\n\n******************************\n";
								cout << "��¼�ɹ������Ժ򡭡�\n";
								sstream_toshell_zero << "��¼�ɹ������Ժ򡭡�\n";
								Sleep(800);

								WaitForSingleObject(tuSema, INFINITE);
								strcpy_s(ThreadID_UserID[0].userID, cur_user_Id);
								ReleaseSemaphore(tuSema, 1, NULL);

								//stringstream ssStream;
								//ssStream << "title simdisk [�ѵ�¼��" << cur_user_Id << "(";
								//if (cur_user_tType == USER)
								//	ssStream << "��ͨ�û�";
								//else
								//	ssStream << "����Ա";
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
							cout << "�ʺŲ����ڻ��������\n�㻹��" << nCount << "�����Ի��ᡣ" << endl;
							sstream_toshell_zero << "�ʺŲ����ڻ��������\n�㻹��" << nCount << "�����Ի��ᡣ\n";
							//system("cls");
							nCount--;
							cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
							sstream_toshell_zero << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
							//�����ʺ�
							cout << "�˺ţ�";
							sstream_toshell_zero << "�˺ţ�";
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
				//cout << "���߳̽���˯�ߣ�����д�߳̽�����һ�����ڣ�" << endl;
				//Sleep(10000000);
				break;
			}
		}
		else
		{
			if (nCountForDirectLog >= 0)
			{
				EnterCriticalSection(&critical_section);
				// �������д���ݵ��ִΣ���ȴ�
				while (is_Write_turn)
				{
					SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
				}
				//3.�ӹ����ڴ��ж�ȡ����
				WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]�ź����ɳ���B����
				char getInfo[BUF_SIZE] = { 0 };		// ����
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
					//�������룬*����
					cout << "���룺";
					sstream_toshell_zero << "���룺";
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
						// �Ƚ������Ƿ���ȷ
						//cout << systemUsers[i].get_user_id() << endl;
						//cout << systemUsers[i].UserPWD << endl;
						if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
							&& systemUsers[i].cmp_pwd(strPwd))
						{
							//δ���ж��û�������cur_user_Id!!!
							memset(cur_user_Id, 0, MAX_ID_LENGTH);
							strcpy_s(cur_user_Id, 10, nUid);
							cur_user_tType = systemUsers[i].get_user_type();
							cout << "\n\n******************************\n";
							sstream_toshell_zero << "\n\n******************************\n";
							cout << "��¼�ɹ������Ժ򡭡�\n";
							sstream_toshell_zero << "��¼�ɹ������Ժ򡭡�\n";
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
						cout << "�ʺŲ����ڻ��������\n�㻹��" << nCountForDirectLog << "�����Ի��ᡣ" << endl;
						sstream_toshell_zero << "�ʺŲ����ڻ��������\n�㻹��" << nCountForDirectLog << "�����Ի��ᡣ\n";
						//system("cls");
						nCountForDirectLog--;
						cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
						sstream_toshell_zero << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
						//�����ʺ�
						cout << "�˺ţ�";
						sstream_toshell_zero << "�˺ţ�";
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
		// �������д���ݵ��ִΣ���ȴ�
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}

		//3.�ӹ����ڴ��ж�ȡ����
		WaitForSingleObject(m_Read[1], INFINITE); //m_Read[1]�ź����ɳ���B����
		//cout << "Have got m_Read[1]!" << endl;
		char getInfo[BUF_SIZE] = { 0 };		// ����
		strcpy_s(getInfo, BUF_SIZE, m_pBuf[1]);
		cout << "\n��Shell1��������Ϣ�ǣ�" << getInfo << endl;
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
	//����Ϣд��sstream_todisk_zero��
	WaitForSingleObject(mutexSim_zero[1], INFINITE);
	sstream_todisk_zero.str(string());
	sstream_todisk_zero.clear();
	sstream_todisk_zero << getInfo;
	ReleaseSemaphore(mutexSim_zero[1], 1, NULL);

	//д���������
	QueuedNum tmp;
	tmp.Requester = 0;

	//��ȡ����
	stringstream stmp;
	stmp.str(getInfo);
	char cmd[512], strArgv1[128];
	stmp >> cmd >> strArgv1;

	//�������������ж���Ҫ���󼸴ζ�дѭ���������ö�Ӧ������Ŀ¼״̬λ�����в���״̬λ
	if (strlen(cmd) != 0)
	{
		int unsigned i;
		for (i = 0; i < COMMAND_COUNT; i++) {
			//cout << "\n-------------------\nҪ�Ƚϵ�ָ���ǣ�" << commondName << "\t��\t" << strCommands[i] << endl;
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

	//�ȴ�Dispatcher����
	WaitForSingleObject(mutexSim_zero[2], INFINITE);
	WaitForSingleObject(mutexSim[2], INFINITE);

	//���洢���û���Ϣд��Simdisk���̵߳ĵ�ǰ�û�������
	WaitForSingleObject(tuSema, INFINITE);
	cur_user_tType = ThreadID_UserID[0].user_tType;
	cur_dir = ThreadID_UserID[0].user_dir;
	strcpy_s(cur_user_Id, ThreadID_UserID[0].userID);
	strcpy_s(cur_dirName, ThreadID_UserID[0].cur_dirN);
	ReleaseSemaphore(tuSema, 1, NULL);

	//����Ϣ���͸�Simdisk���߳�
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

		//�õ�ǰ�û���Ϣ���´洢���û���Ϣ
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

		//д���������
		QueuedNum tmp;
		tmp.Requester = 0;
		if (logfail)	//�����¼ʧ��
		{
			tmp.ReqCount = 2;
			hideDir_zero = true;
			twoStep_zero = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
			logfail = false;
		}

		//д��sstream_toshell_zero��
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
		// �������д���ݵ��ִΣ���ȴ�
		while (!is_Write_turn_portal)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}

		//3.�����ݷŵ������ڴ�
			//ͨ��m_pBuf[0]�����ڴ�m_hMapFile[0]��������
		WaitForSingleObject(m_Write_portal[0], INFINITE);
		char szInfo_portal[BUF_SIZE] = { 0 };	// ����
		WaitForSingleObject(mutexSim_portal[1], INFINITE);	//�ȴ�Manager������
		string message = to_string(userCount);				//����shell���Ǵ� 1 ��ʼ��userCount
		ReleaseSemaphore(mutexSim_portal[2], 1, NULL);
		strcpy_s(szInfo_portal, message.c_str());	// ���ܺ�������ֵconst char* ����������������Ҳ������תc_str()��+'\n'������Ҳ���������
		strcpy_s(m_pBuf_portal[0], BUF_SIZE, szInfo_portal);
		ReleaseSemaphore(m_Read_portal[0], 1, NULL); //m_Read[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read[0], INFINITE);��ȡ�����ź���֮�����m_pBuf[1]�ڴ�������

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
		// �������д���ݵ��ִΣ���ȴ�
		while (is_Write_turn_portal)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}
		//3.�ӹ����ڴ��ж�ȡ����
		WaitForSingleObject(m_Read_portal[1], INFINITE); //m_Read[1]�ź����ɳ���B����
		char getInfo_portal[BUF_SIZE] = { 0 };		// ����
		strcpy_s(getInfo_portal, BUF_SIZE, m_pBuf_portal[1]);
		// �����յ����û�ʶ���룡
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
	//������ר���߳�1�еĵ�¼ģ��
	while (true)
	{
		if (cur_Idsum == 0)			// ���й���Ա�˻���ע��ͳ�ʼ������ζ��������߳��ڵ�¼��֤���ر��ǵ�һ������Ա�˻�ע�ᣩ
			//��ʱ�������߳̽��붼����Ҫ�������ȴ��������������Ƴ��޸�userCount����
		{
			switch (process) {
			case 1: {				//���͵ĵ�һ����Ϣ���ȶ����˻���Ϣ
				EnterCriticalSection(&critical_section_one);
				// �������д���ݵ��ִΣ���ȴ�
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				WaitForSingleObject(m_Write_one[0], INFINITE);


				cout << "\n\n******************************\n";
				sstream_toshell_one << "\n\n******************************\n";
				cout << "���ڳ�ʼ���û���Ϣ����\n";
				sstream_toshell_one << "���ڳ�ʼ���û���Ϣ����\n";
				cout << "ע�����Ա�˻��С���\n";
				sstream_toshell_one << "ע�����Ա�˻��С���\n";
				//register_user()
				creatingUser = true;
				cmd_md("user", READ_ONLY);
				creatingUser = false;
				cout << "�˺�(�10λ)��";
				sstream_toshell_one << "�˺�(�10λ)��";

				char szInfo_one[BUF_SIZE] = { 0 };
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());	// ���ܺ�������ֵconst char* ����������������Ҳ������תc_str()��+'\n'������Ҳ���������
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;							//����һ��ѭ�������¼��֤�ĵڶ�������
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read_one[0], INFINITE);��ȡ�����ź���֮�����m_pBuf_one[1]�ڴ�������
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}
			case 2: {				//���͵ĵڶ�����Ϣ��
				EnterCriticalSection(&critical_section_one);
				// �������д���ݵ��ִΣ���ȴ�
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				WaitForSingleObject(m_Write_one[0], INFINITE);

				char szInfo_one[BUF_SIZE] = { 0 };
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;							//����һ��ѭ�������¼��֤�ĵڶ�������
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read_one[0], INFINITE);��ȡ�����ź���֮�����m_pBuf_one[1]�ڴ�������
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}	//Load()������
			default: {				//����ʣ�����Ϣ
				EnterCriticalSection(&critical_section_one);
				// �������д���ݵ��ִΣ���ȴ�
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
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();
				//exitting = true;

				process++;
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read_one[0], INFINITE);��ȡ�����ź���֮�����m_pBuf_one[1]�ڴ�������
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
				// �������д���ݵ��ִΣ���ȴ�
				while (!is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}

				//����cmd_logon()
				cout << "��ǰ���� " << nCount << " �λ���" << endl;
				cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				sstream_toshell_one << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				//�����ʺ�
				cout << "�˺ţ�";
				sstream_toshell_one << "�˺ţ�";

				char szInfo_one[BUF_SIZE] = { 0 };
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read_one[0], INFINITE);��ȡ�����ź���֮�����m_pBuf_one[1]�ڴ�������
				is_Write_turn_one = false;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}
			default:
			{
				EnterCriticalSection(&critical_section_one);
				// �������д���ݵ��ִΣ���ȴ�
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
				// дһ�����ܺ��������������ж�ȡ���
				string message = sstream_toshell_one.str();
				strcpy_s(szInfo_one, message.c_str());
				strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);

				//�����ɱ���Ҫ����������!!!
				sstream_toshell_one.str(string());
				sstream_toshell_one.clear();

				process++;
				ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read_one[0], INFINITE);��ȡ�����ź���֮�����m_pBuf_one[1]�ڴ�������
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
		// �������д���ݵ��ִΣ���ȴ�
		while (!is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}
		//3.�����ݷŵ������ڴ�
			//ͨ��m_pBuf_one[0]�����ڴ�m_hMapFile_one[0]��������
		WaitForSingleObject(m_Write_one[0], INFINITE);
		char szInfo_one[BUF_SIZE] = { 0 };	// ����
		// дһ�����ܺ��������������ж�ȡ���
		if (relsimdisk == 0 && succeed == false)
		{
			ReleaseSemaphore(m_ThreadControl_Simdisk, 1, NULL);		//�ͷ��ź�����Simdisk���߳�����
			relsimdisk = 1;
		}
		string message = GetInfoFromSimdisk_one();
		//cout << "\n-------------------------\nMessage to Send is:\n" << message << endl;
		//cout << "Length of Message is-----------" << message.length() << endl;
		strcpy_s(szInfo_one, (message).c_str());	// ���ܺ�������ֵconst char* ����������������Ҳ������תc_str()��+'\n'������Ҳ���������
		strcpy_s(m_pBuf_one[0], BUF_SIZE, szInfo_one);
		ReleaseSemaphore(m_Read_one[0], 1, NULL); //m_Read_one[0]����һ���ź������ɳ���A�е�WaitForSingleObject(m_Read_one[0], INFINITE);��ȡ�����ź���֮�����m_pBuf_one[1]�ڴ�������

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
	int proc = 1;//���������˺Ż�������
	bool success = false;
	int nCountForDirectLog = 2;
	//������ר���߳�1�еĵ�¼ģ��
	while (true)
	{
		if (cur_Idsum == 0)
		{
			switch (process)
			{
			case 1:
			{
				EnterCriticalSection(&critical_section_one);
				// �������д���ݵ��ִΣ���ȴ�
				while (is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				//3.�ӹ����ڴ��ж�ȡ����
				WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]�ź����ɳ���B����

				char getInfo_one[BUF_SIZE] = { 0 };		// ����
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
				// �������д���ݵ��ִΣ���ȴ�
				while (is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				//3.�ӹ����ڴ��ж�ȡ����
				WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]�ź����ɳ���B����
				char getInfo_one[BUF_SIZE] = { 0 };		// ����
				strcpy_s(getInfo_one, BUF_SIZE, m_pBuf_one[1]);

				//cin_pwd()
				stringstream ss;
				ss << getInfo_one;
				int nlen = MAX_PWD_LENGTH;
				strcpy_s(password, MAX_PWD_LENGTH, ss.str().c_str());
				stringstream ssStream;
				ssStream << password << " " << (int)ADMIN;
				char strBuffer[MAX_PWD_LENGTH + 5];//�����ַ����������������û����������
				strcpy_s(strBuffer, 25, ssStream.str().c_str());
				ssStream.clear();
				//��ʽ����ɣ�д���ļ�
				Dir dirUsr;
				analyse_path("/user", dirUsr);
				char* tem = new char[MAX_NAME_LENGTH];
				strcpy_s(tem, 10, ID);
				save_file(tem, strBuffer, MAX_PWD_LENGTH + 5, dirUsr, PROTECTED);
				//���һ���û����ڴ�
				cur_Idsum++;
				systemUsers[cur_Idsum - 1].set_user_info(ID, password, ADMIN);
				//��ȡ�û���Ϣ
				get_user_info();
				memset(cur_user_Id, 0, MAX_ID_LENGTH);
				strcpy_s(cur_user_Id, 10, systemUsers[0].get_user_id());	//��Ϊ��ע��ĵ�һ���û����Էŵ�[0]��λ��
				cur_user_tType = systemUsers[0].get_user_type();
				//��ʼ����ɣ�����
				cout << "\n******************************\n";
				sstream_toshell_one << "\n******************************\n";
				cout << "��ʼ����ɣ�����ת���¼���桭��\n";
				sstream_toshell_one << "��ʼ����ɣ�����ת���¼���桭��\n";
				cout << "**********************���̳�ʼ�����**********************" << endl;
				sstream_toshell_one << "**********************���̳�ʼ�����**********************\n";
				get_user_info();

				//����cmd_logon()
				nCount--;
				cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				sstream_toshell_one << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
				//�����ʺ�
				cout << "�˺ţ�";
				sstream_toshell_one << "�˺ţ�";

				process++;
				ReleaseSemaphore(m_Write_one[1], 1, NULL);
				is_Write_turn_one = true;
				WakeConditionVariable(&condition_variable_one);
				LeaveCriticalSection(&critical_section_one);
				break;
			}		//Load()������
			default:
			{
				if (nCount >= 0)
				{
					EnterCriticalSection(&critical_section_one);
					// �������д���ݵ��ִΣ���ȴ�
					while (is_Write_turn_one)
					{
						SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
					}
					//3.�ӹ����ڴ��ж�ȡ����
					WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]�ź����ɳ���B����
					char getInfo_one[BUF_SIZE] = { 0 };		// ����
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
						//�������룬*����
						cout << "���룺";
						sstream_toshell_one << "���룺";
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
							// �Ƚ������Ƿ���ȷ
							//cout << systemUsers[i].get_user_id() << endl;
							//cout << systemUsers[i].UserPWD << endl;
							if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
								&& systemUsers[i].cmp_pwd(strPwd))
							{
								//δ���ж��û�������cur_user_Id!!!
								memset(cur_user_Id, 0, MAX_ID_LENGTH);
								strcpy_s(cur_user_Id, 10, nUid);
								cur_user_tType = systemUsers[i].get_user_type();
								cout << "\n\n******************************\n";
								sstream_toshell_one << "\n\n******************************\n";
								cout << "��¼�ɹ������Ժ򡭡�\n";
								sstream_toshell_one << "��¼�ɹ������Ժ򡭡�\n";
								Sleep(800);

								WaitForSingleObject(tuSema, INFINITE);
								strcpy_s(ThreadID_UserID[1].userID, cur_user_Id);
								ReleaseSemaphore(tuSema, 1, NULL);

								//stringstream ssStream;
								//ssStream << "title simdisk [�ѵ�¼��" << cur_user_Id << "(";
								//if (cur_user_tType == USER)
								//	ssStream << "��ͨ�û�";
								//else
								//	ssStream << "����Ա";
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
							cout << "�ʺŲ����ڻ��������\n�㻹��" << nCount << "�����Ի��ᡣ" << endl;
							sstream_toshell_one << "�ʺŲ����ڻ��������\n�㻹��" << nCount << "�����Ի��ᡣ\n";
							//system("cls");
							nCount--;
							cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
							sstream_toshell_one << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
							//�����ʺ�
							cout << "�˺ţ�";
							sstream_toshell_one << "�˺ţ�";
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
				// �������д���ݵ��ִΣ���ȴ�
				while (is_Write_turn_one)
				{
					SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
				}
				//3.�ӹ����ڴ��ж�ȡ����
				WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]�ź����ɳ���B����
				char getInfo_one[BUF_SIZE] = { 0 };		// ����
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
					//�������룬*����
					cout << "���룺";
					sstream_toshell_one << "���룺";
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
						// �Ƚ������Ƿ���ȷ
						//cout << systemUsers[i].get_user_id() << endl;
						//cout << systemUsers[i].UserPWD << endl;
						if (strcmp(nUid, systemUsers[i].get_user_id()) == 0
							&& systemUsers[i].cmp_pwd(strPwd))
						{
							//δ���ж��û�������cur_user_Id!!!
							memset(cur_user_Id, 0, MAX_ID_LENGTH);
							strcpy_s(cur_user_Id, 10, nUid);
							cur_user_tType = systemUsers[i].get_user_type();
							cout << "\n\n******************************\n";
							sstream_toshell_one << "\n\n******************************\n";
							cout << "��¼�ɹ������Ժ򡭡�\n";
							sstream_toshell_one << "��¼�ɹ������Ժ򡭡�\n";
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
						cout << "�ʺŲ����ڻ��������\n�㻹��" << nCountForDirectLog << "�����Ի��ᡣ" << endl;
						sstream_toshell_one << "�ʺŲ����ڻ��������\n�㻹��" << nCountForDirectLog << "�����Ի��ᡣ\n";
						//system("cls");
						nCountForDirectLog--;
						cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
						sstream_toshell_one << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
						//�����ʺ�
						cout << "�˺ţ�";
						sstream_toshell_one << "�˺ţ�";
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
		// �������д���ݵ��ִΣ���ȴ�
		while (is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}

		//3.�ӹ����ڴ��ж�ȡ����
		WaitForSingleObject(m_Read_one[1], INFINITE); //m_Read_one[1]�ź����ɳ���B����
		char getInfo_one[BUF_SIZE] = { 0 };		// ����
		strcpy_s(getInfo_one, BUF_SIZE, m_pBuf_one[1]);
		cout << "\n��Shell2��������Ϣ�ǣ�" << getInfo_one << endl;
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
	//����Ϣд��sstream_todisk_one��
	WaitForSingleObject(mutexSim_one[1], INFINITE);
	sstream_todisk_one.str(string());
	sstream_todisk_one.clear();
	sstream_todisk_one << getInfo;
	ReleaseSemaphore(mutexSim_one[1], 1, NULL);

	//д���������
	QueuedNum tmp;
	tmp.Requester = 1;

	//��ȡ����
	stringstream stmp;
	stmp.str(getInfo);
	char cmd[512], strArgv1[128];
	stmp >> cmd >> strArgv1;

	//�������������ж���Ҫ���󼸴ζ�дѭ���������ö�Ӧ������Ŀ¼״̬λ�����в���״̬λ
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

	//�ȴ�Dispatcher����
	WaitForSingleObject(mutexSim_one[2], INFINITE);
	WaitForSingleObject(mutexSim[2], INFINITE);

	//���洢���û���Ϣд��Simdisk���̵߳ĵ�ǰ�û�������
	WaitForSingleObject(tuSema, INFINITE);
	strcpy_s(cur_user_Id, ThreadID_UserID[1].userID);
	strcpy_s(cur_dirName, ThreadID_UserID[1].cur_dirN);
	cur_dir = ThreadID_UserID[1].user_dir;
	cur_user_tType = ThreadID_UserID[1].user_tType;
	ReleaseSemaphore(tuSema, 1, NULL);

	//����Ϣ���͸�Simdisk���߳�
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

		//�õ�ǰ�û���Ϣ���´洢���û���Ϣ
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

		//д���������
		QueuedNum tmp;
		tmp.Requester = 1;
		if (logfail)	//�����½ʧ��
		{
			tmp.ReqCount = 2;
			hideDir_one = true;
			twoStep_one = true;
			WaitForSingleObject(reqSema, INFINITE);
			RequestQueue.push(tmp);
			ReleaseSemaphore(reqSema, 1, NULL);
			logfail = false;
		}

		//д��sstream_toshell_one��
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
	//�����ǰ�û����Ͳ���ϵͳ���߹���Ա�򱨴�û��Ȩ�޴����˻�
	if (cur_user_tType != SYSTEM && cur_user_tType != ADMIN) {
		error(NO_PRIVILEGE, "user");
		return;
	}
	if (cur_Idsum < MAX_USER_NUM)	//û�г�������
	{
		//�����Ƿ����ͬ���˺�
		for (unsigned int i = 0; i < cur_Idsum; i++)
		{
			//����˺�ͬ���򱨴��û��Ѵ���
			if (strcmp(systemUsers[i].get_user_id(), strUserId) == 0)
			{
				error(USER_EXIST, strUserId);
				return;
			}
		}
		//��ʼ��
		cout << "����(�20λ)��";
		sstream_toshell_one << "����(�20λ)��";
	}
	else
	{
		error(USERS_MAX);
	}
}