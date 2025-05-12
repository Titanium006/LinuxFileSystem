/*
handle.cpp
各种处理函数
*/

#include"func.h"
#include"simdisk.h"
#include"ipc.h"

//出错处理函数
void error(error_num errorType, const char* strArg1, const char* strArg2) {
	switch (errorType) {
	case LOGON_FAILED://登录失败的错误处理
		cout << "\n\n******************************" << endl;
		sstream_toshell << "\n\n******************************\n";
		cerr << "取消登录，程序即将退出……" << endl;
		sstream_toshell << "取消登录，程序即将退出……\n";
		Sleep(2000);
		break;
	case WRONG_PWD://密码错误三次的错误处理
		cout << "\n\n******************************" << endl;
		sstream_toshell << "\n\n******************************\n";
		cerr << "密码错误次数已达 3 次，程序即将退出……" << endl;
		sstream_toshell << "密码错误次数已达 3 次，程序即将退出……\n";
		Sleep(2000);
		break;
	case TOO_MANY_ARGS://键入参数过多的错误处理
		cerr << "Error：参数过多。如需帮助，请键入 help /command+/? " << endl;
		sstream_toshell << "Error：参数过多。如需帮助，请键入 help /command+/?\n ";
	case INVILID_CMD://键入无效命令的错误处理
		cerr << "Error：无效的命令 " << strArg1 << "！如需帮助，请键入 help。\n";
		sstream_toshell << "Error：无效的命令 " << strArg1 << "！如需帮助，请键入 help。\n";
		break;

	case NO_PRIVILEGE://没有访问权限时的错误处理
		cerr << "Error：没有权限访问 " << strArg1 << "。";
		sstream_toshell << "Error：没有权限访问 " << strArg1 << "。";
		break;
	case DIR_READONLY://目录只读的错误处理
		cerr << "Error：目录 " << strArg1 << " 只读，无法修改。";
		sstream_toshell << "Error：目录 " << strArg1 << " 只读，无法修改。";
		break;
	case FILE_READONLY://文件只读的错误处理
		cerr << "Error：文件 " << strArg1 << " 只读，无法删除。";
		sstream_toshell << "Error：文件 " << strArg1 << " 只读，无法删除。";
		break;
	case DIR_WRITEONLY://目录只写的错误处理
		cerr << "Error：目录 " << strArg1 << " 只写，无法读取。";
		sstream_toshell << "Error：目录 " << strArg1 << " 只写，无法读取。";
		break;
	case FILE_WRITEONLY://文件只写的错误处理
		cerr << "Error：文件 " << strArg1 << " 只写，无法读取。";
		sstream_toshell << "Error：文件 " << strArg1 << " 只写，无法读取。";
		break;
	case READ_FAILED://无法读取虚拟磁盘的错误处理
		cout << "\n\n******************************\n";
		sstream_toshell << "\n\n******************************\n";
		cerr << "读取模拟 Linux 文件系统失败！\n";
		sstream_toshell << "读取模拟 Linux 文件系统失败！\n";
		break;
	case WRITE_FAILED://无法写入虚拟磁盘的错误处理
		cout << "\n\n******************************\n";
		sstream_toshell << "\n\n******************************\n";
		cerr << "写入模拟 Linux 文件系统失败！\n";
		sstream_toshell << "写入模拟 Linux 文件系统失败！\n";
		break;
	case PATH_NOT_FOUND://找不到指定路径的错误处理
		cerr << "Error：找不到指定路径";
		sstream_toshell << "Error：找不到指定路径";
		break;
	case FILE_EXIST://创建文件时文件已存在的错误处理
		cerr << "Error：目录 " << strArg1 << " 下已存在名为 " << strArg2 << " 的文件或目录！";
		sstream_toshell << "Error：目录 " << strArg1 << " 下已存在名为 " << strArg2 << " 的文件或目录！";
		break;
	case FILE_NOT_EXIST://删除文件时文件不存在的错误处理
		cerr << "Error：目录 " << strArg1 << " 下不存在名为 " << strArg2 << " 的文件！";
		sstream_toshell << "Error：目录 " << strArg1 << " 下不存在名为 " << strArg2 << " 的文件！";
		break;
	case DIR_NOT_EXIST://删除目录时目录不存在的错误处理
		cerr << "Error：目录 " << strArg1 << " 不存在！";
		sstream_toshell << "Error：目录 " << strArg1 << " 不存在！";
		break;
	case SPACE_NOT_ENOUGH://磁盘空间不足的错误处理
		cerr << "Error：磁盘空间不足！";
		sstream_toshell << "Error：磁盘空间不足！";
		break;
	case INODE_ALLOC_FAILED://i-结点分配失败的错误处理
		cerr << "Error：i-节点分配失败！";
		sstream_toshell << "Error：i-节点分配失败！";

	case CD_FAILED://切换目录失败的错误处理
		cerr << "切换目录失败。\n";
		sstream_toshell << "切换目录失败。\n";
		break;
	case DIR_FAILED://显示目录失败的错误处理
		cerr << "目录显示失败！\n";
		sstream_toshell << "目录显示失败！\n";
		break;
	case MD_FAILED://创建目录失败的错误处理
		cerr << "目录创建失败！\n";
		sstream_toshell << "目录创建失败！\n";
		break;
	case MD_MAX_FAILED://由于子目录数达到最大值而无法创建目录的错误处理
		cerr << "目录创建失败！子目录数已达最大值！\n";
		sstream_toshell << "目录创建失败！子目录数已达最大值！\n";
		break;

	case RD_FAILED://删除目录失败的错误处理
		cerr << "目录删除失败！\n";
		sstream_toshell << "目录删除失败！\n";
		break;
	case NEW_FILE_FAILED://文件创建失败的错误处理
		cerr << "文件创建失败！\n";
		sstream_toshell << "文件创建失败！\n";
		break;
	case CAT_FAILED://文件打开失败的错误处理
		cerr << "文件打开失败！\n";
		sstream_toshell << "文件打开失败！\n";
		break;
	case DEL_FAILED://文件删除失败的错误处理
		cerr << "文件删除失败！\n";
		sstream_toshell << "文件删除失败！\n";
		break;
	case COPY_FAILED://文件复制失败的错误处理
		cerr << "文件复制失败！\n";
		sstream_toshell << "文件复制失败！\n";
		break;
	case ERROR_PWD://输入密码错误的错误处理
		cerr << "\n密码错误！\n";
		sstream_toshell << "\n密码错误！\n";
		break;

	case NOT_BOTH_HOST://复制来源和目标不能均为在宿主机的错误处理
		cerr << "Error：复制来源和目标不能均为在宿主机！\n";
		sstream_toshell << "Error：复制来源和目标不能均为在宿主机！\n";
		break;
	case HOST_FILE_NOT_EXIST://宿主机的文件不存在的错误处理
		cerr << "Error：宿主机不存在文件 " << strArg1 << "，文件复制失败！\n";
		sstream_toshell << "Error：宿主机不存在文件 " << strArg1 << "，文件复制失败！\n";
		break;
	case HOST_FILE_WRITE_FAILED://宿主机文件写入失败的错误处理
		cerr << "Error：宿主机写入文件 " << strArg1 << " 出错，文件复制失败！\n";
		sstream_toshell << "Error：宿主机写入文件 " << strArg1 << " 出错，文件复制失败！\n";
		break;
	case NO_DEL_CUR://当前目录不能删除的错误处理
		cerr << "Error：不能删除当前目录！\n";
		sstream_toshell << "Error：不能删除当前目录！\n";
	case WRONG_ARGS://参数输入错误的错误处理
		cerr << "Error：参数错误。如需帮助，请键入 " << strArg1 << " /?。" << endl;
		sstream_toshell << "Error：参数错误。如需帮助，请键入 " << strArg1 << " /?。\n";
		break;
	case USER_EXIST://创建用户时用户已存在的错误处理
		cerr << "Error：用户 " << strArg1 << " 已存在。\n";
		sstream_toshell << "Error：用户 " << strArg1 << " 已存在。\n";
		break;
	case USER_NOT_EXIST://删除用户时用户不存在的错误处理
		cerr << "Error：用户 " << strArg1 << " 不存在。\n";
		sstream_toshell << "Error：用户 " << strArg1 << " 不存在。\n";
		break;
	case PROTECTED_USER://不能删除或更改当前登录用户账号的错误处理
		cerr << "Error：不能删除或更改当前登录用户账号 " << strArg1 << "。\n";
		sstream_toshell << "Error：不能删除或更改当前登录用户账号 " << strArg1 << "。\n";
		break;
	case NO_PRIVILEGE_USERDEL://没有权限删除账号信息的错误处理
		cerr << "Error：没有权限删除账号信息" << endl;
		sstream_toshell << "Error：没有权限删除账号信息\n";
		break;
	case USERS_MAX://注册用户时用户数量已达到最大数量限制而无法注册的错误处理
		cerr << "Error：用户数量已达到最大数量限制 " << MAX_USER_NUM << "。\n";
		sstream_toshell << "Error：用户数量已达到最大数量限制 " << MAX_USER_NUM << "。\n";
		break;
	}
}