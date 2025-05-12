/*
simdisk.h中定义类的成员函数实现
*/

#include"func.h"

/*User类成员函数定义*/

//设置用户信息
void User::set_user_info(const char* UserID, const char* UserPWD, user_type user_tType) {
	strcpy_s(this->UserID, UserID);
	strcpy_s(this->UserPWD, UserPWD);
	this->user_tType = user_tType;
}

//获取用户类型
user_type User::get_user_type() {
	return this->user_tType;
}

//获取用户账号
const char* User::get_user_id() {
	return this->UserID;
}

//比较判断密码是否正确
bool User::cmp_pwd(const char* UserPWD) {
	if (strcmp(this->UserPWD, UserPWD) == 0)
		return true;
	else return false;
}

/*Time类的成员函数定义*/

//设置日期时间
void Time::set_time(tm tmDateTime)
{
	this->modify_time = tmDateTime;
}

//获取日期时间
tm Time::get_time()
{

	return this->modify_time;
}


/*Dir目录类成员函数的实现*/


//当前目录下是否有名为 strDirName 子目录/子文件
bool Dir::have_child(const char* strDirName, unsigned int& index, file_type type) {
	if (strcmp(strDirName, "root") == 0) { index = 0;return true; }
	if (strcmp(strDirName, ".") == 0) { index = SubInode_num[0];return true; }
	if (strcmp(strDirName, "..") == 0) { index = SubInode_num[1];return true; }
	for (int i = 2;i < child_num;i++) {
		if (strcmp(indTable[SubInode_num[i]].fileName, strDirName) == 0
			&& indTable[SubInode_num[i]].fileType == type) {
			index = SubInode_num[i];
			return true;
		}
	}

	return false;
}



//该目录是否已满最大子目录数
bool Dir::isFull() {
	if (child_num >= MAX_SUBITEM_NUM)return true;
	return false;
}

