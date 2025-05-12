/*
handle.cpp
���ִ�����
*/

#include"func.h"
#include"simdisk.h"
#include"ipc.h"

//��������
void error(error_num errorType, const char* strArg1, const char* strArg2) {
	switch (errorType) {
	case LOGON_FAILED://��¼ʧ�ܵĴ�����
		cout << "\n\n******************************" << endl;
		sstream_toshell << "\n\n******************************\n";
		cerr << "ȡ����¼�����򼴽��˳�����" << endl;
		sstream_toshell << "ȡ����¼�����򼴽��˳�����\n";
		Sleep(2000);
		break;
	case WRONG_PWD://����������εĴ�����
		cout << "\n\n******************************" << endl;
		sstream_toshell << "\n\n******************************\n";
		cerr << "�����������Ѵ� 3 �Σ����򼴽��˳�����" << endl;
		sstream_toshell << "�����������Ѵ� 3 �Σ����򼴽��˳�����\n";
		Sleep(2000);
		break;
	case TOO_MANY_ARGS://�����������Ĵ�����
		cerr << "Error���������ࡣ�������������� help /command+/? " << endl;
		sstream_toshell << "Error���������ࡣ�������������� help /command+/?\n ";
	case INVILID_CMD://������Ч����Ĵ�����
		cerr << "Error����Ч������ " << strArg1 << "���������������� help��\n";
		sstream_toshell << "Error����Ч������ " << strArg1 << "���������������� help��\n";
		break;

	case NO_PRIVILEGE://û�з���Ȩ��ʱ�Ĵ�����
		cerr << "Error��û��Ȩ�޷��� " << strArg1 << "��";
		sstream_toshell << "Error��û��Ȩ�޷��� " << strArg1 << "��";
		break;
	case DIR_READONLY://Ŀ¼ֻ���Ĵ�����
		cerr << "Error��Ŀ¼ " << strArg1 << " ֻ�����޷��޸ġ�";
		sstream_toshell << "Error��Ŀ¼ " << strArg1 << " ֻ�����޷��޸ġ�";
		break;
	case FILE_READONLY://�ļ�ֻ���Ĵ�����
		cerr << "Error���ļ� " << strArg1 << " ֻ�����޷�ɾ����";
		sstream_toshell << "Error���ļ� " << strArg1 << " ֻ�����޷�ɾ����";
		break;
	case DIR_WRITEONLY://Ŀ¼ֻд�Ĵ�����
		cerr << "Error��Ŀ¼ " << strArg1 << " ֻд���޷���ȡ��";
		sstream_toshell << "Error��Ŀ¼ " << strArg1 << " ֻд���޷���ȡ��";
		break;
	case FILE_WRITEONLY://�ļ�ֻд�Ĵ�����
		cerr << "Error���ļ� " << strArg1 << " ֻд���޷���ȡ��";
		sstream_toshell << "Error���ļ� " << strArg1 << " ֻд���޷���ȡ��";
		break;
	case READ_FAILED://�޷���ȡ������̵Ĵ�����
		cout << "\n\n******************************\n";
		sstream_toshell << "\n\n******************************\n";
		cerr << "��ȡģ�� Linux �ļ�ϵͳʧ�ܣ�\n";
		sstream_toshell << "��ȡģ�� Linux �ļ�ϵͳʧ�ܣ�\n";
		break;
	case WRITE_FAILED://�޷�д��������̵Ĵ�����
		cout << "\n\n******************************\n";
		sstream_toshell << "\n\n******************************\n";
		cerr << "д��ģ�� Linux �ļ�ϵͳʧ�ܣ�\n";
		sstream_toshell << "д��ģ�� Linux �ļ�ϵͳʧ�ܣ�\n";
		break;
	case PATH_NOT_FOUND://�Ҳ���ָ��·���Ĵ�����
		cerr << "Error���Ҳ���ָ��·��";
		sstream_toshell << "Error���Ҳ���ָ��·��";
		break;
	case FILE_EXIST://�����ļ�ʱ�ļ��Ѵ��ڵĴ�����
		cerr << "Error��Ŀ¼ " << strArg1 << " ���Ѵ�����Ϊ " << strArg2 << " ���ļ���Ŀ¼��";
		sstream_toshell << "Error��Ŀ¼ " << strArg1 << " ���Ѵ�����Ϊ " << strArg2 << " ���ļ���Ŀ¼��";
		break;
	case FILE_NOT_EXIST://ɾ���ļ�ʱ�ļ������ڵĴ�����
		cerr << "Error��Ŀ¼ " << strArg1 << " �²�������Ϊ " << strArg2 << " ���ļ���";
		sstream_toshell << "Error��Ŀ¼ " << strArg1 << " �²�������Ϊ " << strArg2 << " ���ļ���";
		break;
	case DIR_NOT_EXIST://ɾ��Ŀ¼ʱĿ¼�����ڵĴ�����
		cerr << "Error��Ŀ¼ " << strArg1 << " �����ڣ�";
		sstream_toshell << "Error��Ŀ¼ " << strArg1 << " �����ڣ�";
		break;
	case SPACE_NOT_ENOUGH://���̿ռ䲻��Ĵ�����
		cerr << "Error�����̿ռ䲻�㣡";
		sstream_toshell << "Error�����̿ռ䲻�㣡";
		break;
	case INODE_ALLOC_FAILED://i-������ʧ�ܵĴ�����
		cerr << "Error��i-�ڵ����ʧ�ܣ�";
		sstream_toshell << "Error��i-�ڵ����ʧ�ܣ�";

	case CD_FAILED://�л�Ŀ¼ʧ�ܵĴ�����
		cerr << "�л�Ŀ¼ʧ�ܡ�\n";
		sstream_toshell << "�л�Ŀ¼ʧ�ܡ�\n";
		break;
	case DIR_FAILED://��ʾĿ¼ʧ�ܵĴ�����
		cerr << "Ŀ¼��ʾʧ�ܣ�\n";
		sstream_toshell << "Ŀ¼��ʾʧ�ܣ�\n";
		break;
	case MD_FAILED://����Ŀ¼ʧ�ܵĴ�����
		cerr << "Ŀ¼����ʧ�ܣ�\n";
		sstream_toshell << "Ŀ¼����ʧ�ܣ�\n";
		break;
	case MD_MAX_FAILED://������Ŀ¼���ﵽ���ֵ���޷�����Ŀ¼�Ĵ�����
		cerr << "Ŀ¼����ʧ�ܣ���Ŀ¼���Ѵ����ֵ��\n";
		sstream_toshell << "Ŀ¼����ʧ�ܣ���Ŀ¼���Ѵ����ֵ��\n";
		break;

	case RD_FAILED://ɾ��Ŀ¼ʧ�ܵĴ�����
		cerr << "Ŀ¼ɾ��ʧ�ܣ�\n";
		sstream_toshell << "Ŀ¼ɾ��ʧ�ܣ�\n";
		break;
	case NEW_FILE_FAILED://�ļ�����ʧ�ܵĴ�����
		cerr << "�ļ�����ʧ�ܣ�\n";
		sstream_toshell << "�ļ�����ʧ�ܣ�\n";
		break;
	case CAT_FAILED://�ļ���ʧ�ܵĴ�����
		cerr << "�ļ���ʧ�ܣ�\n";
		sstream_toshell << "�ļ���ʧ�ܣ�\n";
		break;
	case DEL_FAILED://�ļ�ɾ��ʧ�ܵĴ�����
		cerr << "�ļ�ɾ��ʧ�ܣ�\n";
		sstream_toshell << "�ļ�ɾ��ʧ�ܣ�\n";
		break;
	case COPY_FAILED://�ļ�����ʧ�ܵĴ�����
		cerr << "�ļ�����ʧ�ܣ�\n";
		sstream_toshell << "�ļ�����ʧ�ܣ�\n";
		break;
	case ERROR_PWD://�����������Ĵ�����
		cerr << "\n�������\n";
		sstream_toshell << "\n�������\n";
		break;

	case NOT_BOTH_HOST://������Դ��Ŀ�겻�ܾ�Ϊ���������Ĵ�����
		cerr << "Error��������Դ��Ŀ�겻�ܾ�Ϊ����������\n";
		sstream_toshell << "Error��������Դ��Ŀ�겻�ܾ�Ϊ����������\n";
		break;
	case HOST_FILE_NOT_EXIST://���������ļ������ڵĴ�����
		cerr << "Error���������������ļ� " << strArg1 << "���ļ�����ʧ�ܣ�\n";
		sstream_toshell << "Error���������������ļ� " << strArg1 << "���ļ�����ʧ�ܣ�\n";
		break;
	case HOST_FILE_WRITE_FAILED://�������ļ�д��ʧ�ܵĴ�����
		cerr << "Error��������д���ļ� " << strArg1 << " �����ļ�����ʧ�ܣ�\n";
		sstream_toshell << "Error��������д���ļ� " << strArg1 << " �����ļ�����ʧ�ܣ�\n";
		break;
	case NO_DEL_CUR://��ǰĿ¼����ɾ���Ĵ�����
		cerr << "Error������ɾ����ǰĿ¼��\n";
		sstream_toshell << "Error������ɾ����ǰĿ¼��\n";
	case WRONG_ARGS://�����������Ĵ�����
		cerr << "Error������������������������ " << strArg1 << " /?��" << endl;
		sstream_toshell << "Error������������������������ " << strArg1 << " /?��\n";
		break;
	case USER_EXIST://�����û�ʱ�û��Ѵ��ڵĴ�����
		cerr << "Error���û� " << strArg1 << " �Ѵ��ڡ�\n";
		sstream_toshell << "Error���û� " << strArg1 << " �Ѵ��ڡ�\n";
		break;
	case USER_NOT_EXIST://ɾ���û�ʱ�û������ڵĴ�����
		cerr << "Error���û� " << strArg1 << " �����ڡ�\n";
		sstream_toshell << "Error���û� " << strArg1 << " �����ڡ�\n";
		break;
	case PROTECTED_USER://����ɾ������ĵ�ǰ��¼�û��˺ŵĴ�����
		cerr << "Error������ɾ������ĵ�ǰ��¼�û��˺� " << strArg1 << "��\n";
		sstream_toshell << "Error������ɾ������ĵ�ǰ��¼�û��˺� " << strArg1 << "��\n";
		break;
	case NO_PRIVILEGE_USERDEL://û��Ȩ��ɾ���˺���Ϣ�Ĵ�����
		cerr << "Error��û��Ȩ��ɾ���˺���Ϣ" << endl;
		sstream_toshell << "Error��û��Ȩ��ɾ���˺���Ϣ\n";
		break;
	case USERS_MAX://ע���û�ʱ�û������Ѵﵽ����������ƶ��޷�ע��Ĵ�����
		cerr << "Error���û������Ѵﵽ����������� " << MAX_USER_NUM << "��\n";
		sstream_toshell << "Error���û������Ѵﵽ����������� " << MAX_USER_NUM << "��\n";
		break;
	}
}