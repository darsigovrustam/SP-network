#include "Header.h"
vector<unsigned short> blockS = { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 };

void transformationS(unsigned int *block)
{
	unsigned short nibble = 0;	//4 бита
	unsigned int ret_block = 0;

	for (int i = 0; i < 32 / BLOCK_SIZE; i++)
	{
		ret_block = ret_block << 4;			//сместили вложенное число
		nibble = (*block << i * 4) >> 28;	//взяли нужные 4 бита
		ret_block += blockS[nibble];		//получили по ним число из S и вложили
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
		ret_block +=						//прибавили нужный бит к имеющемуся результату
			((((*block << index)			//убрали лишнее слева 
				>> index)			//убрали появившиеся нули справа
				>> 32 - index - 1)	//убрали лишние справа
				<< 31 - i);			//перенесли получившийся бит влево
	}

	*block = ret_block;
}

void raundFunction(unsigned int *block, unsigned int key)
{
	*block = *block ^ key;				//xor блока с K
	transformationS(block);		//делим блок на 8 частей и проводим S-преобразования
	permutation(block);					//Перестановка P: (29i + 31) mod 32
}

void encryptBlock(unsigned int * block, unsigned int key, unsigned int *IV)
{
	// Ксорим с синхропосылкой
	*block = *IV ^ *block;

	for (int j = 0; j < D; j++)
	{
		//прогнали 4 раунда
		raundFunction(block, key);
	}
	//отбелили
	*block = *block ^ key;

	// Изменяем синхропосылку
	*IV = *block;
}

// Тоже самое, что функция выше, но синхропосылка не изменяется
void encryptLastBlockS(unsigned int * block, unsigned int key, unsigned int *IV)
{
	// Ксорим с синхропосылкой
	*block = *IV ^ *block;

	for (int j = 0; j < D; j++)
	{
		//прогнали 4 раунда
		raundFunction(block, key);
	}
	//отбелили
	*block = *block ^ key;
}

// Зашифровать весь текст
void encryptText(string & text, string & chipherText, unsigned int key, unsigned int *IV)
{
	string blockString;
	unsigned int block;
	unsigned int tmpBlock;	//Y*
	unsigned int lastBlock;	//Y*
	int position = 0;
	int blockCount = 0;		// на сколько 32-битных блоков (включая неполный) можно разбить текст
	int lastBlockLength;	//длина последнего блока

	//посчитали blockCount и lastBlockLength
	blockCount = text.length() / BLOCK_SIZE;
	lastBlockLength = text.length() % BLOCK_SIZE;
	if (lastBlockLength != 0)
	{
		//Если последний блок неполный, то считать будем N-2 блоков
		blockCount--;
	}

	//Получаем блок, ксорим его с синхропосылкой и зашифровываем (если последний блок неполный, то последние 2 блока не шифруются)
	for (int i = 0; i < blockCount; i++, position += 4)
	{
		blockString = text.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);	
		encryptBlock(&block, key, IV);
		appendBlockToString(chipherText, block, 0);
	}

	//Если последний блок не был кратен BLOCK_SIZE
	if ((text.length() % BLOCK_SIZE) != 0)
	{
		// Вычисляем Y* - Получили предпоследний блок, зашифровали, взяли его правую часть/ Сразу взяли левую, т.к. это она будет ласт блоком
		blockString = text.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		encryptLastBlockS(&block, key, IV);
		tmpBlock = getRigthPart(block, BLOCK_SIZE - lastBlockLength);
		lastBlock = getLeftPart(block, lastBlockLength);

		// Зашифровываем предпоследний блок - для этого получаем последний блок, дополяем его правой стороной прошлого блока и шифруем
		position += 4;
		blockString = text.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		block = block << 8 * (BLOCK_SIZE - lastBlockLength);
		block = block | tmpBlock;
		encryptLastBlockS(&block, key, IV);
		appendBlockToString(chipherText, block, 0);

		// Добавили последний блок
		appendBlockToString(chipherText, lastBlock, BLOCK_SIZE - lastBlockLength);
	}
}