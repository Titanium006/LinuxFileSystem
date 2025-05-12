/*
logon.cpp
登录
*/
#include"func.h"
#include"ipc.h"

bool cmd_logon() {
	unsigned int nCount = 3;
	while (nCount > 0)
	{
		nCount--;
		cout << "模拟Linux文件系统\n请先登录账号\n\n";
		sstream_toshell << "模拟Linux文件系统\n请先登录账号\n\n";
		char nUid[MAX_ID_LENGTH];
		char strPwd[MAX_PWD_LENGTH];
		//输入帐号
		cout << "账号：";
		sstream_toshell << "账号：";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		if (!cin_id(nUid, MAX_ID_LENGTH))
		{
			error(LOGON_FAILED);
			return false;
		}
		cout << endl;
		sstream_toshell << '\n';
		//输入密码，*处理
		cout << "密码：";
		sstream_toshell << "密码：";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		if (!cin_pwd(strPwd, MAX_PWD_LENGTH))
		{
			error(LOGON_FAILED);
			return false;
		}
		for (unsigned int i = 0; i < cur_Idsum ; i++)
		{
			// 比较密码是否正确
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
				cout << "登录成功！请稍候……\n";
				sstream_toshell << "登录成功！请稍候……\n";
				Sleep(800);
				return true;
			}
		}
		//最多 3 次密码错误
		if (nCount > 0)
		{
			logfail = true;
			cout << "\n\n******************************" << endl;
			sstream_toshell << "\n\n******************************\n";
			cout << "帐号不存在或密码错误！\n你还有" << nCount << "次重试机会。" << endl;
			sstream_toshell << "帐号不存在或密码错误！\n你还有" << nCount << "次重试机会。\n";
			Sleep(200);
			//system("cls");
		}
	}
	error(WRONG_PWD);
	logfail = false;
	return false;
}
// 密码输入的*处理
bool cin_pwd(char* strPwd, unsigned int nLen)
{
	cout << "Reading pwd!" << endl;
	char chGet = '\0';
	unsigned int i = 0;

	i = sstream_todisk.str().length();
	strcpy_s(strPwd, MAX_PWD_LENGTH,sstream_todisk.str().c_str());
	cout << "读到的密码是：" << strPwd << " 长度为："<< i << endl;
	return true; //返回成功
}

// 账号输入的处理
bool cin_id(char* strid, unsigned int nLen)
{
	char chGet = '\0';
	unsigned int i = 0;

	i = sstream_todisk.str().length();
	strcpy_s(strid, MAX_ID_LENGTH, sstream_todisk.str().c_str());
	strid[i] = '\0';
	cout << "读到的账户为：" << strid << " 长度为：" << i << endl;
	return true; //返回成功
}