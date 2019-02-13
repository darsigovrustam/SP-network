#include "Header.h"
vector<unsigned short> inverseBlockS = { 1, 8, 14, 5, 13, 7, 4, 11, 15, 2, 0, 12, 10, 9, 3, 6 };
void inverseTransformationS(unsigned int *block)
{
	unsigned short nibble = 0;	//4 ����
	unsigned int ret_block = 0;

	for (int i = 0; i < 32 / BLOCK_SIZE; i++)
	{
		ret_block = ret_block << 4;			//�������� ��������� �����
		nibble = (*block << i * 4) >> 28;	//����� ������ 4 ����
		ret_block += inverseBlockS[nibble];		//�������� �� ��� ����� �� S � �������
	}
	*block = ret_block;
}

void inversePermutation(unsigned int *block)
{
	int index;
	unsigned int ret_block = 0;

	for (int i = 0; i < 32; i++)
	{
		index = 21 * (i + 1) % 32;
		ret_block +=						//��������� ������ ��� � ���������� ����������
			((((*block << index)			//������ ������ ����� 
				>> index)			//������ ����������� ���� ������
				>> 32 - index - 1)	//������ ������ ������
				<< 31 - i);			//��������� ������������ ��� �����
	}

	*block = ret_block;
}

void inverseRaundFunction(unsigned int *block, unsigned int key)
{
	inversePermutation(block);			//������������ P: (29i + 31) mod 32
	inverseTransformationS(block);		//����� ���� �� 8 ������ � �������� S-��������������
	*block = *block ^ key;				//xor ����� � K
}

void decryptBlock(unsigned int * block, unsigned int key, unsigned int *IV)
{
	unsigned int tmpIV = *block;

	// ��������
	*block = *block ^ key;
	for (int j = 0; j < D; j++)
	{
		//�������� 4 ������
		inverseRaundFunction(block, key);
	}
	// ���� � �������������� � �� ����������
	*block = *IV ^ *block;
	*IV = tmpIV;
}

// ���� �����, ��� ������� ����, �� ������������� �� ����������
void decryptLastBlocks(unsigned int * block, unsigned int key, unsigned int *IV)
{
	unsigned int tmpIV = *block;

	// ��������
	*block = *block ^ key;
	for (int j = 0; j < D; j++)
	{
		//�������� 4 ������
		inverseRaundFunction(block, key);
	}
	// ���� � �������������� � �� ����������
	*block = *IV ^ *block;
}

//������������ ���� �����
void decryptText(string & chipherText, string & text, unsigned int key, unsigned int *IV)
{

	string blockString;
	unsigned int block;
	unsigned int tmpBlock;	//Y*
	unsigned int lastBlock;	
	int position = 0;
	int blockCount = 0;		// �� ������� 32-������ ������ (������� ��������) ����� ������� �����
	int lastBlockLength;	// ����� ���������� �����
	unsigned int *tmp;

	//��������� blockCount � lastBlockLength
	blockCount = chipherText.length() / BLOCK_SIZE;
	lastBlockLength = chipherText.length() % BLOCK_SIZE;
	if (lastBlockLength != 0)
	{
		//���� ��������� ���� ��������, �� ������� ����� N-2 ������
		blockCount--;
	}

	//�������� ���� � �������������� (���� ��������� ���� ��������, �� ��������� 2 ����� ��������)
	for (int i = 0; i < blockCount; i++, position += 4)
	{
		blockString = chipherText.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		decryptBlock(&block, key, IV);
		appendBlockToString(text, block, 0);
	}

	//���� ��������� ���� �� ��� ������ BLOCK_SIZE
	if ((chipherText.length() % BLOCK_SIZE) != 0)
	{
		// ��������� Y* - �������� ������������� ����, ������������, ����� ��� ������ �����
		blockString = chipherText.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		decryptLastBlocks(&block, key, IV);
		tmpBlock = getRigthPart(block, BLOCK_SIZE - lastBlockLength);
		lastBlock = getLeftPart(block, lastBlockLength);

		// �������������� ������������� ���� - ��� ����� �������� ��������� ����, �������� ��� ������ �������� �������� ����� � �������
		position += 4;
		blockString = chipherText.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		block = block << 8 * (BLOCK_SIZE - lastBlockLength);
		block = block | tmpBlock;
		decryptLastBlocks(&block, key, IV);
		appendBlockToString(text, block, 0);

		// �������� ��������� ����
		appendBlockToString(text, lastBlock, BLOCK_SIZE - lastBlockLength);
	}
}
