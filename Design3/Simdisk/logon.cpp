/*
logon.cpp
��¼
*/
#include"func.h"
#include"ipc.h"

bool cmd_logon() {
	unsigned int nCount = 3;
	while (nCount > 0)
	{
		nCount--;
		cout << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
		sstream_toshell << "ģ��Linux�ļ�ϵͳ\n���ȵ�¼�˺�\n\n";
		char nUid[MAX_ID_LENGTH];
		char strPwd[MAX_PWD_LENGTH];
		//�����ʺ�
		cout << "�˺ţ�";
		sstream_toshell << "�˺ţ�";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		if (!cin_id(nUid, MAX_ID_LENGTH))
		{
			error(LOGON_FAILED);
			return false;
		}
		cout << endl;
		sstream_toshell << '\n';
		//�������룬*����
		cout << "���룺";
		sstream_toshell << "���룺";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		if (!cin_pwd(strPwd, MAX_PWD_LENGTH))
		{
			error(LOGON_FAILED);
			return false;
		}
		for (unsigned int i = 0; i < cur_Idsum ; i++)
		{
			// �Ƚ������Ƿ���ȷ
			//cout << systemUsers[i].get_user_id() << endl;
			//cout << systemUsers[i].UserPWD << endl;
			if (strcmp(nUid , systemUsers[i].get_user_id())==0
				&& systemUsers[i].cmp_pwd(strPwd))
			{
				memset(cur_user_Id, 0, MAX_ID_LENGTH);
				strcpy(cur_user_Id, nUid);
				cur_user_tType = systemUsers[i].get_user_type();
				cout << "\n\n******************************\n";
				sstream_toshell << "\n\n******************************\n";
				cout << "��¼�ɹ������Ժ򡭡�\n";
				sstream_toshell << "��¼�ɹ������Ժ򡭡�\n";
				Sleep(800);
				return true;
			}
		}
		//��� 3 ���������
		if (nCount > 0)
		{
			logfail = true;
			cout << "\n\n******************************" << endl;
			sstream_toshell << "\n\n******************************\n";
			cout << "�ʺŲ����ڻ��������\n�㻹��" << nCount << "�����Ի��ᡣ" << endl;
			sstream_toshell << "�ʺŲ����ڻ��������\n�㻹��" << nCount << "�����Ի��ᡣ\n";
			Sleep(200);
			//system("cls");
		}
	}
	error(WRONG_PWD);
	logfail = false;
	return false;
}
// ���������*����
bool cin_pwd(char* strPwd, unsigned int nLen)
{
	cout << "Reading pwd!" << endl;
	char chGet = '\0';
	unsigned int i = 0;

	i = sstream_todisk.str().length();
	strcpy_s(strPwd, MAX_PWD_LENGTH,sstream_todisk.str().c_str());
	cout << "�����������ǣ�" << strPwd << " ����Ϊ��"<< i << endl;
	return true; //���سɹ�
}

// �˺�����Ĵ���
bool cin_id(char* strid, unsigned int nLen)
{
	char chGet = '\0';
	unsigned int i = 0;

	i = sstream_todisk.str().length();
	strcpy_s(strid, MAX_ID_LENGTH, sstream_todisk.str().c_str());
	strid[i] = '\0';
	cout << "�������˻�Ϊ��" << strid << " ����Ϊ��" << i << endl;
	return true; //���سɹ�
}