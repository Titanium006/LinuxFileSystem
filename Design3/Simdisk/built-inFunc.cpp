/*
built-inFunc.cpp
���ò����ĺ���
*/
#include"func.h"
//���ݿ��i-�ڵ�ķ�������ա���

//�������ݿ飬�����׵�ַ�����ؿ�ʼ�̿�����startblock_index
long alloc_blocks(unsigned int fileBlocks, unsigned int& startblock_index) {
	int Address = -1;
	if (sbGroups[0].allFreeBlocks < fileBlocks)return Address;
	int count = 0;//�ۼ����������̿�
	int i, j;
	int nAvailIndex = 0;		//�������ݿ�λ������
	int nBlockGroupIndex = 0;		//�׸����ݿ����λ������
	bool bBlockGroup[BLOCK_GROUPS_NUM];		//�����������Ϣ��Ҫ�޸�
	int nBlockGroupNum[BLOCK_GROUPS_NUM];	//���˶��ٿ�

	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		bBlockGroup[i] = false;
		nBlockGroupNum[i] = 0;
	}
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		for (j = 0; j < BLOCKS_EACH; j++)	//�������ݿ�
		{
			if (bsBlockBmp[(i*BLOCKS_EACH + j)] == NOT_USED)
			{
				count++;
				bBlockGroup[i] = true;
				nBlockGroupNum[i]++;
				if (count == 1)
				{
					Address =sbGroups[i].BlockAddr + j * BLOCK_SIZE;
					nAvailIndex = i * BLOCKS_EACH + j;
					nBlockGroupIndex = i;
					startblock_index = i * BLOCKS_EACH + j;
				}
			}
			else	//û��������
			{
				//��ԭ�����¿�ʼ
				count = 0;
				for (int k = nBlockGroupIndex;k <= i;k++) {
					bBlockGroup[k] = false;
					nBlockGroupNum[k] = 0;
				}
			}
			if (count == fileBlocks) break;
		}
		if (count == fileBlocks) break;
	}
	if (count != fileBlocks)
	{
		Address = -1;
		return Address;
	}
	//����ɹ������޸���Ӧ����Ϣ
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
		sbGroups[i].allFreeBlocks -= fileBlocks;
	j = nAvailIndex + fileBlocks;
	for (i = nAvailIndex;i < j; i++)
		bsBlockBmp[i] = USED;
	for (i = nBlockGroupIndex;i < BLOCK_GROUPS_NUM; i++)
		if (bBlockGroup[i]) sbGroups[i].FreeBlocksNum-= nBlockGroupNum[i];
	return Address;
}

//����i-�ڵ㣬���ط����i-�ڵ��
int alloc_inodes() {
	int i_index = -1;//i�ڵ��
	int bg_index;//��ȡi�ڵ����ڵ����ݿ�
	for (int i = 0;i < BLOCK_GROUPS_NUM;i++) {
		for (int j = 0;j < INODES_EACH;j++) {
			if (bsInodeBmp[i * BLOCK_GROUPS_NUM + j] == NOT_USED) {
				i_index = i * BLOCK_GROUPS_NUM + j;
				bg_index = i;
				break;
			}
		}
		if (i_index != -1) break;
	}
	//����ɹ������޸���Ӧ����Ϣ
	if (i_index != -1) {
		bsInodeBmp[i_index] = USED;
		for (int i = 0;i < BLOCK_GROUPS_NUM;i++)
			sbGroups[i].allFreeInodes -= 1;
		sbGroups[bg_index].FreeInodesNum -= 1;
	}
	return i_index;
}

//�������ݿ�
void release_blocks(unsigned int nIndex) {
	for (int i = 0;i < BLOCK_GROUPS_NUM;i++) {
		sbGroups[i].allFreeBlocks += indTable[nIndex].havingBlocks;
	}
	unsigned int startblock = (indTable[nIndex].BlockAddr - indTable[0].BlockAddr) / BLOCK_SIZE;
	for (unsigned int i = startblock;i < startblock + indTable[nIndex].havingBlocks;i++) {
		sbGroups[i / BLOCKS_EACH].FreeBlocksNum++;
		bsBlockBmp[i] = NOT_USED;
	}

}

void release_inodes(unsigned int nIndex) {
	for (int i = 0;i < BLOCK_GROUPS_NUM;i++) {
		sbGroups[i].allFreeInodes++;
	}
	bsInodeBmp[nIndex] = NOT_USED;
	sbGroups[nIndex / INODES_EACH].FreeInodesNum++;
}

//��·���������з�������

//�ж�strPath�Ƿ�ɴ�ɴ��򴫻ظ�Ŀ¼dirTemp
bool analyse_path(const char* strPath, Dir& dirTemp)
{
	dirTemp = cur_dir;	//��ʱĿ¼
	//��ʼ���·��
	char tem_dirName[MAX_NAME_LENGTH];//Ŀǰ�ļ���
	char left_dirName[MAX_NAME_LENGTH];//ʣ��Ŀ¼
	strcpy(left_dirName, strPath);
	vector<string> dirNames{};
	get_dirName(left_dirName, dirNames);

	unsigned int inodeNum = cur_dir.Inode_num;

	for (int i = 0; i < dirNames.size(); i++) {

		strcpy(tem_dirName, dirNames[i].c_str());
		if (!dirTemp.have_child(tem_dirName, inodeNum, TYPE_DIR)) {
			return false;
		}
		else {

			fs.open("Virtual_Disk.bin", ios::in | ios::binary);
			if (!fs.is_open())error(READ_FAILED);
			else {
				fs.seekg((indTable[inodeNum].BlockAddr), ios::beg);
				fs.read(reinterpret_cast<char*>(&dirTemp), sizeof(Dir));
				fs.close();
			}

		}
	}
	return true;
}

//�ж�strPath�еĵ����ڶ���Ŀ¼/�ļ��Ƿ�ɴ���������һ��Ŀ¼/�ļ���strFileName�����ص����ڶ���Ŀ¼dirTemp
bool analyse_path(const char* strPath, Dir& dirTemp, char* strFileName)
{
	dirTemp = cur_dir;	//��ʱĿ¼
	//��ʼ���·��
	char tem_dirName[MAX_NAME_LENGTH];//Ŀǰ�ļ���
	char left_dirName[MAX_NAME_LENGTH];//ʣ��Ŀ¼
	strcpy(left_dirName, strPath);
	vector<string> dirNames{};
	get_dirName(left_dirName, dirNames);


	unsigned int inodeNum = cur_dir.Inode_num;

	for (int i = 0; i < dirNames.size() - 1; i++) {

		strcpy(tem_dirName, dirNames[i].c_str());
		if (!dirTemp.have_child(tem_dirName, inodeNum, TYPE_DIR)) {
			return false;
		}
		else {

			fs.open("Virtual_Disk.bin", ios::in | ios::binary);
			if (!fs.is_open())error(READ_FAILED);
			else {
				fs.seekg((indTable[inodeNum].BlockAddr), ios::beg);
				fs.read(reinterpret_cast<char*>(&dirTemp), sizeof(Dir));
				fs.close();

			}
		}
	}
	if (strcmp(dirNames[dirNames.size() - 1].c_str(), "root") != 0)
		strcpy(strFileName, dirNames[dirNames.size() - 1].c_str());
	return true;
}


//��ʾ��ǰ��·�����ݹ����
void set_cur_path(Dir dirCurDir)
{
	Dir dirTemp = dirCurDir;
	//�ݹ���ң�ֱ���ҵ���Ŀ¼root
	if (dirCurDir.Inode_num != 0)
	{
		//���Ҹ�Ŀ¼
		fs.open("Virtual_Disk.bin", ios::in | ios::binary);
		if (!fs.is_open()) error(READ_FAILED);
		fs.seekg(indTable[dirCurDir.SubInode_num[1]].BlockAddr | ios::beg);
		fs.read(reinterpret_cast<char*>(&dirCurDir), sizeof(Dir));
		fs.close();
		set_cur_path(dirCurDir);
	}
	//���õ�ǰ·���ַ���
	if (dirTemp.Inode_num == 0)
	{
		strcpy(cur_dirName, "root");
	}
	else
	{
		stringstream ssStream;
		ssStream << cur_dirName << "/" << indTable[dirTemp.Inode_num].fileName;
		ssStream >> cur_dirName;
		ssStream.clear();
	}
}

//��ȡ·���ϵ�Ŀ¼��������������
void get_dirName(const char* strPath, vector<string>& dirNames) {
	char x[MAX_NAME_LENGTH];
	strcpy(x, strPath);
	char* name = strtok(x, " /");
	while (name != NULL) {
		dirNames.push_back(name);
		name = strtok(NULL, " /");
	}
	return;

}

//�жϸ�i-�ڵ�Ŷ�Ӧ��Ŀ¼�Ƿ�Ϊ��ǰĿ¼�����ڽڵ�
bool isAncestorOfcur_dir(unsigned int nIndex) {
	Dir dirTemp = cur_dir;
	//�ݹ���ң�ֱ���ҵ���Ŀ¼root
	while (dirTemp.Inode_num != 0)
	{
		//���Ҹ�Ŀ¼
		fs.open("Virtual_Disk.bin", ios::in | ios::binary);
		if (!fs.is_open()) error(READ_FAILED);
		fs.seekg(indTable[dirTemp.SubInode_num[1]].BlockAddr | ios::beg);
		fs.read(reinterpret_cast<char*>(&dirTemp), sizeof(Dir));
		fs.close();
		if (dirTemp.Inode_num == nIndex)return true;
	}
	return false;

}
//������ת��ΪСд
void mytolower(char* s) {
	int len = strlen(s);
	for (int i = 0; i < len; i++) {
		if (s[i] >= 'A' && s[i] <= 'Z') {
			s[i] += 32;//+32ת��ΪСд
			//s[i]=s[i]-'A'+'a';
		}
	}
}

