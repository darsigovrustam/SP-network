#include "Header.h"


////////////////////////////////////////////������ � �������/////////////////////////////////////////////////
void readTextFromFile(string & fileName, string & text)
{
	while (!is_FileExist(fileName))
	{
		cout << fileName << " does not exist, try again: ";
		getline(cin, fileName);
	}
	//Read text from file by name
	ifstream file(fileName, ios::binary);
	text = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	file.close();
}

void saveTextToFile(string fileName, string & text)
{
	ofstream file(fileName, ios::binary);
	file << text;
	file.close();
}

int is_FileExist(string fileName)
{
	ifstream file(fileName);
	if (!file)
	{
		return 0;
	}
	file.close();
	return 1;
}

// ��������� 32-������ ����
unsigned int readKeyFromFile(string fileName)
{
	string data;
	unsigned int key = 0;

	ifstream file(fileName, ios::binary);
	data = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	file.close();

	for (auto byte : data)
	{
		key = key << 8;
		key |= byte;
	}

	return key;
}

////////////////////////////////////////////SP-�������������/////////////////////////////////////////////////

unsigned int getRigthPart(unsigned int block, short partSize)
{
	//�������� ������ ����� (������� �� ������� ���� �����)
	return (block << (BLOCK_SIZE - partSize) * 8) >> (BLOCK_SIZE - partSize) * 8;
}

unsigned int getLeftPart(unsigned int block, short partSize)
{
	//�������� ����� ����� (����� ���� �����)
	return (block >> (BLOCK_SIZE - partSize) * 8) << (BLOCK_SIZE - partSize) * 8;
}

void appendBlockToString(string & text, unsigned int block, int incompletenessFlag)
{
	//���� ��������� ����� ���������� ����������� ������
	//�������� ���� � ����������, ���� ���� ��������� ����� ������ 0, �� ���� ������ �� ����
	for (int i = 0; i < BLOCK_SIZE - incompletenessFlag; i++)
	{
		text +=
			(unsigned char)(((block << i * 8)		//������ ������ �����
				>> i * 8)		//������ ����������� ���� ������
				>> (3 - i) * 8);	//������ ������ ������
	}
}

unsigned int getBlock(string & blockString)
{
	//�������� �� ������ 32-������ ���� ������
	unsigned int block = 0;

	for (unsigned char letter : blockString)
	{
		block = block << 8;
		block += letter;
	}
	return block;
}
