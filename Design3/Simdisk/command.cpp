/*
command.cpp
系统命令的实现
*/
#define _CRT_SECURE_NO_WARNINGS
#include"func.h"
#include"ipc.h"

//系统执行指令
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
		//cout << "\n-------------------\n要比较的指令是：" << commondName << "\t和\t" << strCommands[i] << endl;
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
		{//如果参数一为空或参数二不为空则报错参数错误
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
		cout << "是否选择格式化磁盘 <Y/N> ? ";
		sstream_toshell << "是否选择格式化磁盘 <Y/N> ? ";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		char init_choice = '\0';
		string msg = sstream_todisk.str();
		init_choice = msg[0];
		if (init_choice == 'y' || init_choice == 'Y')
		{
			cmd_init();
			cout << "磁盘格式化完成\n";
			sstream_toshell << "磁盘格式化完成\n";
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
			sstream_toshell << "您已没有登陆机会，shell程序将退出！！\n";
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
		cout << "没有此命令！请重新输入！" << endl;
		sstream_toshell << "没有此命令！请重新输入！\n";
		break;
	}
}

/*
info:  显示整个系统信息(参考Linux文件系统的系统信息)，文件可以根据用户进行读写保护。
目录名和文件名支持全路径名和相对路径名，路径名各分量间用“/”隔开。
*/
void cmd_info() {
	cout << "\t模拟 Linux 文件系统信息：\n\n";
	sstream_toshell << "\t模拟 Linux 文件系统信息：\n\n";
	cout << "整体信息：\n";
	sstream_toshell << "整体信息：\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "用户数量：";
	sstream_toshell << "用户数量：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << cur_Idsum;
	sstream_toshell << cur_Idsum;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 个\n";
	sstream_toshell << " 个\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "最大用户数量：";
	sstream_toshell << "最大用户数量：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << MAX_USER_NUM;
	sstream_toshell << MAX_USER_NUM;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 个\n";
	sstream_toshell << " 个\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "磁盘容量：";
	sstream_toshell << "磁盘容量：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCKS_NUM * BLOCK_SIZE;
	sstream_toshell << BLOCKS_NUM * BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 字节\t";
	sstream_toshell << " 字节\t";
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
	cout << "已用空间：";
	sstream_toshell << "已用空间：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << (BLOCKS_NUM - sbGroups[0].allFreeBlocks) * BLOCK_SIZE;
	sstream_toshell << (BLOCKS_NUM - sbGroups[0].allFreeBlocks) * BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 字节\t";
	sstream_toshell << " 字节\t";
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
	cout << "可用空间：";
	sstream_toshell << "可用空间：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << sbGroups[0].allFreeBlocks * BLOCK_SIZE;
	sstream_toshell << sbGroups[0].allFreeBlocks * BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 字节\t";
	sstream_toshell << " 字节\t";
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
	cout << "可用空间比例：";
	sstream_toshell << "可用空间比例：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << ((float)sbGroups[0].allFreeBlocks / (float)BLOCKS_NUM) * 100
		<< "%\n";
	sstream_toshell << ((float)sbGroups[0].allFreeBlocks / (float)BLOCKS_NUM) * 100
		<< "%\n";
	cout << "\n详细信息：\n";
	sstream_toshell << "\n详细信息：\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "盘块大小：";
	sstream_toshell << "盘块大小：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCK_SIZE;
	sstream_toshell << BLOCK_SIZE;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 字节\n";
	sstream_toshell << " 字节\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "每组盘块数：";
	sstream_toshell << "每组盘块数：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCKS_EACH;
	sstream_toshell << BLOCKS_EACH;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 块\n";
	sstream_toshell << " 块\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "每组i-结点数：";
	sstream_toshell << "每组i-结点数：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << INODES_EACH;
	sstream_toshell << INODES_EACH;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 个\n";
	sstream_toshell << " 个\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "盘块总数：";
	sstream_toshell << "盘块总数：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << BLOCKS_NUM;
	sstream_toshell << BLOCKS_NUM;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 块\n";
	sstream_toshell << " 块\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "i-结点总数：";
	sstream_toshell << "i-结点总数：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << INODES_NUM;
	sstream_toshell << INODES_NUM;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 个\n";
	sstream_toshell << " 个\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "空闲块总数：";
	sstream_toshell << "空闲块总数：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << sbGroups[0].allFreeBlocks;
	sstream_toshell << sbGroups[0].allFreeBlocks;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 块\n";
	sstream_toshell << " 块\n";
	cout.width(18);
	//sstream_toshell.width(18);
	cout << "空闲i-结点总数：";
	sstream_toshell << "空闲i-结点总数：";
	cout.width(10);
	//sstream_toshell.width(10);
	cout << sbGroups[0].allFreeInodes;
	sstream_toshell << sbGroups[0].allFreeInodes;
	cout.width(3);
	//sstream_toshell.width(3);
	cout << " 个\n";
	sstream_toshell << " 个\n";
}

void cmd_help(const char* strArg) {
	if (strArg[0] == '\0')//显示help命令的目录细节
	{
		cout << "命令帮助信息：\n\n";
		sstream_toshell << "命令帮助信息：\n\n";
		cout << "info\t显示系统信息。\n";
		sstream_toshell << "info\t显示系统信息。\n";
		cout << "cd\t显示当前目录名或改变当前目录。\n";
		sstream_toshell << "cd\t显示当前目录名或改变当前目录。\n";
		cout << "dir\t显示目录中的文件和子目录列表。\n";
		sstream_toshell << "dir\t显示目录中的文件和子目录列表。\n";
		cout << "md\t创建目录。\n";
		sstream_toshell << "md\t创建目录。\n";
		cout << "rd\t删除目录。\n";
		sstream_toshell << "rd\t删除目录。\n";
		cout << "newfile\t建立文件。\n";
		sstream_toshell << "newfile\t建立文件。\n";
		cout << "cat\t显示文本文件的内容。\n";
		sstream_toshell << "cat\t显示文本文件的内容。\n";
		cout << "write\t修改文本文件的内容。\n";
		sstream_toshell << "write\t修改文本文件的内容。\n";
		cout << "copy\t将指定文件复制到另一个位置。\n";
		sstream_toshell << "copy\t将指定文件复制到另一个位置。\n";
		cout << "del\t删除指定文件。\n";
		sstream_toshell << "del\t删除指定文件。\n";
		cout << "check\t检测并恢复文件系统。\n";
		sstream_toshell << "check\t检测并恢复文件系统。\n";
		cout << "init\t初始化磁盘文件系统。\n";
		sstream_toshell << "init\t初始化磁盘文件系统。\n";
		cout << "cls\t清除屏幕内容。\n";
		sstream_toshell << "cls\t清除屏幕内容。\n";
		cout << "help\t提供命令的帮助信息。\n";
		sstream_toshell << "help\t提供命令的帮助信息。\n";
		cout << "logon\t重新登录。\n";
		sstream_toshell << "logon\t重新登录。\n";
		cout << "useradd\t添加新用户。\n";
		sstream_toshell << "useradd\t添加新用户。\n";
		cout << "userdel\t删除用户。\n";
		sstream_toshell << "userdel\t删除用户。\n";
		cout << "exit\t保存更改并退出程序。\n\n";
		sstream_toshell << "exit\t保存更改并退出程序。\n\n";
		cout << "\t有关某个命令的详细信息，请键入 help 命令名，或者 命令名 /?。\n";
		sstream_toshell << "\t有关某个命令的详细信息，请键入 help 命令名，或者 命令名 /?。\n";
		return;
	}
	int i = 0;
	for (i = 0; i < COMMAND_COUNT; i++)	// 比较命令，输出对应的命令帮助
		if (strcmp(strCommands[i], strArg) == 0)	break;
	switch (i)
	{
	case 0:		//info
		cout << "info\n说明：显示系统信息。\n格式：info";
		sstream_toshell << "info\n说明：显示系统信息。\n格式：info";
		break;
	case 1:		//cd
		cout << "cd\n说明：显示当前目录名或改变当前目录。\n格式：cd [路径]";
		sstream_toshell << "cd\n说明：显示当前目录名或改变当前目录。\n格式：cd [路径]";
		cout << "\n其中，路径参数省略将显示当前路径。";
		sstream_toshell << "\n其中，路径参数省略将显示当前路径。";
		break;
	case 2:		//dir
		cout << "dir\n说明：显示目录中的文件和子目录列表。\n格式：dir [路径]";
		sstream_toshell << "dir\n说明：显示目录中的文件和子目录列表。\n格式：dir [路径]";
		break;
	case 3:		//md
		cout << "md\n说明：创建目录。\n格式：md 路径 [属性]";
		sstream_toshell << "md\n说明：创建目录。\n格式：md 路径 [属性]";
		cout << "\n其中，属性值可以是以下任何值之一：\n";
		sstream_toshell << "\n其中，属性值可以是以下任何值之一：\n";
		cout << "/a = 读写\t/r = 只读\n/w = 只写\t/s = 共享";
		sstream_toshell << "/a = 读写\t/r = 只读\n/w = 只写\t/s = 共享";
		cout << "\n属性参数省略将默认为“读写”属性。";
		sstream_toshell << "\n属性参数省略将默认为“读写”属性。";
		break;
	case 4:		//rd
		cout << "rd\n说明：删除目录。\n格式：rd 路径";
		sstream_toshell << "rd\n说明：删除目录。\n格式：rd 路径";
		break;
	case 5:		//newfile
		cout << "newfile\n说明：建立文件。\n格式：newfile [路径]文件名 [属性]";
		sstream_toshell << "newfile\n说明：建立文件。\n格式：newfile [路径]文件名 [属性]";
		cout << "\n其中，属性值可以是以下任何值之一：\n";
		sstream_toshell << "\n其中，属性值可以是以下任何值之一：\n";
		cout << "/rw = 读写\t/r = 只读\n/w = 只写\t/s = 共享";
		sstream_toshell << "/rw = 读写\t/r = 只读\n/w = 只写\t/s = 共享";
		cout << "\n属性参数省略将默认为“读写”属性。";
		sstream_toshell << "\n属性参数省略将默认为“读写”属性。";
		break;
	case 6:		//cat
		cout << "cat\n说明：显示文本文件的内容。\n格式：cat [路径]文件名";
		sstream_toshell << "cat\n说明：显示文本文件的内容。\n格式：cat [路径]文件名";
		break;
	case 7:		//write
		cout << "wirte\n说明：修改文本文件的内容。\n格式：write [路径]文件名";
		sstream_toshell << "wirte\n说明：修改文本文件的内容。\n格式：write [路径]文件名";
		break;
	case 8:		//copy
		cout << "copy\n说明：将指定文件复制到另一个位置。\n格式：copy [<host>][来源路径]文件名 [<host>]目标路径";
		sstream_toshell << "copy\n说明：将指定文件复制到另一个位置。\n格式：copy [<host>][来源路径]文件名 [<host>]目标路径";
		cout << "\n其中，<host> 开关选项表示路径参数为宿主机路径；\n来源路径和目标路径参数中，至多只能有一个带有 <host>。";
		sstream_toshell << "\n其中，<host> 开关选项表示路径参数为宿主机路径；\n来源路径和目标路径参数中，至多只能有一个带有 <host>。";
		cout << "\n宿主机文件路径中不能带有空格。";
		sstream_toshell << "\n宿主机文件路径中不能带有空格。";
		break;
	case 9:		//del
		cout << "del\n说明：删除指定文件。\n格式：del [路径]文件名";
		sstream_toshell << "del\n说明：删除指定文件。\n格式：del [路径]文件名";
		break;
	case 10:	//check
		cout << "check\n说明：检测并恢复文件系统。\n格式：check";
		sstream_toshell << "check\n说明：检测并恢复文件系统。\n格式：check";
		break;
	case 11:		//init
		cout << "init\n说明：初始化磁盘文件系统。\n格式：init";
		sstream_toshell << "init\n说明：初始化磁盘文件系统。\n格式：init";
		cout << "\n初始化后将生成 root/usr 系统目录，其下保存用户账户信息，不可删除。";
		sstream_toshell << "\n初始化后将生成 root/usr 系统目录，其下保存用户账户信息，不可删除。";
		cout << "\n* 需要管理员权限 *";
		sstream_toshell << "\n* 需要管理员权限 *";
		break;
	case 12:	//cls
		cout << "cls\n说明：清除屏幕内容。\n格式：cls";
		sstream_toshell << "cls\n说明：清除屏幕内容。\n格式：cls";
		break;
	case 13:	//help
		cout << "help\n说明：提供命令的帮助信息。\n格式：help [命令名] 或者 命令名 /?";
		sstream_toshell << "help\n说明：提供命令的帮助信息。\n格式：help [命令名] 或者 命令名 /?";
		cout << "\n其中，命令名参数省略则显示所有命令列表。";
		sstream_toshell << "\n其中，命令名参数省略则显示所有命令列表。";
		break;
	case 14:	//logon
		cout << "logon\n说明：重新登录。\n格式：logon";
		sstream_toshell << "logon\n说明：重新登录。\n格式：logon";
		break;
	case 15:	//useradd
		cout << "useradd\n说明：添加新用户。\n格式：useradd 账号 [用户类型]";
		sstream_toshell << "useradd\n说明：添加新用户。\n格式：useradd 账号 [用户类型]";
		cout << "\n其中，账号必须为数字。\n用户类型可以为以下任何值之一：\n";
		sstream_toshell << "\n其中，账号必须为数字。\n用户类型可以为以下任何值之一：\n";
		cout << "/u = 普通用户\t/a = 管理员";
		sstream_toshell << "/u = 普通用户\t/a = 管理员";
		cout << "\n用户类型参数省略将默认为普通用户。";
		sstream_toshell << "\n用户类型参数省略将默认为普通用户。";
		cout << "\n* 需要管理员权限 *";
		sstream_toshell << "\n* 需要管理员权限 *";
		break;
	case 16:	//userdel
		cout << "userdel\n说明：删除用户。\n格式：userdel 账号";
		sstream_toshell << "userdel\n说明：删除用户。\n格式：userdel 账号";
		cout << "\n其中，账号必须为数字。";
		sstream_toshell << "\n其中，账号必须为数字。";
		cout << "\n* 需要管理员权限 *";
		sstream_toshell << "\n* 需要管理员权限 *";
		break;
	case 17:	//exit
		cout << "exit\n说明：保存更改并退出程序。\n格式：exit";
		sstream_toshell << "exit\n说明：保存更改并退出程序。\n格式：exit";
		break;
	default:
		error(INVILID_CMD, strArg);
		return;
	}
	cout << "\n\n注意：\t格式中带 [] 的为可选项，路径省略默认为当前路径。\n";
	sstream_toshell << "\n\n注意：\t格式中带 [] 的为可选项，路径省略默认为当前路径。\n";
	cout << "\t支持相对路径，“.”代表当前目录，“..”代表父目录。\n";
	sstream_toshell << "\t支持相对路径，“.”代表当前目录，“..”代表父目录。\n";
}

/*
check命令对应函数，检测并恢复文件系统，对文件系统中的数据一致性进行检测，并自动根据文件系统的结构和信息进行数据再整理
*/
void cmd_check()
{
	int i, j;
	int nStart;				//起始地址
	unsigned int nFreeBlockNum, nFreeInodeNum;			//空闲块和i-结点
	unsigned int nFreeBlockAll = 0, nFreeInodeAll = 0;	//所有空闲块和i-结点
	cout << "\n******************************\n";
	sstream_toshell << "\n******************************\n";
	cout << "正在检查文件系统……\n";
	sstream_toshell << "正在检查文件系统……\n";
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		nFreeBlockNum = 0;
		nFreeInodeNum = 0;
		nStart = i * BLOCKS_EACH;
		//计算空闲块和i-结点总和
		for (j = 0; j < BLOCKS_EACH; j++)
		{
			if (bsBlockBmp[nStart + j] == NOT_USED) nFreeBlockNum++;
			if (bsInodeBmp[nStart + j] == NOT_USED) nFreeInodeNum++;
		}
		//计算结果和磁盘记录不同，则发生了异常
		if (sbGroups[i].FreeBlocksNum != nFreeBlockNum)
		{
			sbGroups[i].FreeBlocksNum = nFreeBlockNum;
		}
		if (sbGroups[i].FreeInodesNum != nFreeInodeNum)
		{
			sbGroups[i].FreeInodesNum = nFreeInodeNum;
		}
		//加入总和
		nFreeBlockAll += sbGroups[i].FreeBlocksNum;
		nFreeInodeAll += sbGroups[i].FreeInodesNum;
	}
	//计算结果和磁盘记录不同，则发生了异常
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

	//保存改动

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
	cout << "文件系统修复完成！\n";
	sstream_toshell << "文件系统修复完成！\n";

}

void cmd_cls() {
	system("cls");
}

//useradd命令对应函数，添加新用户
void cmd_useradd(const char* strUserId, user_type user_tType)
{
	//如果当前用户类型不是系统或者管理员则报错没有权限创建账户
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
	if (cur_Idsum < MAX_USER_NUM)	//没有超过限制
	{
		//查找是否存在同名账号
		for (unsigned int i = 0; i < cur_Idsum; i++)
		{
			//如果账号同名则报错用户已存在
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
		//初始化
		char password[MAX_PWD_LENGTH];
		cout << "密码(最长20位)：";
		sstream_toshell << "密码(最长20位)：";
		ReleaseSemaphore(mutexSim[1], 1, NULL);
		WaitForSingleObject(mutexSim[0], INFINITE);
		cin_pwd(password, MAX_PWD_LENGTH);
		stringstream ssStream;
		ssStream << password << " " << (int)user_tType;
		char strBuffer[MAX_PWD_LENGTH + 5];//建立字符串缓冲区来接受用户输入的密码
		strcpy(strBuffer, ssStream.str().c_str());
		ssStream.clear();
		//格式化完成，写入文件
		Dir dirUsr;
		analyse_path("/user", dirUsr);
		char* tem = new char[MAX_NAME_LENGTH];
		strcpy(tem, strUserId);
		save_file(tem, strBuffer, MAX_PWD_LENGTH + 5, dirUsr, PROTECTED);
		//添加一个用户到内存
		cur_Idsum++;
		systemUsers[cur_Idsum - 1].set_user_info(strUserId, password, user_tType);
	}
	else
	{
		error(USERS_MAX);
	}
}

//userdel：删除用户
void cmd_userdel(const char* strUserId) {
	if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM) {
		error(NO_PRIVILEGE_USERDEL);
		return;
	}
	if (strcmp(strUserId, cur_user_Id) == 0) {
		error(PROTECTED_USER);
		return;
	}
	//查找是否存在同名账号
	for (unsigned int i = 0; i < cur_Idsum; i++)
	{
		if (strcmp(systemUsers[i].get_user_id(), strUserId) == 0)
		{
			char* path = new char[MAX_NAME_LENGTH];
			strcpy(path, "root/user/");
			strcat(path, systemUsers[i].get_user_id());

			Dir dirTemp;
			char strDirName[MAX_NAME_LENGTH];
			if (analyse_path(path, dirTemp, strDirName)) {//路径合法
				//判断用户权限
				if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE &&
					strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
				{
					error(NO_PRIVILEGE, path);
					cout << endl;
					sstream_toshell << '\n';
					error(DEL_FAILED);
					return;
				}
				unsigned int nIndex;//文件i节点号
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

			cout << "账号删除成功！" << endl;
			sstream_toshell << "账号删除成功！\n";
			get_user_info();
			return;
		}
	}
	error(USER_NOT_EXIST, strUserId);
	return;

}

//退出程序
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

//与目录、文件相关的操作指令定义——

/*
cd …:  改变目录：改变当前工作目录，目录不存在时给出出错信息。
*/
void cmd_cd(const char* strArgv) {
	if (strArgv[0] == '\0') {
		return;
	}
	Dir dirTemp;
	if (analyse_path(strArgv, dirTemp)) {//路径合法
		//判断用户权限
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
dir …:  显示目录：显示指定目录下或当前目录下的信息，
包括文件名、物理地址、保护码、文件长度、子目录等
（带/s参数的dir命令，显示所有子目录）。
*/
void cmd_dir(const char* strPath) {

	Dir dirTemp;
	if (strPath[0] == '\0') {
		dirTemp = cur_dir;
	}
	else if (analyse_path(strPath, dirTemp)) {//路径合法
		//判断用户权限
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
	//打印目录下信息
	dir_info(dirTemp.Inode_num);
}

//显示目录下的相关信息
void dir_info(unsigned int InodeNum) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	cout << indTable[InodeNum].fileName << " 的目录\n\n";
	sstream_toshell << indTable[InodeNum].fileName << " 的目录\n\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	cout.width(20);
	cout << left << "目录名/文件名";
	sstream_toshell << "目录名/文件名\t";
	cout.width(20);
	cout << left << "修改日期";
	sstream_toshell << "\t修改日期";
	cout.width(4);
	cout << "类型";
	sstream_toshell << "\t类型";
	cout.width(18);
	cout << right << "大小（字节）";
	sstream_toshell << "\t\t大小（字节）";
	cout.width(15);
	cout << right << "物理地址";
	sstream_toshell << "\t物理地址";
	cout.width(15);
	cout << right << "所有者";
	sstream_toshell << "\t所有者";
	cout.width(15);
	cout << right << "属性";
	sstream_toshell << "\t\t属性";
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
			cout << "\t" << (tem_inode.fileType == TYPE_DIR ? "目录" : "文件");
			sstream_toshell << "\t" << (tem_inode.fileType == TYPE_DIR ? "目录" : "文件");
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
			case PROTECTED: attr = "系统"; break;
			case READ_WRITE:attr = "读写"; break;
			case READ_ONLY:attr = "只读"; break;
			case WRITE_ONLY:attr = "只写"; break;
			case SHARE:attr = "共享"; break;
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
md …:  创建目录：在指定路径或当前路径下创建指定目录。重名时给出错信息。
*/
void cmd_md(const char* strPath, file_attrib privilege) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//路径合法
		//判断用户权限			if (strcmp(tem_dirName, "user") == 0 && strcmp(indTable[dirTemp.Inode_num].fileName, "root"))return false;
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			indTable[dirTemp.Inode_num].fileAttr != SHARE &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0 || (strcmp(indTable[dirTemp.SubInode_num[1]].fileName, "root") == 0 && strcmp(indTable[dirTemp.Inode_num].fileName, "user") == 0))
		{//如果当前用户不是管理员或系统且路径不是root且当前用户id与i-节点表首位用户信息不同则报错无访问权限与目录创建失败
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
			return;
		}
		if (indTable[dirTemp.Inode_num].fileAttr == READ_ONLY)
		{//若当前文件为只读类型的文件则不能创建目录在其下，则报错文件只读与目录创建失败
			error(DIR_READONLY, indTable[dirTemp.Inode_num].fileName);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
			return;
		}
		long Addr = -1;
		int nInode = -1;
		unsigned int nIndex;
		//是否已存在该名字的子项，如果存在则报错文件已存在且目录创建失败
		if (dirTemp.have_child(strDirName, nIndex, TYPE_DIR))
		{
			error(FILE_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
			return;
		}
		//分配目录的存储空间
		Addr = alloc_blocks(DIR_SIZE, nIndex);
		if (Addr < 0)	//空间不足则报错磁盘空间不足且创建目录失败
		{
			error(SPACE_NOT_ENOUGH);
			cout << endl;
			sstream_toshell << '\n';
			error(MD_FAILED);
		}
		else
		{
			//分配i-结点
			nInode = alloc_inodes();
			//如果i-节点位图已满则无法分配i-节点就报错i--节点分配失败且目录创建失败
			if (nInode < 0)
			{
				error(INODE_ALLOC_FAILED);
				cout << endl;
				sstream_toshell << '\n';
				error(MD_FAILED);
				return;
			}
			//开始创建目录，添加设置相应信息
			Dir dirNew;//创建一个新的目录对象
			dirNew.Inode_num = (unsigned int)nInode;//分配i-节点
			dirNew.child_num = 2;//子文件+子目录总数为2
			dirNew.SubInode_num[0] = (unsigned int)nInode;//分配子项目i-节点1
			dirNew.SubInode_num[1] = dirTemp.Inode_num;//分配子项目i-节点2
			strcpy(indTable[dirNew.Inode_num].fileName, strDirName);//配置此目录对应的文件名
			strcpy(indTable[dirNew.Inode_num].UserID, cur_user_Id);//配置此目录所属的用户名
			indTable[nInode].fileType = TYPE_DIR;//在i-节点表中设置文件类型为目录
			indTable[nInode].fileAttr = privilege;//在i-节点表中设置文件属性为输入的属性
			indTable[nInode].fileSize = sizeof(Dir);//在i-节点表中设置文件大小为目录文件大小
			indTable[nInode].BlockAddr = Addr;//在i-节点表中设置目录磁盘块起始地址
			indTable[nInode].havingBlocks = DIR_SIZE;//在i-节点表中设置占用磁盘块数大小为目录尺寸
			time_t lTime;
			time(&lTime);
			tm tmCreatedTime = *localtime(&lTime);//记录目录创建时间
			indTable[nInode].fileMfyTime.set_time(tmCreatedTime);//在i - 节点表中设置当前目录创建时间
			//在父目录中添加信息
			dirTemp.SubInode_num[dirTemp.child_num] = (unsigned int)nInode;//将该目录加入到父目录的子节点中
			dirTemp.child_num++;
			if (dirTemp.Inode_num == cur_dir.Inode_num)
			{
				cur_dir = dirTemp;
				if (!creatingDisk && !creatingUser)
				{
					// 更新两个shell存储的目录
					WaitForSingleObject(tuSema, INFINITE);
					for (int i = 0; i < userCount; i++)
					{
						ThreadID_UserID[i].user_dir.SubInode_num[ThreadID_UserID[i].user_dir.child_num] = (unsigned int)nInode;
						ThreadID_UserID[i].user_dir.child_num++;
					}
					ReleaseSemaphore(tuSema, 1, NULL);
				}
			}
			//保存
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

	else {//报错无法找到路径且目录创建失败
		error(PATH_NOT_FOUND);
		cout << endl;
		error(MD_FAILED);
		return;
	}
}

/*
rd …:  删除目录：删除指定目录下所有文件和子目录。要删目录不空时，要给出提示是否要删除。
*/
void cmd_rd(const char* strPath) {
	Dir dirTemp;
	Dir dirRemove;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//路径合法
		if (strcmp(strDirName, ".") == 0 || strcmp(strDirName, "..") == 0) {
			error(PATH_NOT_FOUND, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(RD_FAILED);
			return;
		}
		//判断用户权限
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
		//判断要删除的目录是否存在
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
				cout << "删除目录完成!" << endl;
				sstream_toshell << "删除目录完成!\n";
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

//删除子文件 从目录dirRemove中删除节点号为nIndex的文件，主要将当前目录项的child_num以及超级块和位图修改并写回虚拟磁盘
void delete_file(Dir& dirRemove, unsigned int nIndex) {
	release_blocks(nIndex);
	release_inodes(nIndex);

	int len = dirRemove.child_num;//目录子项个数
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
		// 更新两个shell存储的目录
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

//删除子目录
void remove_dir(Dir& dirTemp, Dir& dirRemove) {
	unsigned int dirTempInode = dirTemp.Inode_num, dirRemoveInode = dirRemove.Inode_num;
	release_blocks(dirRemoveInode);
	release_inodes(dirRemoveInode);

	int len = dirTemp.child_num;//目录子项个数
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
		// 更新两个shell存储的目录
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
newfile …:  建立文件。
*/
void cmd_newfile(const char* strPath, file_attrib privilege) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//路径合法
		//判断用户权限
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			indTable[dirTemp.Inode_num].fileAttr != SHARE &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
		{//如果当前用户状态不是管理员或系统或者目录文件名不是root切文件属性不是共享和用户id匹配不上则报错配有权限且无法创建新文件
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
		{//如果当前文件属性为只读则报错文件只读且无法创建新文件
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

		unsigned int nIndex;//返回的文件索引
		if (dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
		{//判断当前目录下是否有同名子文件，如果有则报错文件已存在且无法创建新文件
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
		user_input(strBuffer, strBufferSize);//用户输入
		save_file(strDirName, strBuffer, strBufferSize, dirTemp, privilege);
		*/
		// 修改 newfile 函数中的如下部分：

		unsigned long strBufferSize = 200;
		char* strBuffer = new char[strBufferSize + 1];  // 增加一位来存放字符串的结尾标志 '\0'
		user_input(strBuffer, strBufferSize);  // 调用 user_input 函数时，传递的长度参数为 strBufferSize
		strBuffer[strBufferSize] = '\0';  // 在末尾添加字符串的结尾标志
		save_file(strDirName, strBuffer, strlen(strBuffer), dirTemp, privilege);  // 通过 strlen 获取实际字符串的长度

	}

	else {//如果路径无法解析则报错无法找到路径且新文件创建失败
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

//存储文件的函数
void save_file(char* strFileName, char* strBuffer, unsigned int strBufferSize, Dir dirTemp, file_attrib privilege) {
	long Addr = -1; // 分配的数据块首地址
	int nInode = -1; // 分配的i节点号
	unsigned int nIndex = 0;
	bool bExist = false; // 判断文件是否已存在

	for (int i = 0; i < dirTemp.child_num; i++) { // 遍历子项
		if (strcmp(indTable[dirTemp.SubInode_num[i]].fileName, strFileName) == 0) { // 如果找到同名文件
			nInode = dirTemp.SubInode_num[i]; // 直接使用原有的i节点号
			bExist = true;
			break;
		}
	}

	if (!bExist) { // 文件不存在，则进行分配空间和创建i节点
		// 分配目录的存储空间
		// 分配存储文件内容的数据块，并获取数据块的起始地址和索引
		Addr = alloc_blocks((strBufferSize * sizeof(char)) / BLOCK_SIZE + 1, nIndex);
		if (Addr < 0) { // 空间不足
			error(SPACE_NOT_ENOUGH);
			cout << endl;
			error(NEW_FILE_FAILED);
		}
		else {
			// 分配i-结点
			nInode = alloc_inodes();
			if (nInode < 0) {
				error(INODE_ALLOC_FAILED);
				cout << endl;
				error(NEW_FILE_FAILED);
				return;
			}
			// 开始创建i节点信息，添加设置相应信息
			strcpy(indTable[nInode].fileName, strFileName); // 设置文件名
			strcpy(indTable[nInode].UserID, cur_user_Id); // 设置用户ID
			indTable[nInode].fileType = TYPE_FILE; // 设置文件类型为普通文件
			indTable[nInode].fileAttr = privilege; // 设置文件属性（权限）
			indTable[nInode].fileSize = strBufferSize * sizeof(char); // 设置文件大小
			indTable[nInode].BlockAddr = Addr; // 设置数据块地址
			indTable[nInode].havingBlocks = (strBufferSize * sizeof(char)) / BLOCK_SIZE + 1; // 设置数据块数量

			time_t lTime;
			time(&lTime);
			tm tmCreatedTime = *localtime(&lTime);
			indTable[nInode].fileMfyTime.set_time(tmCreatedTime); // 设置文件的修改时间

			// 在父目录中添加信息
			dirTemp.SubInode_num[dirTemp.child_num] = (unsigned int)nInode; // 记录子节点的i节点号
			dirTemp.child_num++;

			if (dirTemp.Inode_num == cur_dir.Inode_num)
			{
				cur_dir = dirTemp; // 更新当前目录
				if (!creatingDisk)
				{
					// 更新两个shell存储的目录
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

	// 保存到磁盘中
	fs.open("Virtual_Disk.bin", ios::out | ios::binary | ios::_Nocreate); // 打开虚拟磁盘文件
	if (!fs.is_open()) error(WRITE_FAILED); // 打开失败则报错

	int i;
	for (i = 0; i < BLOCK_GROUPS_NUM; i++) {
		fs.write(reinterpret_cast<char*>(&sbGroups[i]), sizeof(SuperBlock)); // 写入超级块
		fs.flush();
	}

	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * nInode), ios::beg);
	fs.write(reinterpret_cast<char*>(&indTable[nInode]), sizeof(Inode)); // 写入该文件的i节点信息
	fs.flush();

	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * nIndex), ios::beg);
	for (i = 0; i < indTable[nInode].havingBlocks; i++) {
		fs.write(reinterpret_cast<char*>(&bsBlockBmp[nIndex + i]), sizeof(bmp_status)); // 写入数据块位图
		fs.flush();
	}

	fs.seekp((sizeof(SuperBlock) * BLOCK_GROUPS_NUM + sizeof(Inode) * INODES_NUM + sizeof(bmp_status) * BLOCKS_NUM + sizeof(bmp_status) * nInode), ios::beg);
	fs.write(reinterpret_cast<char*>(&bsInodeBmp[nInode]), sizeof(bmp_status)); // 写入i节点位图
	fs.flush();

	fs.seekp(indTable[nInode].BlockAddr, ios::beg);
	fs.write(reinterpret_cast<char*>(strBuffer), indTable[nInode].fileSize); // 写入文件内容
	fs.flush();

	fs.seekp(indTable[dirTemp.Inode_num].BlockAddr, ios::beg);
	fs.write(reinterpret_cast<char*>(&dirTemp), sizeof(Dir)); // 更新父目录中的信息
	fs.flush();

	fs.close(); // 关闭虚拟磁盘文件
}

//用户输入文件信息函数
void user_input(char* strBuffer, unsigned long& strBufferSize) {
	char user_cin = '\0';//用户输入字符
	unsigned long nowBufferSize = 0;//获取当前缓冲区的大小
	for (int i = 0; i < strBufferSize; i++)strBuffer[i] = ' ';//初始化字符数组：在函数开始时，将strBuffer所指向的字符数组的所有元素都设置为' '，即空格。
	cout << "\n请输入文件内容，按$结束" << endl;
	sstream_toshell << "\n请输入文件内容，按$结束" << endl;
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
cat …:  打开文件。
*/
void cmd_cat(const char* strPath) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//路径合法
		//判断用户权限
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
		{//如果当前用户类型不是管理员或系统，文件属性不是共享，且用户名和文件名对不上的话就报错没有权限且无法打开
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(CAT_FAILED);
			return;
		}
		unsigned int nIndex;//文件i节点号
		if (!dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
		{//如果当前目录下没有这个文件则报错文件不存在且无法打开
			error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strDirName);
			cout << endl;
			sstream_toshell << '\n';
			error(CAT_FAILED);
			return;
		}
		else {

			if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[nIndex].fileAttr != SHARE &&
				strcmp(indTable[nIndex].UserID, cur_user_Id) != 0)
			{//如果当前用户类型不是管理员或系统，文件属性不是共享，且用户名对不上的话就报错没有权限且无法打开
				error(NO_PRIVILEGE, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(CAT_FAILED);
				return;
			}

			if (indTable[dirTemp.Inode_num].fileAttr == WRITE_ONLY)
			{//如果当前文件属性为只写则报错文件只写无法打开
				error(FILE_WRITEONLY, strPath);
				cout << endl;
				sstream_toshell << '\n';
				error(CAT_FAILED);
				return;
			}
			char* strBuffer = new char[indTable[nIndex].fileSize];
			open_file(nIndex, strBuffer);
			cout << "文件内容：" << endl;
			sstream_toshell << "文件内容：\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
			wchar_t* wStrBuffer = new wchar_t[indTable[nIndex].fileSize * 2];  // 为存储 wchar_t 类型的数组分配空间，乘以2是因为每个字符可能会占用两个字节
			MultiByteToWideChar(CP_UTF8, 0, strBuffer, -1, wStrBuffer, indTable[nIndex].fileSize * 2);  // 将 strBuffer 转换为 wchar_t 类型的数组
			int i = 0;
			// 将wchar_t*类型的数组转换为std::string类型
			wstring_convert<codecvt_utf8<wchar_t>> converter;
			while (wStrBuffer[i] != L'\0') {  // 判断字符串终止条件
				if (iswprint(wStrBuffer[i])) {  // 判断字符是否可打印，使用 iswprint 函数判断 wchar_t 类型的字符
					//wcout << wStrBuffer[i];
					string str = converter.to_bytes(wStrBuffer[i]);
					cout << str;
					sstream_toshell << str;
				}
				if (wStrBuffer[i] == L'\n') {  // 判断是否为换行符，如果是则输出两个回车符"\r\n"
					cout << "\r\n";
					sstream_toshell << "\r\n";
				}
				i++;
			}
			delete[] wStrBuffer;  // 释放动态分配的内存
			cout << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
	else {//如果是无法解析的路径则报错路径错误无法打开
		error(PATH_NOT_FOUND);
		cout << endl;
		error(CAT_FAILED);
		return;
	}
}

/*
write …:  修改文件。
*/
void cmd_write(const char* strPath) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//路径合法
		//判断用户权限
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0)
		{//如果当前用户类型不是管理员或系统，文件属性不是共享，且用户名和文件名对不上的话就报错没有权限且无法打开
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
		unsigned int nIndex = 0;//文件i节点号
		if (!dirTemp.have_child(strDirName, nIndex, TYPE_FILE))
		{//如果当前目录下没有这个文件则报错文件不存在且无法打开
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
			{//如果当前用户类型不是管理员或系统，文件属性不是共享，且用户名对不上的话就报错没有权限且无法打开
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
			{//如果当前文件属性为只读则报错文件只读无法打开
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

			char* strBuffer = new char[indTable[nIndex].fileSize + 1];  // 增加一位来存放字符串的结尾标志 '\0'
			open_file(nIndex, strBuffer);

			cout << "当前文件内容：" << endl;
			sstream_toshell << "当前文件内容：\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
			wchar_t* wStrBuffer = new wchar_t[indTable[nIndex].fileSize * 2];  // 存储 wchar_t 类型的数组，乘以2是因为每个字符可能会占用两个字节
			MultiByteToWideChar(CP_UTF8, 0, strBuffer, -1, wStrBuffer, indTable[nIndex].fileSize * 2);  // 将 strBuffer 转换为 wchar_t 类型的数组
			int i = 0;
			// 将wchar_t*类型的数组转换为std::string类型
			wstring_convert<codecvt_utf8<wchar_t>> converter;
			while (wStrBuffer[i] != L'\0') {  // 判断字符串终止条件
				if (iswprint(wStrBuffer[i])) {  // 判断字符是否可打印，使用 iswprint 函数判断 wchar_t 类型的字符
					//wcout << wStrBuffer[i];
					string str = converter.to_bytes(wStrBuffer[i]);
					cout << str;
					sstream_toshell << str;
				}
				if (wStrBuffer[i] == L'\n') {  // 判断是否为换行符，如果是则输出两个回车符"\r\n"
					cout << "\r\n";
					sstream_toshell << "\r\n";
				}
				i++;
			}
			delete[] wStrBuffer;  // 释放动态分配的内存
			cout << endl;
			sstream_toshell << "\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			cout << "请输入新的文件内容：" << endl;
			sstream_toshell << "请输入新的文件内容：\n";
			unsigned long strBufferSize = 200;
			char* strBuffer1 = new char[strBufferSize + 1];  // 增加一位来存放字符串的结尾标志 '\0'
			user_input(strBuffer1, strBufferSize);  // 调用 user_input 函数时，传递的长度参数为 strBufferSize
			strBuffer1[strBufferSize] = '\0';  // 在末尾添加字符串的结尾标志
			save_file(strDirName, strBuffer1, strlen(strBuffer1), dirTemp, indTable[dirTemp.Inode_num].fileAttr);  // 保存文件

			delete[] strBuffer;  // 释放动态分配的内存
			delete[] strBuffer1;  // 释放动态分配的内存
		}
	}
	else {//如果是无法解析的路径则报错路径错误无法打开
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
	//读取指定地址
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
del …:  删除文件：删除指定文件，不存在时给出出错信息。
*/
void cmd_del(const char* strPath) {
	Dir dirTemp;
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, dirTemp, strDirName)) {//路径合法
		//判断用户权限
		if (cur_user_tType != ADMIN && cur_user_tType != SYSTEM && indTable[dirTemp.Inode_num].fileAttr != SHARE && strcmp(indTable[dirTemp.Inode_num].fileName, "root") != 0 &&
			strcmp(indTable[dirTemp.Inode_num].UserID, cur_user_Id) != 0 || (strcmp(indTable[dirTemp.Inode_num].fileName, "user") == 0 && strcmp(indTable[dirTemp.SubInode_num[1]].fileName, "root") == 0))
		{
			error(NO_PRIVILEGE, strPath);
			cout << endl;
			sstream_toshell << '\n';
			error(DEL_FAILED);
			return;
		}
		unsigned int nIndex;//文件i节点号
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
copy …:  拷贝文件，除支持模拟Linux文件系统内部的文件拷贝外，还支持host文件系统与模拟Linux文件系统间的文件拷贝，host文件系统的文件命名为<host>…,
如：将windows下D：盘的文件\data\sample\test.txt文件拷贝到模拟Linux文件系统中的/test/data目录，windows下D：盘的当前目录为D：\data，则使用命令：
simdisk copy <host>D：\data\sample\test.txt /test/data
或者：simdisk copy <host>D：sample\test.txt /test/data
*/
void cmd_copy(const char* strSrcPath, const char* strDesPath) {
	char strFileName[MAX_NAME_LENGTH] = { "" };
	char* strBuffer;
	Dir dirTemp;
	char strDiv;
	long nLen = 0;
	size_t nSrcLen = strlen(strSrcPath);
	size_t nDesLen = strlen(strDesPath);
	//复制路径，用于修改
	char* strSrcFinalPath = new char[nSrcLen];
	char* strDesFinalPath = new char[nDesLen];
	strcpy(strSrcFinalPath, strSrcPath);
	strcpy(strDesFinalPath, strDesPath);
	//用 if_host_path 判断来源路径参数中是否带有 <host>
	if (isHost(strSrcFinalPath))	//host 文件复制到模拟磁盘中
	{
		if (isHost(strDesFinalPath))
		{
			error(NOT_BOTH_HOST);
			return;
		}
		//读取宿主机文件
		fstream fsHostIn;
		fsHostIn.open(strSrcFinalPath, ios::in | ios::binary);
		if (!fsHostIn.is_open())
		{
			error(HOST_FILE_NOT_EXIST, strSrcFinalPath);
			return;
		}
		//计算文件长度
		fsHostIn.seekg(0, ios::end);
		nLen = fsHostIn.tellg();
		//分配存储空间
		strBuffer = new char[nLen + 1];
		strBuffer[nLen] = 0;
		fsHostIn.seekg(0, ios::beg);
		fsHostIn.read(reinterpret_cast<char*>(strBuffer), nLen);
		fsHostIn.close();
		//提取文件名
		strDiv = '\\';
		cout << strrchr(strSrcFinalPath, strDiv) << endl;
		strcpy(strFileName, strrchr(strSrcFinalPath, strDiv) + 1);
		//分析目标路径

		if (analyse_path(strDesPath, dirTemp))
		{
			//判断用户权限
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
			//判断目录是否已存在同名子项

			unsigned int nIndex;
			if (dirTemp.have_child(strFileName, nIndex, TYPE_FILE))
			{
				delete(strBuffer);
				error(FILE_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
				return;
			}
			//保存到磁盘
			save_file(strFileName, strBuffer, nLen, dirTemp, READ_WRITE);
			delete(strBuffer);
			cout << "文件复制完成！\n";
			sstream_toshell << "文件复制完成！\n";
		}
		else
		{
			error(COPY_FAILED);
		}
	}
	else	//第一个参数不带有 <host>
	{
		if (isHost(strDesFinalPath))	//模拟磁盘文件复制到 host 中
		{
			//分割路径，得到文件名
			if (analyse_path(strSrcFinalPath, dirTemp, strFileName))
			{
				unsigned int nIndex;
				if (!dirTemp.have_child(strFileName, nIndex, TYPE_FILE))
				{//不存在
					error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
					cout << endl;
					sstream_toshell << '\n';
					error(COPY_FAILED);
					return;
				}
				else
				{
					//判断用户权限
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
					//读取文件到内存
					strBuffer = new char[indTable[nIndex].fileSize];
					nLen = open_file(nIndex, strBuffer);
					//合并为宿主机全路径
					char* strFullPath = new char[nSrcLen + nDesLen + 2];
					stringstream ssStream;
					ssStream << strDesFinalPath;
					if (strDesFinalPath[nDesLen - 1] != '\\') ssStream << "\\";
					ssStream << strFileName;
					ssStream >> strFullPath;
					ssStream.clear();
					//写入文件到宿主机系统
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
					cout << "文件复制完成！\n";
					sstream_toshell << "文件复制完成！\n";
				}
			}
			else
			{
				error(COPY_FAILED);
			}
		}
		else	//模拟磁盘中文件复制
		{
			//分割路径，得到文件名
			if (analyse_path(strSrcFinalPath, dirTemp, strFileName))
			{
				unsigned int nIndex;
				if (!dirTemp.have_child(strFileName, nIndex, TYPE_FILE))
				{//不存在
					error(FILE_NOT_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
					cout << endl;
					error(COPY_FAILED);
					return;
				}
				else	//找到
				{
					//判断用户权限
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
					//合并为模拟磁盘全路径
					char* strFullPath = new char[nSrcLen + nDesLen + 2];
					stringstream ssStream;
					ssStream << strDesFinalPath;
					if (strDesFinalPath[nDesLen - 1] != '/') ssStream << "/";
					ssStream << strFileName;
					ssStream >> strFullPath;
					ssStream.clear();
					//分割目标路径，得到文件名
					if (analyse_path(strFullPath, dirTemp, strFileName))
					{
						//判断用户权限
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
						//判断目录是否已存在同名子项
						unsigned int nindex;
						if (dirTemp.have_child(strFileName, nindex, TYPE_FILE))
						{
							delete(strBuffer);
							delete(strFullPath);
							error(FILE_EXIST, indTable[dirTemp.Inode_num].fileName, strFileName);
							return;
						}
						//保存文件
						save_file(strFileName, strBuffer, nLen, dirTemp, faReadWrite);
						cout << "文件复制完成！\n";
						sstream_toshell << "文件复制完成！\n";
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

//判断是否主机路径，并传回路径
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