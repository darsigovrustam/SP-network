#include "Header.h"
vector<unsigned short> blockS = { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 };

void transformationS(unsigned int *block)
{
	unsigned short nibble = 0;	//4 ����
	unsigned int ret_block = 0;

	for (int i = 0; i < 32 / BLOCK_SIZE; i++)
	{
		ret_block = ret_block << 4;			//�������� ��������� �����
		nibble = (*block << i * 4) >> 28;	//����� ������ 4 ����
		ret_block += blockS[nibble];		//�������� �� ��� ����� �� S � �������
	}
	*block = ret_block;
}

void permutation(unsigned int *block)
{
	int index;
	unsigned int ret_block = 0;

	for (int i = 0; i < 32; i++)
	{
		index = (29 * i + 31) % 32;
		ret_block +=						//��������� ������ ��� � ���������� ����������
			((((*block << index)			//������ ������ ����� 
				>> index)			//������ ����������� ���� ������
				>> 32 - index - 1)	//������ ������ ������
				<< 31 - i);			//��������� ������������ ��� �����
	}

	*block = ret_block;
}

void raundFunction(unsigned int *block, unsigned int key)
{
	*block = *block ^ key;				//xor ����� � K
	transformationS(block);		//����� ���� �� 8 ������ � �������� S-��������������
	permutation(block);					//������������ P: (29i + 31) mod 32
}

void encryptBlock(unsigned int * block, unsigned int key, unsigned int *IV)
{
	// ������ � ��������������
	*block = *IV ^ *block;

	for (int j = 0; j < D; j++)
	{
		//�������� 4 ������
		raundFunction(block, key);
	}
	//��������
	*block = *block ^ key;

	// �������� �������������
	*IV = *block;
}

// ���� �����, ��� ������� ����, �� ������������� �� ����������
void encryptLastBlockS(unsigned int * block, unsigned int key, unsigned int *IV)
{
	// ������ � ��������������
	*block = *IV ^ *block;

	for (int j = 0; j < D; j++)
	{
		//�������� 4 ������
		raundFunction(block, key);
	}
	//��������
	*block = *block ^ key;
}

// ����������� ���� �����
void encryptText(string & text, string & chipherText, unsigned int key, unsigned int *IV)
{
	string blockString;
	unsigned int block;
	unsigned int tmpBlock;	//Y*
	unsigned int lastBlock;	//Y*
	int position = 0;
	int blockCount = 0;		// �� ������� 32-������ ������ (������� ��������) ����� ������� �����
	int lastBlockLength;	//����� ���������� �����

	//��������� blockCount � lastBlockLength
	blockCount = text.length() / BLOCK_SIZE;
	lastBlockLength = text.length() % BLOCK_SIZE;
	if (lastBlockLength != 0)
	{
		//���� ��������� ���� ��������, �� ������� ����� N-2 ������
		blockCount--;
	}

	//�������� ����, ������ ��� � �������������� � ������������� (���� ��������� ���� ��������, �� ��������� 2 ����� �� ���������)
	for (int i = 0; i < blockCount; i++, position += 4)
	{
		blockString = text.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);	
		encryptBlock(&block, key, IV);
		appendBlockToString(chipherText, block, 0);
	}

	//���� ��������� ���� �� ��� ������ BLOCK_SIZE
	if ((text.length() % BLOCK_SIZE) != 0)
	{
		// ��������� Y* - �������� ������������� ����, �����������, ����� ��� ������ �����/ ����� ����� �����, �.�. ��� ��� ����� ���� ������
		blockString = text.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		encryptLastBlockS(&block, key, IV);
		tmpBlock = getRigthPart(block, BLOCK_SIZE - lastBlockLength);
		lastBlock = getLeftPart(block, lastBlockLength);

		// ������������� ������������� ���� - ��� ����� �������� ��������� ����, �������� ��� ������ �������� �������� ����� � �������
		position += 4;
		blockString = text.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		block = block << 8 * (BLOCK_SIZE - lastBlockLength);
		block = block | tmpBlock;
		encryptLastBlockS(&block, key, IV);
		appendBlockToString(chipherText, block, 0);

		// �������� ��������� ����
		appendBlockToString(chipherText, lastBlock, BLOCK_SIZE - lastBlockLength);
	}
}