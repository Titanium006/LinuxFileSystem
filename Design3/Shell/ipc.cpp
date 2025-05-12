#include"ipc.h"

void CloseFileMap()
{
	for (int i = 0; i < 2; i++)
	{
		if (identification == 1)
		{
			//4.�����ļ���ͼUnMapViewOfFile()
			UnmapViewOfFile(m_pBuf[i]);
			//5.�ر�ӳ���ļ����CloseHandle()
			CloseHandle(m_hMapFile[i]);
		}
		else if (identification == 2)
		{
			//4.�����ļ���ͼUnMapViewOfFile()
			UnmapViewOfFile(m_pBuf_one[i]);
			//5.�ر�ӳ���ļ����CloseHandle()
			CloseHandle(m_hMapFile_one[i]);
		}
		//4.�����ļ���ͼUnMapViewOfFile()
		UnmapViewOfFile(m_pBuf_portal[i]);
		//5.�ر�ӳ���ļ����CloseHandle()
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

	m_cstrSzName[0] = L"NameOfMappingObject0";//�����ڴ�0�����֣�����A��B�жԸñ���������������ͬ��
	m_cstrSzName[1] = L"NameOfMappingObject1";//�����ڴ�1�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName[0] = L"ReadSemaphoreName0";//���ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName[1] = L"ReadSemaphoreName1";//���ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName[0] = L"WriteSemaphoreName0";//д�ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName[1] = L"WriteSemaphoreName1";//д�ź���1�����֣�����A��B�жԸñ���������������ͬ��

	m_cstrSzName_portal[0] = L"NameOfMappingObject0_portal";//�����ڴ�0�����֣�����A��B�жԸñ���������������ͬ��
	m_cstrSzName_portal[1] = L"NameOfMappingObject1_portal";//�����ڴ�1�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_portal[0] = L"ReadSemaphoreName0_portal";//���ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_portal[1] = L"ReadSemaphoreName1_portal";//���ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_portal[0] = L"WriteSemaphoreName0_portal"; //д�ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_portal[1] = L"WriteSemaphoreName1_portal"; //д�ź���1�����֣�����A��B�жԸñ���������������ͬ��

	m_cstrSzName_one[0] = L"NameOfMappingObject0_one";//�����ڴ�0�����֣�����A��B�жԸñ���������������ͬ��
	m_cstrSzName_one[1] = L"NameOfMappingObject1_one";//�����ڴ�1�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_one[0] = L"ReadSemaphoreName0_one";//���ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName_one[1] = L"ReadSemaphoreName1_one";//���ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_one[0] = L"WriteSemaphoreName0_one"; //д�ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName_one[1] = L"WriteSemaphoreName1_one"; //д�ź���1�����֣�����A��B�жԸñ���������������ͬ��

	exitSemaphore = CreateSemaphore(NULL, 0, 1, NULL);//�˳��ź���������
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
	is_Write_turn = false;

	m_Read_portal[0] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_portal[0].c_str());
	m_Write_portal[0] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_portal[0].c_str());
	m_Read_portal[1] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName_portal[1].c_str());
	m_Write_portal[1] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName_portal[1].c_str());
	InitializeCriticalSection(&critical_section_portal);
	InitializeConditionVariable(&condition_variable_portal);
	is_Write_turn_portal = 1;	//��_portal�߳���д���

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
	//3.�����ݷŵ������ڴ�
	while (true)
	{
		EnterCriticalSection(&critical_section);
		// �������д���ݵ��ִΣ���ȴ�
		while (!is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}


		//ͨ��m_pBuf[0]�����ڴ�m_hMapFile[0]��������
		WaitForSingleObject(m_Write[1], INFINITE);
		char szInfo[BUF_SIZE] = { 0 };
		gets_s(szInfo, BUF_SIZE);//c++11����gets_s() ����gets()
		memcpy(m_pBuf[1], szInfo, BUF_SIZE - 1);
		//����û�������������cls��������
		if (strcmp(szInfo, "cls") == 0)
		{
			system("cls");
		}
		ReleaseSemaphore(m_Read[1], 1, NULL);

		//�ж��û����������Ƿ�Ϊexit������ǣ������ź��˳�Shell����
		if (strcmp(szInfo, "exit") == 0)
		{
			ReleaseSemaphore(exitSemaphore, 1, NULL); // �����ź�����main����
			//break; // �˳�ѭ���������߳�
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
	// 3.�ӹ����ڴ��ж�ȡ����
	while (true)
	{
		EnterCriticalSection(&critical_section);
		// �������д���ݵ��ִΣ���ȴ�
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}

		WaitForSingleObject(m_Read[0], INFINITE); //m_Read[1]�ź����ɳ���B����
		string str = m_pBuf[0];
		Sleep(200);
		cout << m_pBuf[0];
		ReleaseSemaphore(m_Write[0], 1, NULL);
		is_Write_turn = true;
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
		if (str.find("����û�е�½���ᣬshell�����˳�") != string::npos)
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
	//3.�����ݷŵ������ڴ�
	while (true)
	{
		EnterCriticalSection(&critical_section_portal);
		// �������д���ݵ��ִΣ���ȴ�
		while (!is_Write_turn_portal)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}
		if (is_Write_turn_portal == 2)				//�ѽ��յ���Ч��id���˳����߳�
		{
			WakeConditionVariable(&condition_variable_portal);
			LeaveCriticalSection(&critical_section_portal);
			break;
		}
		//ͨ��m_pBuf[0]�����ڴ�m_hMapFile[0]��������
		WaitForSingleObject(m_Write_portal[1], INFINITE);
		//�����̵߳�Ψһ��ʶID���͸�Manager
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
	// 3.�ӹ����ڴ��ж�ȡ����
	while (true)
	{
		EnterCriticalSection(&critical_section_portal);
		// �������д���ݵ��ִΣ���ȴ�
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable_portal, &critical_section_portal, INFINITE);
		}
		WaitForSingleObject(m_Read_portal[0], INFINITE); //m_Read[1]�ź����ɳ���B����

		//���յ�Manager���ص��û�ͨ�ű�ʶ
		identification = atoi(m_pBuf_portal[0]);
		if (identification > 0 && identification < 10)		//���յ���Ч��id���ͷŶ�Ӧ���߳̽���ͨ�ţ��˳����߳�
		{
			ReleaseSemaphore(m_ThreadControl_Read[identification - 1], 1, NULL);
			ReleaseSemaphore(m_ThreadControl_Write[identification - 1], 1, NULL);

			ReleaseSemaphore(m_Write_portal[0], 1, NULL);
			is_Write_turn_portal = 2;						//��WriteSharedData_portal�̴߳����źţ�����Ҳ�˳�
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
	//3.�����ݷŵ������ڴ�
	while (true)
	{
		EnterCriticalSection(&critical_section_one);
		// �������д���ݵ��ִΣ���ȴ�
		while (!is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}

		//ͨ��m_pBuf[0]�����ڴ�m_hMapFile[0]��������
		WaitForSingleObject(m_Write_one[1], INFINITE);
		char szInfo_one[BUF_SIZE] = { 0 };
		gets_s(szInfo_one, BUF_SIZE);//c++11����gets_s() ����gets()
		memcpy(m_pBuf_one[1], szInfo_one, BUF_SIZE - 1);
		//����û�������������cls��������
		if (strcmp(szInfo_one, "cls") == 0)
		{
			system("cls");
		}
		ReleaseSemaphore(m_Read_one[1], 1, NULL);


		//�ж��û����������Ƿ�Ϊexit������ǣ������ź��˳�Shell����
		if (strcmp(szInfo_one, "exit") == 0)
		{
			ReleaseSemaphore(exitSemaphore, 1, NULL); // �����ź�����main����
			//break; // �˳�ѭ���������߳�
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
	// 3.�ӹ����ڴ��ж�ȡ����
	while (true)
	{
		EnterCriticalSection(&critical_section_one);
		// �������д���ݵ��ִΣ���ȴ�
		while (is_Write_turn_one)
		{
			SleepConditionVariableCS(&condition_variable_one, &critical_section_one, INFINITE);
		}
		WaitForSingleObject(m_Read_one[0], INFINITE); //m_Read[1]�ź����ɳ���B����

		string str = m_pBuf_one[0];
		Sleep(200);
		cout << m_pBuf_one[0];
		
		ReleaseSemaphore(m_Write_one[0], 1, NULL);
		is_Write_turn_one = true;
		WakeConditionVariable(&condition_variable_one);
		LeaveCriticalSection(&critical_section_one);
		counts++;
		if (str.find("����û�е�½���ᣬshell�����˳�") != string::npos)
		{
			Sleep(500);
			ReleaseSemaphore(exitSemaphore, 1, NULL);
		}
	}
	return true;
}
