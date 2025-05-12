#include"ipc.h"
void CloseFileMap()
{
	for (int i = 0; i < 2; i++)
	{
		//4.�����ļ���ͼUnMapViewOfFile()
		UnmapViewOfFile(m_pBuf[i]);
		//5.�ر�ӳ���ļ����CloseHandle()
		CloseHandle(m_hMapFile[i]);
	}
	CloseHandle(exitSemaphore);
}
void CreateFileMap()
{
	m_cstrSzName[0] = L"NameOfMappingObject0";//�����ڴ�0�����֣�����A��B�жԸñ���������������ͬ��
	m_cstrSzName[1] = L"NameOfMappingObject1";//�����ڴ�1�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName[0] = L"ReadSemaphoreName0";//���ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_ReadSemaphoreName[1] = L"ReadSemaphoreName1";//���ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName[0] = L"WriteSemaphoreName0";//д�ź���0�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteSemaphoreName[1] = L"WriteSemaphoreName1";//д�ź���1�����֣�����A��B�жԸñ���������������ͬ��
	m_WriteBeforeRead = L"WriteBeforeReadSemaphore";//д����ź���������
	exitSemaphore = CreateSemaphore(NULL, 0, 1, L"ExitSemaphore");//�˳��ź���������
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
	}
	m_Read[0] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName[0].c_str());
	m_Write[0] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName[0].c_str());
	m_Read[1] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)m_ReadSemaphoreName[1].c_str());
	m_Write[1] = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteSemaphoreName[1].c_str());
	writeBeforeRead = CreateSemaphore(NULL, 1, 1, (LPCWSTR)m_WriteBeforeRead.c_str());

	InitializeCriticalSection(&critical_section);

	InitializeConditionVariable(&condition_variable);
}
unsigned int __stdcall WriteSharedData(void* pPM)
{
	//3.�����ݷŵ������ڴ�
	while (true)
	{
		//�����ٽ���
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
		is_Write_turn = false;
		//�ͷ������������뿪�ٽ���
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
		//�ж��û����������Ƿ�Ϊexit������ǣ������ź��˳�Shell����
		if (strcmp(szInfo, "exit") == 0)
		{
			ReleaseSemaphore(exitSemaphore, 1, NULL); // �����ź�����main����
			break; // �˳�ѭ���������߳�
		}
	}
	return true;
}
unsigned int __stdcall ReadSharedData(void* pPM)
{
	int counts = 0;
	// 3.�ӹ����ڴ��ж�ȡ����
	while (true)
	{
		//�����ٽ���
		EnterCriticalSection(&critical_section);
		// �����д���ݵ��ִΣ���ȴ�
		while (is_Write_turn)
		{
			SleepConditionVariableCS(&condition_variable, &critical_section, INFINITE);
		}
		WaitForSingleObject(m_Read[0], INFINITE); //m_Read[1]�ź����ɳ���B����
		//if (counts < 2)
		//{
		//	cout << "begin to write!" << endl;
		//}
		cout << m_pBuf[0];
		ReleaseSemaphore(m_Write[0], 1, NULL);
		is_Write_turn = true;
		//�ͷ������������뿪�ٽ���
		WakeConditionVariable(&condition_variable);
		LeaveCriticalSection(&critical_section);
		counts++;
	}
	return true;
}
