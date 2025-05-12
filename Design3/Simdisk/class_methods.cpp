/*
simdisk.h�ж�����ĳ�Ա����ʵ��
*/

#include"func.h"

/*User���Ա��������*/

//�����û���Ϣ
void User::set_user_info(const char* UserID, const char* UserPWD, user_type user_tType) {
	strcpy_s(this->UserID, UserID);
	strcpy_s(this->UserPWD, UserPWD);
	this->user_tType = user_tType;
}

//��ȡ�û�����
user_type User::get_user_type() {
	return this->user_tType;
}

//��ȡ�û��˺�
const char* User::get_user_id() {
	return this->UserID;
}

//�Ƚ��ж������Ƿ���ȷ
bool User::cmp_pwd(const char* UserPWD) {
	if (strcmp(this->UserPWD, UserPWD) == 0)
		return true;
	else return false;
}

/*Time��ĳ�Ա��������*/

//��������ʱ��
void Time::set_time(tm tmDateTime)
{
	this->modify_time = tmDateTime;
}

//��ȡ����ʱ��
tm Time::get_time()
{

	return this->modify_time;
}


/*DirĿ¼���Ա������ʵ��*/


//��ǰĿ¼���Ƿ�����Ϊ strDirName ��Ŀ¼/���ļ�
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



//��Ŀ¼�Ƿ����������Ŀ¼��
bool Dir::isFull() {
	if (child_num >= MAX_SUBITEM_NUM)return true;
	return false;
}

