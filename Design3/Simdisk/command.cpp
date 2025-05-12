/*
command.cpp
ϵͳ�����ʵ��
*/
#define _CRT_SECURE_NO_WARNINGS
#include"func.h"
#include"ipc.h"

//ϵͳִ��ָ��
void cmd_execute(const char* commondName, const char* strArgv1, const char* strArgv2) {
	if (strcmp(strArgv1, "/?") == 0) {
		if (strArgv2[0] != '\0') {
			error(TOO_MANY_ARGS);
		}
		else {
			cmd_help(commondName);
		}
		return;
	}
	int unsigned i;
	for (i = 0; i < COMMAND_COUNT; i++) {
		//cout << "\n-------------------\nҪ�Ƚϵ�ָ���ǣ�" << commondName << "\t��\t" << strCommands[i] << endl;
		if (strcmp(commondName, strCommands[i]) == 0)
			break;
	}

	switch (i)
	{
	case 0://info
		if (strArgv1[0] != '\0')
			error(TOO_MANY_ARGS);
		else
			cmd_info();
		break;
	case 1://cd
		if (strArgv2[0] != '\0')
			error(TOO_MANY_ARGS);
		else
			cmd_cd(strArgv1);
		break;
	case 2://dir
		if (strArgv2[0] != '\0')
			error(TOO_MANY_ARGS);
		else
			cmd_dir(strArgv1);
		break;
	case 3://md
		if (strArgv2[0] == '\0')
			if (strArgv1[0] == '\0')
				error(WRONG_ARGS);
			else cmd_md(strArgv1, READ_WRITE);
		else {
			file_attrib privilege;
			if (strcmp(strArgv2, "/rw") == 0) privilege = READ_WRITE;
			else if (strcmp(strArgv2, "/r") == 0)privilege = READ_ONLY;
			else if (strcmp(strArgv2, "/w") == 0)privilege = WRITE_ONLY;
			else if (strcmp(strArgv2, "/s") == 0)privilege = SHARE;
			else { error(WRONG_ARGS); return; }
			cmd_md(strArgv1, privilege);
		}
		break;
	case 4:	//rd
		if (strArgv1[0] == '\0' || strArgv2[0] != '\0')
		{
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_rd(strArgv1);
		break;
	case 5://newfile
		if (strArgv2[0] == '\0')
			if (strArgv1[0] == '\0')
			{
				error(WRONG_ARGS);
				WaitForSingleObject(errorSema, INFINITE);
				createError = true;
				WaitForSingleObject(hDSema, INFINITE);
				if (curShellNum == 0)
					hideDir_zero = false;
				else if (curShellNum == 1)
					hideDir_one = false;
				ReleaseSemaphore(hDSema, 1, NULL);
				ReleaseSemaphore(errorSema, 1, NULL);
			}
			else cmd_newfile(strArgv1, READ_WRITE);
		else {
			file_attrib privilege;
			if (strcmp(strArgv2, "/rw") == 0) privilege = READ_WRITE;
			else if (strcmp(strArgv2, "/r") == 0)privilege = READ_ONLY;
			else if (strcmp(strArgv2, "/w") == 0)privilege = WRITE_ONLY;
			else if (strcmp(strArgv2, "/s") == 0)privilege = SHARE;
			else {
				error(WRONG_ARGS);
				WaitForSingleObject(errorSema, INFINITE);
				createError = true;
				WaitForSingleObject(hDSema, INFINITE);
				if (curShellNum == 0)
					hideDir_zero = false;
				else if (curShellNum == 1)
					hideDir_one = false;
				ReleaseSemaphore(hDSema, 1, NULL);
				ReleaseSemaphore(errorSema, 1, NULL);
				return;
			}
			cmd_newfile(strArgv1, privilege);
		}
		break;
	case 6:	//cat
		if (strArgv1[0] == '\0' || strArgv2[0] != '\0')
		{
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_cat(strArgv1);
		break;
	case 7:	//write
		if (strArgv1[0] == '\0' || strArgv2[0] != '\0')
		{//�������һΪ�ջ��������Ϊ���򱨴��������
			error(WRONG_ARGS, commondName);
			WaitForSingleObject(errorSema, INFINITE);
			createError = true;
			WaitForSingleObject(hDSema, INFINITE);
			if (curShellNum == 0)
				hideDir_zero = false;
			else if (curShellNum == 1)
				hideDir_one = false;
			ReleaseSemaphore(hDSema, 1, NULL);
			ReleaseSemaphore(errorSema, 1, NULL);
			return;
		}
		cmd_write(strArgv1);
		break;
	case 8:	//copy
		if (strArgv2[0] == '\0')
		{
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_copy(strArgv1, strArgv2);
		break;
	case 9:	//del
		if (strArgv1[0] == '\0' || strArgv2[0] != '\0')
		{
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_del(strArgv1);
		break;
	case 10://check
		if (strArgv1[0] != '\0') {
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_check();
		break;
	case 11://init
	{
		if (strArgv1[0] != '\0') {
			error(WRONG_ARGS, commondName);
			return;
		}
		cout << "�Ƿ�ѡ���ʽ������ <Y/N> ? ";
		sstream_toshell << "�Ƿ�ѡ���ʽ������ <Y/N> ? ";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		char init_choice = '\0';
		string msg = sstream_todisk.str();
		init_choice = msg[0];
		if (init_choice == 'y' || init_choice == 'Y')
		{
			cmd_init();
			cout << "���̸�ʽ�����\n";
			sstream_toshell << "���̸�ʽ�����\n";
			Sleep(3000);
		}
		break;
	}
	case 12://cls
		if (strArgv1[0] != '\0') {
			error(WRONG_ARGS, commondName);
			return;
		}
		system("cls");
		break;
	case 13://help
		if (strArgv2[0] != '\0') {
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_help(strArgv1);
		break;
	case 14://logon
		if (strArgv1[0] != '\0')
		{
			error(WRONG_ARGS, commondName);
			return;
		}
		//system("cls");
		//system("title SimDisk");
		if (!cmd_logon())
		{
			sstream_toshell << "����û�е�½���ᣬshell�����˳�����\n";
			Sleep(400);
			WaitForSingleObject(exitSema, INFINITE);
			curShellCount--;
			ReleaseSemaphore(exitSema, 1, NULL);
			cmd_exit();
		}
		cin.sync();
		//system("cls");
		break;
	case 15://useradd
		if (strArgv1[0] == '\0') {
			error(WRONG_ARGS, commondName);
			WaitForSingleObject(errorSema, INFINITE);
			createError = true;
			WaitForSingleObject(hDSema, INFINITE);
			if (curShellNum == 0)
				hideDir_zero = false;
			else if (curShellNum == 1)
				hideDir_one = false;
			ReleaseSemaphore(hDSema, 1, NULL);
			ReleaseSemaphore(errorSema, 1, NULL);
			return;
		}
		if (strArgv2[0] == '\0')
			cmd_useradd(strArgv1, USER);
		else {
			if (strcmp(strArgv2, "/u") == 0)
				cmd_useradd(strArgv1, USER);
			else if (strcmp(strArgv2, "/a") == 0)
				cmd_useradd(strArgv1, ADMIN);
			else error(WRONG_ARGS, commondName);
		}
		break;
	case 16://userdel
		if (strArgv1[0] == '\0' || strArgv2[0] != '\0') {
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_userdel(strArgv1);
		break;
	case 17://exit
		if (strArgv1[0] != '\0') {
			error(WRONG_ARGS, commondName);
			return;
		}
		cmd_exit();
		break;
	default:
		error(INVILID_CMD);
		cout << "û�д�������������룡" << endl;
		sstream_toshell << "û�д�������������룡\n";
		break;
	}
}

/*
info:  ��ʾ����ϵͳ��Ϣ(�ο�Linux�ļ�ϵͳ��ϵͳ��Ϣ)���ļ����Ը����û����ж�д������
Ŀ¼�����ļ���֧��ȫ·���������·������·�������������á�/��������
*/
void cmd_info() {
	cout << "\tģ�� Linux �ļ�ϵͳ��Ϣ��\n\n";
	sstream_toshell << "\tģ�� Linux �ļ�ϵͳ��Ϣ��\n\n";
	cout << "������Ϣ��\n";
	sstream_toshell << "������Ϣ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "�û�������";
	sstream_toshell << "�û�������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << cur_Idsum;
	sstream_toshell << cur_Idsum;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "����û�������";
	sstream_toshell << "����û�������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << MAX_USER_NUM;
	sstream_toshell << MAX_USER_NUM;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "����������";
	sstream_toshell << "����������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCKS_NUM * BLOCK_SIZE;
	sstream_toshell << BLOCKS_NUM * BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " �ֽ�\t";
	sstream_toshell << " �ֽ�\t";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << (float)BLOCKS_NUM / 1024;
	sstream_toshell << (float)BLOCKS_NUM / 1024;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " MB\n";
	sstream_toshell << " MB\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "���ÿռ䣺";
	sstream_toshell << "���ÿռ䣺";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << (BLOCKS_NUM - sbGroups[0].allFreeBlocks) * BLOCK_SIZE;
	sstream_toshell << (BLOCKS_NUM - sbGroups[0].allFreeBlocks) * BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " �ֽ�\t";
	sstream_toshell << " �ֽ�\t";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << (float)(BLOCKS_NUM - sbGroups[0].allFreeBlocks) / 1024;
	sstream_toshell << (float)(BLOCKS_NUM - sbGroups[0].allFreeBlocks) / 1024;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " MB\n";
	sstream_toshell << " MB\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "���ÿռ䣺";
	sstream_toshell << "���ÿռ䣺";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << sbGroups[0].allFreeBlocks * BLOCK_SIZE;
	sstream_toshell << sbGroups[0].allFreeBlocks * BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " �ֽ�\t";
	sstream_toshell << " �ֽ�\t";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << (float)sbGroups[0].allFreeBlocks / 1024;
	sstream_toshell << (float)sbGroups[0].allFreeBlocks / 1024;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " MB\n";
	sstream_toshell << " MB\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "���ÿռ������";
	sstream_toshell << "���ÿռ������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << ((float)sbGroups[0].allFreeBlocks / (float)BLOCKS_NUM) * 100
		<< "%\n";
	sstream_toshell << ((float)sbGroups[0].allFreeBlocks / (float)BLOCKS_NUM) * 100
		<< "%\n";
	cout << "\n��ϸ��Ϣ��\n";
	sstream_toshell << "\n��ϸ��Ϣ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "�̿��С��";
	sstream_toshell << "�̿��С��";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCK_SIZE;
	sstream_toshell << BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " �ֽ�\n";
	sstream_toshell << " �ֽ�\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "ÿ���̿�����";
	sstream_toshell << "ÿ���̿�����";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCKS_EACH;
	sstream_toshell << BLOCKS_EACH;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "ÿ��i-�������";
	sstream_toshell << "ÿ��i-�������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << INODES_EACH;
	sstream_toshell << INODES_EACH;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "�̿�������";
	sstream_toshell << "�̿�������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCKS_NUM;
	sstream_toshell << BLOCKS_NUM;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "i-���������";
	sstream_toshell << "i-���������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << INODES_NUM;
	sstream_toshell << INODES_NUM;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "���п�������";
	sstream_toshell << "���п�������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << sbGroups[0].allFreeBlocks;
	sstream_toshell << sbGroups[0].allFreeBlocks;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "����i-���������";
	sstream_toshell << "����i-���������";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << sbGroups[0].allFreeInodes;
	sstream_toshell << sbGroups[0].allFreeInodes;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " ��\n";
	sstream_toshell << " ��\n";
}

void cmd_help(const char* strArg) {
	if (strArg[0] == '\0')//��ʾhelp�����Ŀ¼ϸ��
	{
		cout << "���������Ϣ��\n\n";
		sstream_toshell << "���������Ϣ��\n\n";
		cout << "info\t��ʾϵͳ��Ϣ��\n";
		sstream_toshell << "info\t��ʾϵͳ��Ϣ��\n";
		cout << "cd\t��ʾ��ǰĿ¼����ı䵱ǰĿ¼��\n";
		sstream_toshell << "cd\t��ʾ��ǰĿ¼����ı䵱ǰĿ¼��\n";
		cout << "dir\t��ʾĿ¼�е��ļ�����Ŀ¼�б�\n";
		sstream_toshell << "dir\t��ʾĿ¼�е��ļ�����Ŀ¼�б�\n";
		cout << "md\t����Ŀ¼��\n";
		sstream_toshell << "md\t����Ŀ¼��\n";
		cout << "rd\tɾ��Ŀ¼��\n";
		sstream_toshell << "rd\tɾ��Ŀ¼��\n";
		cout << "newfile\t�����ļ���\n";
		sstream_toshell << "newfile\t�����ļ���\n";
		cout << "cat\t��ʾ�ı��ļ������ݡ�\n";
		sstream_toshell << "cat\t��ʾ�ı��ļ������ݡ�\n";
		cout << "write\t�޸��ı��ļ������ݡ�\n";
		sstream_toshell << "write\t�޸��ı��ļ������ݡ�\n";
		cout << "copy\t��ָ���ļ����Ƶ���һ��λ�á�\n";
		sstream_toshell << "copy\t��ָ���ļ����Ƶ���һ��λ�á�\n";
		cout << "del\tɾ��ָ���ļ���\n";
		sstream_toshell << "del\tɾ��ָ���ļ���\n";
		cout << "check\t��Ⲣ�ָ��ļ�ϵͳ��\n";
		sstream_toshell << "check\t��Ⲣ�ָ��ļ�ϵͳ��\n";
		cout << "init\t��ʼ�������ļ�ϵͳ��\n";
		sstream_toshell << "init\t��ʼ�������ļ�ϵͳ��\n";
		cout << "cls\t�����Ļ���ݡ�\n";
		sstream_toshell << "cls\t�����Ļ���ݡ�\n";
		cout << "help\t�ṩ����İ�����Ϣ��\n";
		sstream_toshell << "help\t�ṩ����İ�����Ϣ��\n";
		cout << "logon\t���µ�¼��\n";
		sstream_toshell << "logon\t���µ�¼��\n";
		cout << "useradd\t������û���\n";
		sstream_toshell << "useradd\t������û���\n";
		cout << "userdel\tɾ���û���\n";
		sstream_toshell << "userdel\tɾ���û���\n";
		cout << "exit\t������Ĳ��˳�����\n\n";
		sstream_toshell << "exit\t������Ĳ��˳�����\n\n";
		cout << "\t�й�ĳ���������ϸ��Ϣ������� help ������������ ������ /?��\n";
		sstream_toshell << "\t�й�ĳ���������ϸ��Ϣ������� help ������������ ������ /?��\n";
		return;
	}
	int i = 0;
	for (i = 0; i < COMMAND_COUNT; i++)	// �Ƚ���������Ӧ���������
		if (strcmp(strCommands[i], strArg) == 0)	break;
	switch (i)
	{
	case 0:		//info
		cout << "info\n˵������ʾϵͳ��Ϣ��\n��ʽ��info";
		sstream_toshell << "info\n˵������ʾϵͳ��Ϣ��\n��ʽ��info";
		break;
	case 1:		//cd
		cout << "cd\n˵������ʾ��ǰĿ¼����ı䵱ǰĿ¼��\n��ʽ��cd [·��]";
		sstream_toshell << "cd\n˵������ʾ��ǰĿ¼����ı䵱ǰĿ¼��\n��ʽ��cd [·��]";
		cout << "\n���У�·������ʡ�Խ���ʾ��ǰ·����";
		sstream_toshell << "\n���У�·������ʡ�Խ���ʾ��ǰ·����";
		break;
	case 2:		//dir
		cout << "dir\n˵������ʾĿ¼�е��ļ�����Ŀ¼�б�\n��ʽ��dir [·��]";
		sstream_toshell << "dir\n˵������ʾĿ¼�е��ļ�����Ŀ¼�б�\n��ʽ��dir [·��]";
		break;
	case 3:		//md
		cout << "md\n˵��������Ŀ¼��\n��ʽ��md ·�� [����]";
		sstream_toshell << "md\n˵��������Ŀ¼��\n��ʽ��md ·�� [����]";
		cout << "\n���У�����ֵ�����������κ�ֵ֮һ��\n";
		sstream_toshell << "\n���У�����ֵ�����������κ�ֵ֮һ��\n";
		cout << "/a = ��д\t/r = ֻ��\n/w = ֻд\t/s = ����";
		sstream_toshell << "/a = ��д\t/r = ֻ��\n/w = ֻд\t/s = ����";
		cout << "\n���Բ���ʡ�Խ�Ĭ��Ϊ����д�����ԡ�";
		sstream_toshell << "\n���Բ���ʡ�Խ�Ĭ��Ϊ����д�����ԡ�";
		break;
	case 4:		//rd
		cout << "rd\n˵����ɾ��Ŀ¼��\n��ʽ��rd ·��";
		sstream_toshell << "rd\n˵����ɾ��Ŀ¼��\n��ʽ��rd ·��";
		break;
	case 5:		//newfile
		cout << "newfile\n˵���������ļ���\n��ʽ��newfile [·��]�ļ��� [����]";
		sstream_toshell << "newfile\n˵���������ļ���\n��ʽ��newfile [·��]�ļ��� [����]";
		cout << "\n���У�����ֵ�����������κ�ֵ֮һ��\n";
		sstream_toshell << "\n���У�����ֵ�����������κ�ֵ֮һ��\n";
		cout << "/rw = ��д\t/r = ֻ��\n/w = ֻд\t/s = ����";
		sstream_toshell << "/rw = ��д\t/r = ֻ��\n/w = ֻд\t/s = ����";
		cout << "\n���Բ���ʡ�Խ�Ĭ��Ϊ����д�����ԡ�";
		sstream_toshell << "\n���Բ���ʡ�Խ�Ĭ��Ϊ����д�����ԡ�";
		break;
	case 6:		//cat
		cout << "cat\n˵������ʾ�ı��ļ������ݡ�\n��ʽ��cat [·��]�ļ���";
		sstream_toshell << "cat\n˵������ʾ�ı��ļ������ݡ�\n��ʽ��cat [·��]�ļ���";
		break;
	case 7:		//write
		cout << "wirte\n˵�����޸��ı��ļ������ݡ�\n��ʽ��write [·��]�ļ���";
		sstream_toshell << "wirte\n˵�����޸��ı��ļ������ݡ�\n��ʽ��write [·��]�ļ���";
		break;
	case 8:		//copy
		cout << "copy\n˵������ָ���ļ����Ƶ���һ��λ�á�\n��ʽ��copy [<host>][��Դ·��]�ļ��� [<host>]Ŀ��·��";
		sstream_toshell << "copy\n˵������ָ���ļ����Ƶ���һ��λ�á�\n��ʽ��copy [<host>][��Դ·��]�ļ��� [<host>]Ŀ��·��";
		cout << "\n���У�<host> ����ѡ���ʾ·������Ϊ������·����\n��Դ·����Ŀ��·�������У�����ֻ����һ������ <host>��";
		sstream_toshell << "\n���У�<host> ����ѡ���ʾ·������Ϊ������·����\n��Դ·����Ŀ��·�������У�����ֻ����һ������ <host>��";
		cout << "\n�������ļ�·���в��ܴ��пո�";
		sstream_toshell << "\n�������ļ�·���в��ܴ��пո�";
		break;
	case 9:		//del
		cout << "del\n˵����ɾ��ָ���ļ���\n��ʽ��del [·��]�ļ���";
		sstream_toshell << "del\n˵����ɾ��ָ���ļ���\n��ʽ��del [·��]�ļ���";
		break;
	case 10:	//check
		cout << "check\n˵������Ⲣ�ָ��ļ�ϵͳ��\n��ʽ��check";
		sstream_toshell << "check\n˵������Ⲣ�ָ��ļ�ϵͳ��\n��ʽ��check";
		break;
	case 11:		//init
		cout << "init\n˵������ʼ�������ļ�ϵͳ��\n��ʽ��init";
		sstream_toshell << "init\n˵������ʼ�������ļ�ϵͳ��\n��ʽ��init";
		cout << "\n��ʼ�������� root/usr ϵͳĿ¼�����±����û��˻���Ϣ������ɾ����";
		sstream_toshell << "\n��ʼ�������� root/usr ϵͳĿ¼�����±����û��˻���Ϣ������ɾ����";
		cout << "\n* ��Ҫ����ԱȨ�� *";
		sstream_toshell << "\n* ��Ҫ����ԱȨ�� *";
		break;
	case 12:	//cls
		cout << "cls\n˵���������Ļ���ݡ�\n��ʽ��cls";
		sstream_toshell << "cls\n˵���������Ļ���ݡ�\n��ʽ��cls";
		break;
	case 13:	//help
		cout << "help\n˵�����ṩ����İ�����Ϣ��\n��ʽ��help [������] ���� ������ /?";
		sstream_toshell << "help\n˵�����ṩ����İ�����Ϣ��\n��ʽ��help [������] ���� ������ /?";
		cout << "\n���У�����������ʡ������ʾ���������б�";
		sstream_toshell << "\n���У�����������ʡ������ʾ���������б�";
		break;
	case 14:	//logon
		cout << "logon\n˵�������µ�¼��\n��ʽ��logon";
		sstream_toshell << "logon\n˵�������µ�¼��\n��ʽ��logon";
		break;
	case 15:	//useradd
		cout << "useradd\n˵����������û���\n��ʽ��useradd �˺� [�û�����]";
		sstream_toshell << "useradd\n˵����������û���\n��ʽ��useradd �˺� [�û�����]";
		cout << "\n���У��˺ű���Ϊ���֡�\n�û����Ϳ���Ϊ�����κ�ֵ֮һ��\n";
		sstream_toshell << "\n���У��˺ű���Ϊ���֡�\n�û����Ϳ���Ϊ�����κ�ֵ֮һ��\n";
		cout << "/u = ��ͨ�û�\t/a = ����Ա";
		sstream_toshell << "/u = ��ͨ�û�\t/a = ����Ա";
		cout << "\n�û����Ͳ���ʡ�Խ�Ĭ��Ϊ��ͨ�û���";
		sstream_toshell << "\n�û����Ͳ���ʡ�Խ�Ĭ��Ϊ��ͨ�û���";
		cout << "\n* ��Ҫ����ԱȨ�� *";
		sstream_toshell << "\n* ��Ҫ����ԱȨ�� *";
		break;
	case 16:	//userdel
		cout << "userdel\n˵����ɾ���û���\n��ʽ��userdel �˺�";
		sstream_toshell << "userdel\n˵����ɾ���û���\n��ʽ��userdel �˺�";
		cout << "\n���У��˺ű���Ϊ���֡�";
		sstream_toshell << "\n���У��˺ű���Ϊ���֡�";
		cout << "\n* ��Ҫ����ԱȨ�� *";
		sstream_toshell << "\n* ��Ҫ����ԱȨ�� *";
		break;
	case 17:	//exit
		cout << "exit\n˵����������Ĳ��˳�����\n��ʽ��exit";
		sstream_toshell << "exit\n˵����������Ĳ��˳�����\n��ʽ��exit";
		break;
	default:
		error(INVILID_CMD, strArg);
		return;
	}
	cout << "\n\nע�⣺\t��ʽ�д� [] ��Ϊ��ѡ�·��ʡ��Ĭ��Ϊ��ǰ·����\n";
	sstream_toshell << "\n\nע�⣺\t��ʽ�д� [] ��Ϊ��ѡ�·��ʡ��Ĭ��Ϊ��ǰ·����\n";
	cout << "\t֧�����·������.������ǰĿ¼����..������Ŀ¼��\n";
	sstream_toshell << "\t֧�����·������.������ǰĿ¼����..������Ŀ¼��\n";
}

/*
check�����Ӧ��������Ⲣ�ָ��ļ�ϵͳ�����ļ�ϵͳ�е�����һ���Խ��м�⣬���Զ������ļ�ϵͳ�Ľṹ����Ϣ��������������
*/
void cmd_check()
{
	int i, j;
	int nStart;				//��ʼ��ַ
	unsigned int nFreeBlockNum, nFreeInodeNum;			//���п��i-���
	unsigned int nFreeBlockAll = 0, nFreeInodeAll = 0;	//���п��п��i-���
	cout << "\n******************************\n";
	sstream_toshell << "\n******************************\n";
	cout << "���ڼ���ļ�ϵͳ����\n";
	sstream_toshell << "���ڼ���ļ�ϵͳ����\n";
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		nFreeBlockNum = 0;
		nFreeInodeNum = 0;
		nStart = i * BLOCKS_EACH;
		//������п��i-����ܺ�
		for (j = 0; j < BLOCKS_EACH; j++)
		{
			if (bsBlockBmp[nStart + j] == NOT_USED) nFreeBlockNum++;
			if (bsInodeBmp[nStart + j] == NOT_USED) nFreeInodeNum++;
		}
		//�������ʹ��̼�¼��ͬ���������쳣
		if (sbGroups[i].FreeBlocksNum != nFreeBlockNum)
		{
			sbGroups[i].FreeBlocksNum = nFreeBlockNum;
		}
		if (sbGroups[i].FreeInodesNum != nFreeInodeNum)
		{
			sbGroups[i].FreeInodesNum = nFreeInodeNum;
		}
		//�����ܺ�
		nFreeBlockAll += sbGroups[i].FreeBlocksNum;
		nFreeInodeAll += sbGroups[i].FreeInodesNum;
	}
	//�������ʹ��̼�¼��ͬ���������쳣
	if (sbGroups[0].allFreeBlocks != nFreeBlockAll)
	{
		for (i = 0; i < BLOCK_GROUPS_NUM; i++)
			sbGroups[i].allFreeBlocks = nFreeBlockAll;
	}
	if (sbGroups[0].allFreeInodes != nFreeInodeAll)
	{
		for (i = 0; i < BLOCK_GROUPS_NUM; i++)
			sbGroups[i].allFreeInodes = nFreeInodeAll;
	}

	//����Ķ�

	fs.open("Virtual_Disk.bin", ios::out | ios::binary | ios::_Nocreate);
	if (!fs.is_open()) error(WRITE_FAILED);
	for (int i = 0; i < BLOCK_GROUPS_NUM; i++)
		fs.write(reinterpret_cast<char*>(&sbGroups[i]), sizeof(SuperBlock));
	for (int i = 0; i < INODES_NUM; i++)
		fs.write(reinterpret_cast<char*>(&indTable[i]), sizeof(Inode));
	for (int i = 0; i < BLOCKS_NUM; i++)
		fs.write(reinterpret_cast<char*>(&bsBlockBmp[i]), sizeof(bmp_status));
	for (int i = 0; i < INODES_NUM; i++)
		fs.write(reinterpret_cast<char*>(&bsInodeBmp[i]), sizeof(bmp_status));


	fs.seekp(indTable[cur_dir.Inode_num].BlockAddr, ios::beg);
	fs.write(reinterpret_cast<char*>(&cur_dir), sizeof(Dir));
	fs.close();
	cout << "\n******************************\n";
	sstream_toshell << "\n******************************\n";
	cout << "�ļ�ϵͳ�޸���ɣ�\n";
	sstream_toshell << "�ļ�ϵͳ�޸���ɣ�\n";

}

void cmd_cls() {
	system("cls");
}

//useradd�����Ӧ������������û�
void cmd_useradd(const char* strUserId, user_type user_tType)
{
	//�����ǰ�û����Ͳ���ϵͳ���߹���Ա�򱨴�û��Ȩ�޴����˻�
	if (cur_user_tType != SYSTEM && cur_user_tType != ADMIN) {
		error(NO_PRIVILEGE, "user");
		WaitForSingleObject(errorSema, INFINITE);
		createError = true;
		WaitForSingleObject(hDSema, INFINITE);
		if (curShellNum == 0)
			hideDir_zero = false;
		else if (curShellNum == 1)
			hideDir_one = false;
		ReleaseSemaphore(hDSema, 1, NULL);
		ReleaseSemaphore(errorSema, 1, NULL);
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
				WaitForSingleObject(errorSema, INFINITE);
				createError = true;
				WaitForSingleObject(hDSema, INFINITE);
				if (curShellNum == 0)
					hideDir_zero = false;
				else if (curShellNum == 1)
					hideDir_one = false;
				ReleaseSemaphore(hDSema, 1, NULL);
				ReleaseSemaphore(errorSema, 1, NULL);
				return;
			}
		}
		//��ʼ��
		char password[MAX_PWD_LENGTH];
		cout << "����(�20λ)��";
		sstream_toshell << "����(�20λ)��";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		cin_pwd(password, MAX_PWD_LENGTH);
		stringstream ssStream;
		ssStream << password << " " << (int)user_tType;
		char strBuffer[MAX_PWD_LENGTH + 5];//�����ַ����������������û����������
		strcpy(strBuffer, ssStream.str().c_str());
		ssStream.clear();
		//��ʽ����ɣ�д���ļ�
		Dir dirUsr;
		analyse_path("/user", dirUsr);
		char* tem = new char[MAX_NAME_LENGTH];
		strcpy(tem, strUserId);
		save_file(tem, strBuffer, MAX_PWD_LENGTH + 5, dirUsr, PROTECTED);
		//���һ���û����ڴ�
		cur_Idsum++;
		systemUsers[cur_Idsum - 1].set_user_info(strUserId, password, user_tType);
	}
	else
	{
		error(USERS_MAX);
	}
}

//userdel��ɾ���û�
void cmd_userdel(const char* strUserId) {
	if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM) {
		error(NO_PRIVILEGE_USERDEL);
		return;
	}
	if (strcmp(strUserId, cur_user_Id) == 0) {
		error(PROTECTED_USER);
		return;
	}
	//�����Ƿ����ͬ���˺�
	for (unsigned int i = 0; i < cur_Idsum; i++)
	{
		if (strcmp(systemUsers[i].get_user_id(), strUserId) == 0)
		{
			char* path = new char[MAX_NAME_LENGTH];
			strcpy(path, "root/user/");
			strcat(path, systemUsers[i].get_user_id());

			Dir dirTemp;
			char strDirName[MAX_NAME_LENGTH];
			if (analyse_path(path, dirTemp, strDirName)) {//·���Ϸ�
				//�ж��û�Ȩ��
				if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE &&
					strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
				{
					error(NO_PRIVILEGE, path);
					cout << endl;
					sstream_toshell << '\n';
					error(DEL_FAILED);
					return;
				}
				unsigned int nIndex;//�ļ�i�ڵ��
				if (!dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
				{
					error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
					cout << endl;
					sstream_toshell << '\n';
					error(CAT_FAILED);
					return;
				}
				else {
					if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[nIndex].fileAttr != SHARE &&
						strcmp(indTable[nIndex].UserID, cur_user_Id) != 0)
					{
						error(NO_PRIVILEGE, path);
						cout << endl;
						sstream_toshell << '\n';
						error(DEL_FAILED);
						return;
					}
					delete_file(dirTemp, nIndex);
				}
			}
			else {
				error(PATH_NOT_FOUND);
				cout << endl;
				sstream_toshell << '\n';
				error(DEL_FAILED);
				return;
			}

			cout << "�˺�ɾ���ɹ���" << endl;
			sstream_toshell << "�˺�ɾ���ɹ���\n";
			get_user_info();
			return;
		}
	}
	error(USER_NOT_EXIST, strUserId);
	return;

}

//�˳�����
void cmd_exit()
{
	WaitForSingleObject(exitSema, INFINITE);
	if (curShellCount == 0)
	{
		ReleaseSemaphore(exitSema, 1, NULL);
		system("cls");
		sstream_toshell << "exit";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		Sleep(1000);
		exit(0);
	}
	else
	{
		ReleaseSemaphore(exitSema, 1, NULL);
	}
}

//��Ŀ¼���ļ���صĲ���ָ��塪��

/*
cd ��:  �ı�Ŀ¼���ı䵱ǰ����Ŀ¼��Ŀ¼������ʱ����������Ϣ��
*/
void cmd_cd(const char* strArgv) {
	if (strArgv[0] == '\0') {
		return;
	}
	Dir dirTemp;
	if (analyse_path(strArgv, dirTemp)) {//·���Ϸ�
		//�ж��û�Ȩ��
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			indTable[dirTemp.Inode_num].fileAttr != SHARE &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0) {
			error(NO_PRIVILEGE);
			cout << endl;
			sstream_toshell << '\n';
			error(CD_FAILED);
			return;
		}
		if (indTable[dirTemp.Inode_num].fileAttr == WRITE_ONLY)
		{
			error(DIR_WRITEONLY, indTable[dirTemp.Inode_num].fileName);
			cout << endl;
			sstream_toshell << '\n';
			error(CD_FAILED);
			return;
		}
		cur_dir = dirTemp;
		set_cur_path(cur_dir);
	}
	else {
		error(PATH_NOT_FOUND);
		cout << endl;
		sstream_toshell << '\n';
		error(CD_FAILED);
		return;
	}
}


/*
dir ��:  ��ʾĿ¼����ʾָ��Ŀ¼�»�ǰĿ¼�µ���Ϣ��
�����ļ����������ַ�������롢�ļ����ȡ���Ŀ¼��
����/s������dir�����ʾ������Ŀ¼����
*/
void cmd_dir(const char* strPath) {

	Dir dirTemp;
	if (strPath[0] == '\0') {
		dirTemp = cur_dir;
	}
	else if (analyse_path(strPath, dirTemp)) {//·���Ϸ�
		//�ж��û�Ȩ��
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			indTable[dirTemp.Inode_num].fileAttr != SHARE &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0) {
			error(NO_PRIVILEGE);
			cout << endl;
			sstream_toshell << '\n';
			error(DIR_FAILED);
			return;
		}
		if (indTable[dirTemp.Inode_num].fileAttr == WRITE_ONLY)
		{
			error(DIR_WRITEONLY, indTable[dirTemp.Inode_num].fileName);
			cout << endl;
			sstream_toshell << '\n';
			error(DIR_FAILED);
			return;
		}

	}
	else {
		error(PATH_NOT_FOUND);
		cout << endl;
		sstream_toshell << '\n';
		error(DIR_FAILED);
		return;
	}
	//��ӡĿ¼����Ϣ
	dir_info(dirTemp.Inode_num);
}

//��ʾĿ¼�µ������Ϣ
void dir_info(unsigned int InodeNum) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	cout << indTable[InodeNum].fileName << " ��Ŀ¼\n\n";
	sstream_toshell << indTable[InodeNum].fileName << " ��Ŀ¼\n\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	cout.width(20);
	cout << left << "Ŀ¼��/�ļ���";
	sstream_toshell << "Ŀ¼��/�ļ���\t";
	cout.width(20);
	cout << left << "�޸�����";
	sstream_toshell << "\t�޸�����";
	cout.width(4);
	cout << "����";
	sstream_toshell << "\t����";
	cout.width(18);
	cout << right << "��С���ֽڣ�";
	sstream_toshell << "\t\t��С���ֽڣ�";
	cout.width(15);
	cout << right << "�����ַ";
	sstream_toshell << "\t�����ַ";
	cout.width(15);
	cout << right << "������";
	sstream_toshell << "\t������";
	cout.width(15);
	cout << right << "����";
	sstream_toshell << "\t\t����";
	cout << endl;
	sstream_toshell << '\n';

	Dir tem_dir;
	fs.open("Virtual_Disk.bin", ios::in | ios::binary);
	if (!fs.is_open()) error(READ_FAILED);
	else {
		fs.seekg(indTable[InodeNum].BlockAddr, ios::beg);
		fs.read(reinterpret_cast<char*>(&tem_dir), sizeof(Dir));
		for (unsigned int i = 0; i < tem_dir.child_num; i++) {
			//cout << tem_dir.SubInode_num[i] << endl;
			Inode tem_inode = indTable[tem_dir.SubInode_num[i]];

			cout << left << (i == 0 ? "." : (i == 1 ? ".." : indTable[tem_dir.SubInode_num[i]].fileName)) << "\t";
			sstream_toshell << left << (i == 0 ? "." : (i == 1 ? ".." : indTable[tem_dir.SubInode_num[i]].fileName)) << "\t";
			tm tmCreatedTime = tem_inode.fileMfyTime.get_time();
			cout.fill('0');
			cout.width(4);
			cout << "\t" << right << tmCreatedTime.tm_year + 1900;
			sstream_toshell << "\t" << tmCreatedTime.tm_year + 1900;
			cout << "/";
			sstream_toshell << "/";
			cout.width(2);
			cout << tmCreatedTime.tm_mon + 1;
			sstream_toshell << tmCreatedTime.tm_mon + 1;
			cout << "/";
			sstream_toshell << "/";
			cout.width(2);
			cout << tmCreatedTime.tm_mday;
			sstream_toshell << tmCreatedTime.tm_mday;
			cout << " ";
			sstream_toshell << " ";
			cout.width(2);
			cout << tmCreatedTime.tm_hour;
			sstream_toshell << tmCreatedTime.tm_hour;
			cout << ":";
			sstream_toshell << ":";
			cout.width(2);
			cout << tmCreatedTime.tm_min;
			sstream_toshell << tmCreatedTime.tm_min;
			cout << ":";
			sstream_toshell << ":";
			cout.width(2);
			cout << tmCreatedTime.tm_sec;
			sstream_toshell << tmCreatedTime.tm_sec;
			cout << " ";
			sstream_toshell << " ";
			cout.fill('\0');
			cout.width(4);
			cout << "\t" << (tem_inode.fileType == TYPE_DIR ? "Ŀ¼" : "�ļ�");
			sstream_toshell << "\t" << (tem_inode.fileType == TYPE_DIR ? "Ŀ¼" : "�ļ�");
			cout.width(18);
			cout << "\t\t" << right << (tem_inode.fileType == TYPE_DIR ? " " : to_string(tem_inode.fileSize));
			sstream_toshell << "\t\t" << (tem_inode.fileType == TYPE_DIR ? " " : to_string(tem_inode.fileSize));
			cout.width(15);
			cout << "\t\t" << right << tem_inode.BlockAddr;
			sstream_toshell << "\t\t" << tem_inode.BlockAddr;
			cout.width(15);
			cout << "\t" << right << tem_inode.UserID;
			sstream_toshell << "\t" << tem_inode.UserID;
			file_attrib tem_at = tem_inode.fileAttr;
			string attr;
			switch (tem_at) {
			case PROTECTED: attr = "ϵͳ"; break;
			case READ_WRITE:attr = "��д"; break;
			case READ_ONLY:attr = "ֻ��"; break;
			case WRITE_ONLY:attr = "ֻд"; break;
			case SHARE:attr = "����"; break;
			}
			cout.width(15);
			cout << "\t\t" << right << attr;
			sstream_toshell << "\t\t" << attr;
			cout << endl;
			sstream_toshell << '\n';
		}
		fs.close();
	}
}

/*
md ��:  ����Ŀ¼����ָ��·����ǰ·���´���ָ��Ŀ¼������ʱ��������Ϣ��
*/
void cmd_md(const char* strPath, file_attrib privilege) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//·���Ϸ�
		//�ж��û�Ȩ��			if (strcmp(tem_dirName, "user") == 0 && strcmp(indTable[dirTemp.Inode_num].fileName, "root"))return false;
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			indTable[dirTemp.Inode_num].fileAttr != SHARE &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0 || (strcmp(indTable[dirTemp.SubInode_num[1]].fileName, "root") == 0 && strcmp(indTable[dirTemp.Inode_num].fileName, "user") == 0))
		{//�����ǰ�û����ǹ���Ա��ϵͳ��·������root�ҵ�ǰ�û�id��i-�ڵ����λ�û���Ϣ��ͬ�򱨴��޷���Ȩ����Ŀ¼����ʧ��
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
			return;
		}
		if (indTable[dirTemp.Inode_num].fileAttr == READ_ONLY)
		{//����ǰ�ļ�Ϊֻ�����͵��ļ����ܴ���Ŀ¼�����£��򱨴��ļ�ֻ����Ŀ¼����ʧ��
			error(DIR_READONLY, indTable[dirTemp.Inode_num].fileName);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
			return;
		}
		long Addr = -1;
		int nInode = -1;
		unsigned int nIndex;
		//�Ƿ��Ѵ��ڸ����ֵ������������򱨴��ļ��Ѵ�����Ŀ¼����ʧ��
		if (dirTemp.have_child(strDirName, nIndex, TYPE_DIR))
		{
			error(FILE_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
			return;
		}
		//����Ŀ¼�Ĵ洢�ռ�
		Addr = alloc_blocks(DIR_SIZE, nIndex);
		if (Addr < 0)	//�ռ䲻���򱨴���̿ռ䲻���Ҵ���Ŀ¼ʧ��
		{
			error(SPACE_NOT_ENOUGH);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
		}
		else
		{
			//����i-���
			nInode = alloc_inodes();
			//���i-�ڵ�λͼ�������޷�����i-�ڵ�ͱ���i--�ڵ����ʧ����Ŀ¼����ʧ��
			if (nInode < 0)
			{
				error(INODE_ALLOC_FAILED);
				cout << endl;
				sstream_toshell << '\n';
				error(MD_FAILED);
				return;
			}
			//��ʼ����Ŀ¼�����������Ӧ��Ϣ
			Dir dirNew;//����һ���µ�Ŀ¼����
			dirNew.Inode_num = (unsigned int)nInode;//����i-�ڵ�
			dirNew.child_num = 2;//���ļ�+��Ŀ¼����Ϊ2
			dirNew.SubInode_num[0] = (unsigned int)nInode;//��������Ŀi-�ڵ�1
			dirNew.SubInode_num[1] = dirTemp.Inode_num;//��������Ŀi-�ڵ�2
			strcpy(indTable[dirNew.Inode_num].fileName, strDirName);//���ô�Ŀ¼��Ӧ���ļ���
			strcpy(indTable[dirNew.Inode_num].UserID, cur_user_Id);//���ô�Ŀ¼�������û���
			indTable[nInode].fileType = TYPE_DIR;//��i-�ڵ���������ļ�����ΪĿ¼
			indTable[nInode].fileAttr = privilege;//��i-�ڵ���������ļ�����Ϊ���������
			indTable[nInode].fileSize = sizeof(Dir);//��i-�ڵ���������ļ���СΪĿ¼�ļ���С
			indTable[nInode].BlockAddr = Addr;//��i-�ڵ��������Ŀ¼���̿���ʼ��ַ
			indTable[nInode].havingBlocks = DIR_SIZE;//��i-�ڵ��������ռ�ô��̿�����СΪĿ¼�ߴ�
			time_t lTime;
			time(&lTime);
			tm tmCreatedTime = *localtime(&lTime);//��¼Ŀ¼����ʱ��
			indTable[nInode].fileMfyTime.set_time(tmCreatedTime);//��i - �ڵ�������õ�ǰĿ¼����ʱ��
			//�ڸ�Ŀ¼�������Ϣ
			dirTemp.SubInode_num[dirTemp.child_num] = (unsigned int)nInode;//����Ŀ¼���뵽��Ŀ¼���ӽڵ���
			dirTemp.child_num++;
			if (dirTemp.Inode_num == cur_dir.Inode_num)
			{
				cur_dir = dirTemp;
				if (!creatingDisk && !creatingUser)
				{
					// ��������shell�洢��Ŀ¼
					WaitForSingleObject(tuSema, INFINITE);
					for (int i = 0; i < userCount; i++)
					{
						ThreadID_UserID[i].user_dir.SubInode_num[ThreadID_UserID[i].user_dir.child_num] = (unsigned int)nInode;
						ThreadID_UserID[i].user_dir.child_num++;
					}
					ReleaseSemaphore(tuSema, 1, NULL);
				}
			}
			//����
			fs.open("Virtual_Disk.bin", ios::out | ios::binary | ios::_Nocreate);
			if (!fs.is_open()) error(WRITE_FAILED);
			int i;
			for (i = 0; i < BLOCK_GROUPS_NUM; i++) {
				fs.write(reinterpret_cast<char*>(&sbGroups[i]), sizeof(SuperBlock));
				fs.flush();
			}
			fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * nInode), ios::beg);
			fs.write(reinterpret_cast<char*>(&indTable[nInode]), sizeof(Inode));
			fs.flush();
			fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * nIndex), ios::beg);
			for (i = 0; i < DIR_SIZE; i++) {
				fs.write(reinterpret_cast<char*>(&bsBlockBmp[nIndex + i]), sizeof(bmp_status));
				fs.flush();
			}
			fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * BLOCKS_NUM + sizeof(bmp_status) * nInode), ios::beg);
			fs.write(reinterpret_cast<char*>(&bsInodeBmp[nInode]), sizeof(bmp_status));
			fs.flush();
			fs.seekp(Addr, ios::beg);
			fs.write(reinterpret_cast<char*>(&dirNew), sizeof(Dir));
			fs.flush();
			fs.seekp(indTable[dirTemp.Inode_num].BlockAddr, ios::beg);
			fs.write(reinterpret_cast<char*>(&dirTemp), sizeof(Dir));
			fs.flush();
			fs.close();
		}
	}

	else {//�����޷��ҵ�·����Ŀ¼����ʧ��
		error(PATH_NOT_FOUND);
		cout << endl;
		error(MD_FAILED);
		return;
	}
}

/*
rd ��:  ɾ��Ŀ¼��ɾ��ָ��Ŀ¼�������ļ�����Ŀ¼��ҪɾĿ¼����ʱ��Ҫ������ʾ�Ƿ�Ҫɾ����
*/
void cmd_rd(const char* strPath) {
	Dir dirTemp;
	Dir dirRemove;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//·���Ϸ�
		if (strcmp(strDirName, ".") == 0 || strcmp(strDirName, "..") == 0) {
			error(PATH_NOT_FOUND, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(RD_FAILED);
			return;
		}
		//�ж��û�Ȩ��
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
		{
			error(NO_PRIVILEGE, strPath);
			sstream_toshell << '\n';
			cout << endl;
			error(RD_FAILED);
			return;
		}

		if (indTable[dirTemp.Inode_num].fileAttr == READ_ONLY)
		{
			error(DIR_READONLY, indTable[dirTemp.Inode_num].fileName);
			cout << endl;
			sstream_toshell << '\n';
			error(RD_FAILED);
			return;
		}
		unsigned int nIndex;
		//�ж�Ҫɾ����Ŀ¼�Ƿ����
		if (!dirTemp.have_child(strDirName, nIndex, TYPE_DIR)) {
			error(DIR_NOT_EXIST, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(RD_FAILED);
			return;
		}
		else {
			if (indTable[nIndex].fileType != TYPE_DIR) {
				error(DIR_NOT_EXIST, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(RD_FAILED);
			}
			if (nIndex == cur_dir.Inode_num) {
				error(NO_DEL_CUR);
				return;
			}
			if (isAncestorOfcur_dir(nIndex)) {
				error(DIR_NOT_EXIST, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(RD_FAILED);
			}
			if (indTable[nIndex].fileAttr == READ_ONLY)
			{
				error(DIR_READONLY, indTable[nIndex].fileName);
				cout << endl;
				sstream_toshell << '\n';
				error(RD_FAILED);
				return;
			}
			fs.open("Virtual_Disk.bin", ios::in | ios::binary);
			if (!fs.is_open())error(READ_FAILED);
			else {
				fs.seekg(indTable[nIndex].BlockAddr, ios::beg);
				fs.read(reinterpret_cast<char*>(&dirRemove), sizeof(Dir));
				fs.close();
				remove_dir(dirTemp, dirRemove);
				cout << "ɾ��Ŀ¼���!" << endl;
				sstream_toshell << "ɾ��Ŀ¼���!\n";
			}
		}
	}
	else {
		error(PATH_NOT_FOUND);
		cout << endl;
		sstream_toshell << '\n';
		error(RD_FAILED);
	}

}

//ɾ�����ļ� ��Ŀ¼dirRemove��ɾ���ڵ��ΪnIndex���ļ�����Ҫ����ǰĿ¼���child_num�Լ��������λͼ�޸Ĳ�д���������
void delete_file(Dir& dirRemove, unsigned int nIndex) {
	release_blocks(nIndex);
	release_inodes(nIndex);

	int len = dirRemove.child_num;//Ŀ¼�������
	for (int i = 0; i < len; i++) {
		if (dirRemove.SubInode_num[i] == nIndex) {
			for (int j = i; j < len - 1; j++)
				dirRemove.SubInode_num[j] = dirRemove.SubInode_num[j + 1];
			break;
		}
	}
	dirRemove.child_num--;
	if (cur_dir.Inode_num == dirRemove.Inode_num)
	{
		cur_dir = dirRemove;
		// ��������shell�洢��Ŀ¼
		WaitForSingleObject(tuSema, INFINITE);
		for (int t = 0; t < userCount; t++)
		{
			for (int i = 0; i < len; i++) {
				if (ThreadID_UserID[t].user_dir.SubInode_num[i] == nIndex) {
					for (int j = i; j < len - 1; j++)
					{
						ThreadID_UserID[t].user_dir.SubInode_num[j] = ThreadID_UserID[t].user_dir.SubInode_num[j + 1];
					}
					break;
				}
			}
			ThreadID_UserID[t].user_dir.child_num--;
		}
		ReleaseSemaphore(tuSema, 1, NULL);
	}
	fs.open("Virtual_Disk.bin", ios::out | ios::binary | ios::_Nocreate);
	if (!fs.is_open()) error(WRITE_FAILED);
	int i;
	for (i = 0; i < BLOCK_GROUPS_NUM; i++) {
		fs.write(reinterpret_cast<char*>(&sbGroups[i]), sizeof(SuperBlock));
		fs.flush();
	}
	unsigned int startblock = (indTable[nIndex].BlockAddr - indTable[0].BlockAddr) / BLOCK_SIZE;
	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * startblock), ios::beg);
	for (i = 0; i < indTable[nIndex].havingBlocks; i++) {
		fs.write(reinterpret_cast<char*>(&bsBlockBmp[startblock + i]), sizeof(bmp_status));
		fs.flush();
	}
	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * BLOCKS_NUM + sizeof(bmp_status) * nIndex), ios::beg);
	fs.write(reinterpret_cast<char*>(&bsInodeBmp[nIndex]), sizeof(bmp_status));
	fs.flush();
	fs.seekp(indTable[dirRemove.Inode_num].BlockAddr, ios::beg);
	fs.write(reinterpret_cast<char*>(&dirRemove), sizeof(Dir));
	fs.flush();
	fs.close();

}

//ɾ����Ŀ¼
void remove_dir(Dir& dirTemp, Dir& dirRemove) {
	unsigned int dirTempInode = dirTemp.Inode_num, dirRemoveInode = dirRemove.Inode_num;
	release_blocks(dirRemoveInode);
	release_inodes(dirRemoveInode);

	int len = dirTemp.child_num;//Ŀ¼�������
	for (int i = 0; i < len; i++) {
		if (dirTemp.SubInode_num[i] == dirRemoveInode) {
			for (int j = i; j < len - 1; j++)
				dirTemp.SubInode_num[j] = dirTemp.SubInode_num[j + 1];
			break;
		}
	}
	dirTemp.child_num--;
	if (cur_dir.Inode_num == dirTemp.Inode_num)
	{
		cur_dir = dirTemp;
		// ��������shell�洢��Ŀ¼
		WaitForSingleObject(tuSema, INFINITE);
		for (int t = 0; t < userCount; t++)
		{
			for (int i = 0; i < len; i++) {
				if (ThreadID_UserID[t].user_dir.SubInode_num[i] == dirRemoveInode) {
					for (int j = i; j < len - 1; j++)
					{
						ThreadID_UserID[t].user_dir.SubInode_num[j] = ThreadID_UserID[t].user_dir.SubInode_num[j + 1];
					}
					break;
				}
			}
			ThreadID_UserID[t].user_dir.child_num--;
		}
		ReleaseSemaphore(tuSema, 1, NULL);
	}
	fs.open("Virtual_Disk.bin", ios::out | ios::binary | ios::_Nocreate);
	if (!fs.is_open()) error(WRITE_FAILED);
	int i;
	for (i = 0; i < BLOCK_GROUPS_NUM; i++) {
		fs.write(reinterpret_cast<char*>(&sbGroups[i]), sizeof(SuperBlock));
		fs.flush();
	}
	unsigned int startblock = (indTable[dirRemoveInode].BlockAddr - indTable[0].BlockAddr) / BLOCK_SIZE;
	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * startblock), ios::beg);
	for (i = 0; i < indTable[dirRemoveInode].havingBlocks; i++) {
		fs.write(reinterpret_cast<char*>(&bsBlockBmp[startblock + i]), sizeof(bmp_status));
		fs.flush();
	}
	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * BLOCKS_NUM + sizeof(bmp_status) * dirRemoveInode), ios::beg);
	fs.write(reinterpret_cast<char*>(&bsInodeBmp[dirRemoveInode]), sizeof(bmp_status));
	fs.flush();
	fs.seekp(indTable[dirTempInode].BlockAddr, ios::beg);
	fs.write(reinterpret_cast<char*>(&dirTemp), sizeof(Dir));
	fs.flush();
	fs.close();
}

/*
newfile ��:  �����ļ���
*/
void cmd_newfile(const char* strPath, file_attrib privilege) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//·���Ϸ�
		//�ж��û�Ȩ��
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			indTable[dirTemp.Inode_num].fileAttr != SHARE &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
		{//�����ǰ�û�״̬���ǹ���Ա��ϵͳ����Ŀ¼�ļ�������root���ļ����Բ��ǹ�����û�idƥ�䲻���򱨴�����Ȩ�����޷��������ļ�
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			error(NEW_FILE_FAILED);
			WaitForSingleObject(errorSema, INFINITE);
			createError = true;
			WaitForSingleObject(hDSema, INFINITE);
			if (curShellNum == 0)
				hideDir_zero = false;
			else if (curShellNum == 1)
				hideDir_one = false;
			ReleaseSemaphore(hDSema, 1, NULL);
			ReleaseSemaphore(errorSema, 1, NULL);
			return;
		}
		if (indTable[dirTemp.Inode_num].fileAttr == READ_ONLY)
		{//�����ǰ�ļ�����Ϊֻ���򱨴��ļ�ֻ�����޷��������ļ�
			error(DIR_READONLY, indTable[dirTemp.Inode_num].fileName);
			cout << endl;
			error(NEW_FILE_FAILED);
			WaitForSingleObject(errorSema, INFINITE);
			createError = true;
			WaitForSingleObject(hDSema, INFINITE);
			if (curShellNum == 0)
				hideDir_zero = false;
			else if (curShellNum == 1)
				hideDir_one = false;
			ReleaseSemaphore(hDSema, 1, NULL);
			ReleaseSemaphore(errorSema, 1, NULL);
			return;
		}

		unsigned int nIndex;//���ص��ļ�����
		if (dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
		{//�жϵ�ǰĿ¼���Ƿ���ͬ�����ļ���������򱨴��ļ��Ѵ������޷��������ļ�
			error(FILE_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
			cout << endl;
			error(NEW_FILE_FAILED);
			WaitForSingleObject(errorSema, INFINITE);
			createError = true;
			WaitForSingleObject(hDSema, INFINITE);
			if (curShellNum == 0)
				hideDir_zero = false;
			else if (curShellNum == 1)
				hideDir_one = false;
			ReleaseSemaphore(hDSema, 1, NULL);
			ReleaseSemaphore(errorSema, 1, NULL);
			return;
		}
		/*unsigned long strBufferSize = 10;
		char* strBuffer = new char[strBufferSize];
		user_input(strBuffer, strBufferSize);//�û�����
		save_file(strDirName, strBuffer, strBufferSize, dirTemp, privilege);
		*/
		// �޸� newfile �����е����²��֣�

		unsigned long strBufferSize = 200;
		char* strBuffer = new char[strBufferSize + 1];  // ����һλ������ַ����Ľ�β��־ '\0'
		user_input(strBuffer, strBufferSize);  // ���� user_input ����ʱ�����ݵĳ��Ȳ���Ϊ strBufferSize
		strBuffer[strBufferSize] = '\0';  // ��ĩβ����ַ����Ľ�β��־
		save_file(strDirName, strBuffer, strlen(strBuffer), dirTemp, privilege);  // ͨ�� strlen ��ȡʵ���ַ����ĳ���

	}

	else {//���·���޷������򱨴��޷��ҵ�·�������ļ�����ʧ��
		error(PATH_NOT_FOUND);
		cout << endl;
		error(NEW_FILE_FAILED);
		WaitForSingleObject(errorSema, INFINITE);
		createError = true;
		WaitForSingleObject(hDSema, INFINITE);
		if (curShellNum == 0)
			hideDir_zero = false;
		else if (curShellNum == 1)
			hideDir_one = false;
		ReleaseSemaphore(hDSema, 1, NULL);
		ReleaseSemaphore(errorSema, 1, NULL);
		return;
	}
}

//�洢�ļ��ĺ���
void save_file(char* strFileName, char* strBuffer, unsigned int strBufferSize, Dir dirTemp, file_attrib privilege) {
	long Addr = -1; // ��������ݿ��׵�ַ
	int nInode = -1; // �����i�ڵ��
	unsigned int nIndex = 0;
	bool bExist = false; // �ж��ļ��Ƿ��Ѵ���

	for (int i = 0; i < dirTemp.child_num; i++) { // ��������
		if (strcmp(indTable[dirTemp.SubInode_num[i]].fileName, strFileName) == 0) { // ����ҵ�ͬ���ļ�
			nInode = dirTemp.SubInode_num[i]; // ֱ��ʹ��ԭ�е�i�ڵ��
			bExist = true;
			break;
		}
	}

	if (!bExist) { // �ļ������ڣ�����з���ռ�ʹ���i�ڵ�
		// ����Ŀ¼�Ĵ洢�ռ�
		// ����洢�ļ����ݵ����ݿ飬����ȡ���ݿ����ʼ��ַ������
		Addr = alloc_blocks((strBufferSize * sizeof(char)) / BLOCK_SIZE + 1, nIndex);
		if (Addr < 0) { // �ռ䲻��
			error(SPACE_NOT_ENOUGH);
			cout << endl;
			error(NEW_FILE_FAILED);
		}
		else {
			// ����i-���
			nInode = alloc_inodes();
			if (nInode < 0) {
				error(INODE_ALLOC_FAILED);
				cout << endl;
				error(NEW_FILE_FAILED);
				return;
			}
			// ��ʼ����i�ڵ���Ϣ�����������Ӧ��Ϣ
			strcpy(indTable[nInode].fileName, strFileName); // �����ļ���
			strcpy(indTable[nInode].UserID, cur_user_Id); // �����û�ID
			indTable[nInode].fileType = TYPE_FILE; // �����ļ�����Ϊ��ͨ�ļ�
			indTable[nInode].fileAttr = privilege; // �����ļ����ԣ�Ȩ�ޣ�
			indTable[nInode].fileSize = strBufferSize * sizeof(char); // �����ļ���С
			indTable[nInode].BlockAddr = Addr; // �������ݿ��ַ
			indTable[nInode].havingBlocks = (strBufferSize * sizeof(char)) / BLOCK_SIZE + 1; // �������ݿ�����

			time_t lTime;
			time(&lTime);
			tm tmCreatedTime = *localtime(&lTime);
			indTable[nInode].fileMfyTime.set_time(tmCreatedTime); // �����ļ����޸�ʱ��

			// �ڸ�Ŀ¼�������Ϣ
			dirTemp.SubInode_num[dirTemp.child_num] = (unsigned int)nInode; // ��¼�ӽڵ��i�ڵ��
			dirTemp.child_num++;

			if (dirTemp.Inode_num == cur_dir.Inode_num)
			{
				cur_dir = dirTemp; // ���µ�ǰĿ¼
				if (!creatingDisk)
				{
					// ��������shell�洢��Ŀ¼
					WaitForSingleObject(tuSema, INFINITE);
					for (int i = 0; i < userCount; i++)
					{
						ThreadID_UserID[i].user_dir.SubInode_num[ThreadID_UserID[i].user_dir.child_num] = (unsigned int)nInode;
						ThreadID_UserID[i].user_dir.child_num++;
					}
					ReleaseSemaphore(tuSema, 1, NULL);
				}
			}
		}
	}

	// ���浽������
	fs.open("Virtual_Disk.bin", ios::out | ios::binary | ios::_Nocreate); // ����������ļ�
	if (!fs.is_open()) error(WRITE_FAILED); // ��ʧ���򱨴�

	int i;
	for (i = 0; i < BLOCK_GROUPS_NUM; i++) {
		fs.write(reinterpret_cast<char*>(&sbGroups[i]), sizeof(SuperBlock)); // д�볬����
		fs.flush();
	}

	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * nInode), ios::beg);
	fs.write(reinterpret_cast<char*>(&indTable[nInode]), sizeof(Inode)); // д����ļ���i�ڵ���Ϣ
	fs.flush();

	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * nIndex), ios::beg);
	for (i = 0; i < indTable[nInode].havingBlocks; i++) {
		fs.write(reinterpret_cast<char*>(&bsBlockBmp[nIndex + i]), sizeof(bmp_status)); // д�����ݿ�λͼ
		fs.flush();
	}

	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * BLOCKS_NUM + sizeof(bmp_status) * nInode), ios::beg);
	fs.write(reinterpret_cast<char*>(&bsInodeBmp[nInode]), sizeof(bmp_status)); // д��i�ڵ�λͼ
	fs.flush();

	fs.seekp(indTable[nInode].BlockAddr, ios::beg);
	fs.write(reinterpret_cast<char*>(strBuffer), indTable[nInode].fileSize); // д���ļ�����
	fs.flush();

	fs.seekp(indTable[dirTemp.Inode_num].BlockAddr, ios::beg);
	fs.write(reinterpret_cast<char*>(&dirTemp), sizeof(Dir)); // ���¸�Ŀ¼�е���Ϣ
	fs.flush();

	fs.close(); // �ر���������ļ�
}

//�û������ļ���Ϣ����
void user_input(char* strBuffer, unsigned long& strBufferSize) {
	char user_cin = '\0';//�û������ַ�
	unsigned long nowBufferSize = 0;//��ȡ��ǰ�������Ĵ�С
	for (int i = 0; i < strBufferSize; i++)strBuffer[i] = ' ';//��ʼ���ַ����飺�ں�����ʼʱ����strBuffer��ָ����ַ����������Ԫ�ض�����Ϊ' '�����ո�
	cout << "\n�������ļ����ݣ���$����" << endl;
	sstream_toshell << "\n�������ļ����ݣ���$����" << endl;
	ReleaseSemaphore(mutexSim[1], 1, NULL);
	WaitForSingleObject(mutexSim[0], INFINITE);
	unsigned int i = 0;
	i = sstream_todisk.str().length();
	char strmessage[200];
	strcpy_s(strmessage, 200, sstream_todisk.str().c_str());
	//strid[i] = '\0';
	while (nowBufferSize < i && strmessage[nowBufferSize] != '$')
	{
		strBuffer[nowBufferSize] = strmessage[nowBufferSize];
		nowBufferSize++;
		if (nowBufferSize + 1 >= strBufferSize) {
			char* tem = new char[strBufferSize];
			strcpy(tem, strBuffer);
			delete[]strBuffer;
			strBufferSize = strBufferSize * 2;
			strBuffer = new char[strBufferSize];
			for (int j = 0; j < strBufferSize; j++) strBuffer[j] = ' ';
			strcpy(strBuffer, tem);
			delete[]tem;
		}
	}
	return;
}

/*
cat ��:  ���ļ���
*/
void cmd_cat(const char* strPath) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//·���Ϸ�
		//�ж��û�Ȩ��
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
		{//�����ǰ�û����Ͳ��ǹ���Ա��ϵͳ���ļ����Բ��ǹ������û������ļ����Բ��ϵĻ��ͱ���û��Ȩ�����޷���
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(CAT_FAILED);
			return;
		}
		unsigned int nIndex;//�ļ�i�ڵ��
		if (!dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
		{//�����ǰĿ¼��û������ļ��򱨴��ļ����������޷���
			error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
			cout << endl;
			sstream_toshell << '\n';
			error(CAT_FAILED);
			return;
		}
		else {

			if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[nIndex].fileAttr != SHARE &&
				strcmp(indTable[nIndex].UserID, cur_user_Id) != 0)
			{//�����ǰ�û����Ͳ��ǹ���Ա��ϵͳ���ļ����Բ��ǹ������û����Բ��ϵĻ��ͱ���û��Ȩ�����޷���
				error(NO_PRIVILEGE, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(CAT_FAILED);
				return;
			}

			if (indTable[dirTemp.Inode_num].fileAttr == WRITE_ONLY)
			{//�����ǰ�ļ�����Ϊֻд�򱨴��ļ�ֻд�޷���
				error(FILE_WRITEONLY, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(CAT_FAILED);
				return;
			}
			char* strBuffer = new char[indTable[nIndex].fileSize];
			open_file(nIndex, strBuffer);
			cout << "�ļ����ݣ�" << endl;
			sstream_toshell << "�ļ����ݣ�\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
			wchar_t* wStrBuffer = new wchar_t[indTable[nIndex].fileSize * 2];  // Ϊ�洢 wchar_t ���͵��������ռ䣬����2����Ϊÿ���ַ����ܻ�ռ�������ֽ�
			MultiByteToWideChar(CP_UTF8, 0, strBuffer, -1, wStrBuffer, indTable[nIndex].fileSize * 2);  // �� strBuffer ת��Ϊ wchar_t ���͵�����
			int i = 0;
			// ��wchar_t*���͵�����ת��Ϊstd::string����
			wstring_convert<codecvt_utf8<wchar_t>> converter;
			while (wStrBuffer[i] != L'\0') {  // �ж��ַ�����ֹ����
				if (iswprint(wStrBuffer[i])) {  // �ж��ַ��Ƿ�ɴ�ӡ��ʹ�� iswprint �����ж� wchar_t ���͵��ַ�
					//wcout << wStrBuffer[i];
					string str = converter.to_bytes(wStrBuffer[i]);
					cout << str;
					sstream_toshell << str;
				}
				if (wStrBuffer[i] == L'\n') {  // �ж��Ƿ�Ϊ���з������������������س���"\r\n"
					cout << "\r\n";
					sstream_toshell << "\r\n";
				}
				i++;
			}
			delete[] wStrBuffer;  // �ͷŶ�̬������ڴ�
			cout << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
	else {//������޷�������·���򱨴�·�������޷���
		error(PATH_NOT_FOUND);
		cout << endl;
		error(CAT_FAILED);
		return;
	}
}

/*
write ��:  �޸��ļ���
*/
void cmd_write(const char* strPath) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//·���Ϸ�
		//�ж��û�Ȩ��
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
		{//�����ǰ�û����Ͳ��ǹ���Ա��ϵͳ���ļ����Բ��ǹ������û������ļ����Բ��ϵĻ��ͱ���û��Ȩ�����޷���
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(WRITE_FAILED);
			WaitForSingleObject(errorSema, INFINITE);
			createError = true;
			WaitForSingleObject(hDSema, INFINITE);
			if (curShellNum == 0)
				hideDir_zero = false;
			else if (curShellNum == 1)
				hideDir_one = false;
			ReleaseSemaphore(hDSema, 1, NULL);
			ReleaseSemaphore(errorSema, 1, NULL);
			return;
		}
		unsigned int nIndex = 0;//�ļ�i�ڵ��
		if (!dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
		{//�����ǰĿ¼��û������ļ��򱨴��ļ����������޷���
			error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
			cout << endl;
			sstream_toshell << '\n';
			error(WRITE_FAILED);
			WaitForSingleObject(errorSema, INFINITE);
			createError = true;
			WaitForSingleObject(hDSema, INFINITE);
			if (curShellNum == 0)
				hideDir_zero = false;
			else if (curShellNum == 1)
				hideDir_one = false;
			ReleaseSemaphore(hDSema, 1, NULL);
			ReleaseSemaphore(errorSema, 1, NULL);
			return;
		}
		else {

			if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[nIndex].fileAttr != SHARE &&
				strcmp(indTable[nIndex].UserID, cur_user_Id) != 0)
			{//�����ǰ�û����Ͳ��ǹ���Ա��ϵͳ���ļ����Բ��ǹ������û����Բ��ϵĻ��ͱ���û��Ȩ�����޷���
				error(NO_PRIVILEGE, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(WRITE_FAILED);
				WaitForSingleObject(errorSema, INFINITE);
				createError = true;
				WaitForSingleObject(hDSema, INFINITE);
				if (curShellNum == 0)
					hideDir_zero = false;
				else if (curShellNum == 1)
					hideDir_one = false;
				ReleaseSemaphore(hDSema, 1, NULL);
				ReleaseSemaphore(errorSema, 1, NULL);
				return;
			}

			if (indTable[dirTemp.Inode_num].fileAttr == READ_ONLY)
			{//�����ǰ�ļ�����Ϊֻ���򱨴��ļ�ֻ���޷���
				error(FILE_READONLY, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(WRITE_FAILED);
				WaitForSingleObject(errorSema, INFINITE);
				createError = true;
				WaitForSingleObject(hDSema, INFINITE);
				if (curShellNum == 0)
					hideDir_zero = false;
				else if (curShellNum == 1)
					hideDir_one = false;
				ReleaseSemaphore(hDSema, 1, NULL);
				ReleaseSemaphore(errorSema, 1, NULL);
				return;
			}

			char* strBuffer = new char[indTable[nIndex].fileSize + 1];  // ����һλ������ַ����Ľ�β��־ '\0'
			open_file(nIndex, strBuffer);

			cout << "��ǰ�ļ����ݣ�" << endl;
			sstream_toshell << "��ǰ�ļ����ݣ�\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
			wchar_t* wStrBuffer = new wchar_t[indTable[nIndex].fileSize * 2];  // �洢 wchar_t ���͵����飬����2����Ϊÿ���ַ����ܻ�ռ�������ֽ�
			MultiByteToWideChar(CP_UTF8, 0, strBuffer, -1, wStrBuffer, indTable[nIndex].fileSize * 2);  // �� strBuffer ת��Ϊ wchar_t ���͵�����
			int i = 0;
			// ��wchar_t*���͵�����ת��Ϊstd::string����
			wstring_convert<codecvt_utf8<wchar_t>> converter;
			while (wStrBuffer[i] != L'\0') {  // �ж��ַ�����ֹ����
				if (iswprint(wStrBuffer[i])) {  // �ж��ַ��Ƿ�ɴ�ӡ��ʹ�� iswprint �����ж� wchar_t ���͵��ַ�
					//wcout << wStrBuffer[i];
					string str = converter.to_bytes(wStrBuffer[i]);
					cout << str;
					sstream_toshell << str;
				}
				if (wStrBuffer[i] == L'\n') {  // �ж��Ƿ�Ϊ���з������������������س���"\r\n"
					cout << "\r\n";
					sstream_toshell << "\r\n";
				}
				i++;
			}
			delete[] wStrBuffer;  // �ͷŶ�̬������ڴ�
			cout << endl;
			sstream_toshell << "\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			cout << "�������µ��ļ����ݣ�" << endl;
			sstream_toshell << "�������µ��ļ����ݣ�\n";
			unsigned long strBufferSize = 200;
			char* strBuffer1 = new char[strBufferSize + 1];  // ����һλ������ַ����Ľ�β��־ '\0'
			user_input(strBuffer1, strBufferSize);  // ���� user_input ����ʱ�����ݵĳ��Ȳ���Ϊ strBufferSize
			strBuffer1[strBufferSize] = '\0';  // ��ĩβ����ַ����Ľ�β��־
			save_file(strDirName, strBuffer1, strlen(strBuffer1), dirTemp, indTable[dirTemp.Inode_num].fileAttr);  // �����ļ�

			delete[] strBuffer;  // �ͷŶ�̬������ڴ�
			delete[] strBuffer1;  // �ͷŶ�̬������ڴ�
		}
	}
	else {//������޷�������·���򱨴�·�������޷���
		error(PATH_NOT_FOUND);
		cout << endl;
		sstream_toshell << '\n';
		error(WRITE_FAILED);
		WaitForSingleObject(errorSema, INFINITE);
		createError = true;
		WaitForSingleObject(hDSema, INFINITE);
		if (curShellNum == 0)
			hideDir_zero = false;
		else if (curShellNum == 1)
			hideDir_one = false;
		ReleaseSemaphore(hDSema, 1, NULL);
		ReleaseSemaphore(errorSema, 1, NULL);
		return;
	}
}

long open_file(unsigned int nInode, char* strBuffer)
{
	//��ȡָ����ַ
	fs.open("Virtual_Disk.bin", ios::in | ios::binary);
	if (!fs.is_open())error(READ_FAILED);
	else {
		fs.seekg((indTable[nInode].BlockAddr), ios::beg);
		fs.read(reinterpret_cast<char*>(strBuffer), indTable[nInode].fileSize);
		fs.close();
	}
	return indTable[nInode].fileSize;
}
/*
del ��:  ɾ���ļ���ɾ��ָ���ļ���������ʱ����������Ϣ��
*/
void cmd_del(const char* strPath) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//·���Ϸ�
		//�ж��û�Ȩ��
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0 || (strcmp(indTable[dirTemp.Inode_num].fileName, "user") == 0 && strcmp(indTable[dirTemp.SubInode_num[1]].fileName, "root") == 0))
		{
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(DEL_FAILED);
			return;
		}
		unsigned int nIndex;//�ļ�i�ڵ��
		if (!dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
		{
			error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
			cout << endl;
			sstream_toshell << '\n';
			error(CAT_FAILED);
			return;
		}
		else {
			if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[nIndex].fileAttr != SHARE &&
				strcmp(indTable[nIndex].UserID, cur_user_Id) != 0)
			{
				error(NO_PRIVILEGE, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(DEL_FAILED);
				return;
			}
			delete_file(dirTemp, nIndex);
		}
	}
	else {
		error(PATH_NOT_FOUND);
		cout << endl;
		sstream_toshell << '\n';
		error(DEL_FAILED);
		return;
	}
}

/*
copy ��:  �����ļ�����֧��ģ��Linux�ļ�ϵͳ�ڲ����ļ������⣬��֧��host�ļ�ϵͳ��ģ��Linux�ļ�ϵͳ����ļ�������host�ļ�ϵͳ���ļ�����Ϊ<host>��,
�磺��windows��D���̵��ļ�\data\sample\test.txt�ļ�������ģ��Linux�ļ�ϵͳ�е�/test/dataĿ¼��windows��D���̵ĵ�ǰĿ¼ΪD��\data����ʹ�����
simdisk copy <host>D��\data\sample\test.txt /test/data
���ߣ�simdisk copy <host>D��sample\test.txt /test/data
*/
void cmd_copy(const char* strSrcPath, const char* strDesPath) {
	char strFileName[MAX_NAME_LENGTH] = { "" };
	char* strBuffer;
	Dir dirTemp;
	char strDiv;
	long nLen = 0;
	size_t nSrcLen = strlen(strSrcPath);
	size_t nDesLen = strlen(strDesPath);
	//����·���������޸�
	char* strSrcFinalPath = new char[nSrcLen];
	char* strDesFinalPath = new char[nDesLen];
	strcpy(strSrcFinalPath, strSrcPath);
	strcpy(strDesFinalPath, strDesPath);
	//�� if_host_path �ж���Դ·���������Ƿ���� <host>
	if (isHost(strSrcFinalPath))	//host �ļ����Ƶ�ģ�������
	{
		if (isHost(strDesFinalPath))
		{
			error(NOT_BOTH_HOST);
			return;
		}
		//��ȡ�������ļ�
		fstream fsHostIn;
		fsHostIn.open(strSrcFinalPath, ios::in | ios::binary);
		if (!fsHostIn.is_open())
		{
			error(HOST_FILE_NOT_EXIST, strSrcFinalPath);
			return;
		}
		//�����ļ�����
		fsHostIn.seekg(0, ios::end);
		nLen = fsHostIn.tellg();
		//����洢�ռ�
		strBuffer = new char[nLen + 1];
		strBuffer[nLen] = 0;
		fsHostIn.seekg(0, ios::beg);
		fsHostIn.read(reinterpret_cast<char*>(strBuffer), nLen);
		fsHostIn.close();
		//��ȡ�ļ���
		strDiv = '\\';
		cout << strrchr(strSrcFinalPath, strDiv) << endl;
		strcpy(strFileName, strrchr(strSrcFinalPath, strDiv) + 1);
		//����Ŀ��·��

		if (analyse_path(strDesPath, dirTemp))
		{
			//�ж��û�Ȩ��
			if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
				indTable[dirTemp.Inode_num].fileAttr != SHARE &&
				strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
			{
				error(NO_PRIVILEGE, strDesFinalPath);
				delete(strBuffer);
				return;
			}
			if (indTable[dirTemp.Inode_num].fileAttr == READ_ONLY)
			{
				error(DIR_READONLY, strDesFinalPath);
				delete(strBuffer);
				return;
			}
			//�ж�Ŀ¼�Ƿ��Ѵ���ͬ������

			unsigned int nIndex;
			if (dirTemp.have_child(strFileName, nIndex, TYPE_FILE))
			{
				delete(strBuffer);
				error(FILE_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
				return;
			}
			//���浽����
			save_file(strFileName, strBuffer, nLen, dirTemp, READ_WRITE);
			delete(strBuffer);
			cout << "�ļ�������ɣ�\n";
			sstream_toshell << "�ļ�������ɣ�\n";
		}
		else
		{
			error(COPY_FAILED);
		}
	}
	else	//��һ������������ <host>
	{
		if (isHost(strDesFinalPath))	//ģ������ļ����Ƶ� host ��
		{
			//�ָ�·�����õ��ļ���
			if (analyse_path(strSrcFinalPath, dirTemp, strFileName))
			{
				unsigned int nIndex;
				if (!dirTemp.have_child(strFileName, nIndex, TYPE_FILE))
				{//������
					error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
					cout << endl;
					sstream_toshell << '\n';
					error(COPY_FAILED);
					return;
				}
				else
				{
					//�ж��û�Ȩ��
					if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
						indTable[dirTemp.Inode_num].fileAttr != SHARE &&
						strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0 || (strcmp(indTable[dirTemp.Inode_num].fileName, "user") == 0 && strcmp(indTable[dirTemp.SubInode_num[1]].fileName, "root") == 0))
					{
						error(NO_PRIVILEGE, strSrcFinalPath);
						return;
					}
					if (indTable[nIndex].fileAttr == WRITE_ONLY)
					{
						error(FILE_WRITEONLY, strSrcFinalPath);
						return;
					}
					//��ȡ�ļ����ڴ�
					strBuffer = new char[indTable[nIndex].fileSize];
					nLen = open_file(nIndex, strBuffer);
					//�ϲ�Ϊ������ȫ·��
					char* strFullPath = new char[nSrcLen + nDesLen + 2];
					stringstream ssStream;
					ssStream << strDesFinalPath;
					if (strDesFinalPath[nDesLen - 1] != '\\') ssStream << "\\";
					ssStream << strFileName;
					ssStream >> strFullPath;
					ssStream.clear();
					//д���ļ���������ϵͳ
					fstream fsHostOut;
					fsHostOut.open(strFullPath, ios::out | ios::binary);
					if (!fsHostOut.is_open())
					{
						error(HOST_FILE_WRITE_FAILED, strFullPath);
						delete(strBuffer);
						delete(strFullPath);
						return;
					}
					fsHostOut.write(reinterpret_cast<char*>(strBuffer), nLen);
					fsHostOut.close();
					delete(strFullPath);
					delete(strBuffer);
					cout << "�ļ�������ɣ�\n";
					sstream_toshell << "�ļ�������ɣ�\n";
				}
			}
			else
			{
				error(COPY_FAILED);
			}
		}
		else	//ģ��������ļ�����
		{
			//�ָ�·�����õ��ļ���
			if (analyse_path(strSrcFinalPath, dirTemp, strFileName))
			{
				unsigned int nIndex;
				if (!dirTemp.have_child(strFileName, nIndex, TYPE_FILE))
				{//������
					error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
					cout << endl;
					error(COPY_FAILED);
					return;
				}
				else	//�ҵ�
				{
					//�ж��û�Ȩ��
					if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM &&
						indTable[dirTemp.Inode_num].fileAttr != SHARE &&
						strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
					{
						error(NO_PRIVILEGE, strSrcFinalPath);
						return;
					}
					if (indTable[nIndex].fileAttr == FILE_WRITEONLY)
					{
						error(FILE_WRITEONLY, strSrcFinalPath);
						return;
					}
					file_attrib faReadWrite = indTable[nIndex].fileAttr;
					strBuffer = new char[indTable[nIndex].fileSize];
					nLen = open_file(nIndex, strBuffer);
					//�ϲ�Ϊģ�����ȫ·��
					char* strFullPath = new char[nSrcLen + nDesLen + 2];
					stringstream ssStream;
					ssStream << strDesFinalPath;
					if (strDesFinalPath[nDesLen - 1] != '/') ssStream << "/";
					ssStream << strFileName;
					ssStream >> strFullPath;
					ssStream.clear();
					//�ָ�Ŀ��·�����õ��ļ���
					if (analyse_path(strFullPath, dirTemp, strFileName))
					{
						//�ж��û�Ȩ��
						if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM &&
							indTable[dirTemp.Inode_num].fileAttr != SHARE &&
							strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
						{
							error(NO_PRIVILEGE, strDesFinalPath);
							delete(strBuffer);
							delete(strFullPath);
							return;
						}
						if (indTable[dirTemp.Inode_num].fileAttr == READ_ONLY)
						{
							error(DIR_READONLY, strDesFinalPath);
							delete(strBuffer);
							delete(strFullPath);
							return;
						}
						//�ж�Ŀ¼�Ƿ��Ѵ���ͬ������
						unsigned int nindex;
						if (dirTemp.have_child(strFileName, nindex, TYPE_FILE))
						{
							delete(strBuffer);
							delete(strFullPath);
							error(FILE_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
							return;
						}
						//�����ļ�
						save_file(strFileName, strBuffer, nLen, dirTemp, faReadWrite);
						cout << "�ļ�������ɣ�\n";
						sstream_toshell << "�ļ�������ɣ�\n";
					}
					else
					{
						error(COPY_FAILED);
					}
					delete(strBuffer);
					delete(strFullPath);
				}
			}
			else
			{
				error(COPY_FAILED);
			}
		}
	}
}

//�ж��Ƿ�����·����������·��
bool isHost(char* strPath) {
	char* strDiv;
	strDiv = strstr(strPath, "<host>");
	if (strDiv == strPath)
	{
		strcpy(strPath, strDiv + 6);
		return true;
	}
	return false;
}