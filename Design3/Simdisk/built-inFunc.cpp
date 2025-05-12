/*
built-inFunc.cpp
内置操作的函数
*/
#include"func.h"
//数据块和i-节点的分配与回收——

//分配数据块，返回首地址，传回开始盘块索引startblock_index
long alloc_blocks(unsigned int fileBlocks, unsigned int& startblock_index) {
	int Address = -1;
	if (sbGroups[0].allFreeBlocks < fileBlocks)return Address;
	int count = 0;//累计连续空闲盘块
	int i, j;
	int nAvailIndex = 0;		//可用数据块位置索引
	int nBlockGroupIndex = 0;		//首个数据块组的位置索引
	bool bBlockGroup[BLOCK_GROUPS_NUM];		//数据组的组信息需要修改
	int nBlockGroupNum[BLOCK_GROUPS_NUM];	//用了多少块

	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		bBlockGroup[i] = false;
		nBlockGroupNum[i] = 0;
	}
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		for (j = 0; j < BLOCKS_EACH; j++)	//连续数据块
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
			else	//没有连续的
			{
				//还原，重新开始
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
	//分配成功，则修改相应的信息
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
		sbGroups[i].allFreeBlocks -= fileBlocks;
	j = nAvailIndex + fileBlocks;
	for (i = nAvailIndex;i < j; i++)
		bsBlockBmp[i] = USED;
	for (i = nBlockGroupIndex;i < BLOCK_GROUPS_NUM; i++)
		if (bBlockGroup[i]) sbGroups[i].FreeBlocksNum-= nBlockGroupNum[i];
	return Address;
}

//分配i-节点，传回分配的i-节点号
int alloc_inodes() {
	int i_index = -1;//i节点号
	int bg_index;//所取i节点所在的数据块
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
	//分配成功，则修改相应的信息
	if (i_index != -1) {
		bsInodeBmp[i_index] = USED;
		for (int i = 0;i < BLOCK_GROUPS_NUM;i++)
			sbGroups[i].allFreeInodes -= 1;
		sbGroups[bg_index].FreeInodesNum -= 1;
	}
	return i_index;
}

//回收数据块
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

//对路径参数进行分析——

//判断strPath是否可达，可达则传回该目录dirTemp
bool analyse_path(const char* strPath, Dir& dirTemp)
{
	dirTemp = cur_dir;	//临时目录
	//开始拆分路径
	char tem_dirName[MAX_NAME_LENGTH];//目前文件夹
	char left_dirName[MAX_NAME_LENGTH];//剩余目录
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

//判断strPath中的倒数第二个目录/文件是否可达，并传回最后一个目录/文件名strFileName，传回倒数第二个目录dirTemp
bool analyse_path(const char* strPath, Dir& dirTemp, char* strFileName)
{
	dirTemp = cur_dir;	//临时目录
	//开始拆分路径
	char tem_dirName[MAX_NAME_LENGTH];//目前文件夹
	char left_dirName[MAX_NAME_LENGTH];//剩余目录
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


//显示当前的路径，递归查找
void set_cur_path(Dir dirCurDir)
{
	Dir dirTemp = dirCurDir;
	//递归查找，直到找到根目录root
	if (dirCurDir.Inode_num != 0)
	{
		//查找父目录
		fs.open("Virtual_Disk.bin", ios::in | ios::binary);
		if (!fs.is_open()) error(READ_FAILED);
		fs.seekg(indTable[dirCurDir.SubInode_num[1]].BlockAddr | ios::beg);
		fs.read(reinterpret_cast<char*>(&dirCurDir), sizeof(Dir));
		fs.close();
		set_cur_path(dirCurDir);
	}
	//设置当前路径字符串
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

//获取路径上的目录名并存入数组中
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

//判断该i-节点号对应的目录是否为当前目录的祖宗节点
bool isAncestorOfcur_dir(unsigned int nIndex) {
	Dir dirTemp = cur_dir;
	//递归查找，直到找到根目录root
	while (dirTemp.Inode_num != 0)
	{
		//查找父目录
		fs.open("Virtual_Disk.bin", ios::in | ios::binary);
		if (!fs.is_open()) error(READ_FAILED);
		fs.seekg(indTable[dirTemp.SubInode_num[1]].BlockAddr | ios::beg);
		fs.read(reinterpret_cast<char*>(&dirTemp), sizeof(Dir));
		fs.close();
		if (dirTemp.Inode_num == nIndex)return true;
	}
	return false;

}
//将命令转换为小写
void mytolower(char* s) {
	int len = strlen(s);
	for (int i = 0; i < len; i++) {
		if (s[i] >= 'A' && s[i] <= 'Z') {
			s[i] += 32;//+32转换为小写
			//s[i]=s[i]-'A'+'a';
		}
	}
}

