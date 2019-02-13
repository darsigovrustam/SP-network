#include "Header.h"
vector<unsigned short> inverseBlockS = { 1, 8, 14, 5, 13, 7, 4, 11, 15, 2, 0, 12, 10, 9, 3, 6 };
void inverseTransformationS(unsigned int *block)
{
	unsigned short nibble = 0;	//4 бита
	unsigned int ret_block = 0;

	for (int i = 0; i < 32 / BLOCK_SIZE; i++)
	{
		ret_block = ret_block << 4;			//сместили вложенное число
		nibble = (*block << i * 4) >> 28;	//взяли нужные 4 бита
		ret_block += inverseBlockS[nibble];		//получили по ним число из S и вложили
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
		ret_block +=						//прибавили нужный бит к имеющемуся результату
			((((*block << index)			//убрали лишнее слева 
				>> index)			//убрали появившиеся нули справа
				>> 32 - index - 1)	//убрали лишние справа
				<< 31 - i);			//перенесли получившийся бит влево
	}

	*block = ret_block;
}

void inverseRaundFunction(unsigned int *block, unsigned int key)
{
	inversePermutation(block);			//Перестановка P: (29i + 31) mod 32
	inverseTransformationS(block);		//делим блок на 8 частей и проводим S-преобразования
	*block = *block ^ key;				//xor блока с K
}

void decryptBlock(unsigned int * block, unsigned int key, unsigned int *IV)
{
	unsigned int tmpIV = *block;

	// Отбелили
	*block = *block ^ key;
	for (int j = 0; j < D; j++)
	{
		//прогнали 4 раунда
		inverseRaundFunction(block, key);
	}
	// Ксор с синхропосылкой и ее перезапись
	*block = *IV ^ *block;
	*IV = tmpIV;
}

// Тоже самое, что функция выше, но синхропосылка не изменяется
void decryptLastBlocks(unsigned int * block, unsigned int key, unsigned int *IV)
{
	unsigned int tmpIV = *block;

	// Отбелили
	*block = *block ^ key;
	for (int j = 0; j < D; j++)
	{
		//прогнали 4 раунда
		inverseRaundFunction(block, key);
	}
	// Ксор с синхропосылкой и ее перезапись
	*block = *IV ^ *block;
}

//расшифровать весь текст
void decryptText(string & chipherText, string & text, unsigned int key, unsigned int *IV)
{

	string blockString;
	unsigned int block;
	unsigned int tmpBlock;	//Y*
	unsigned int lastBlock;	
	int position = 0;
	int blockCount = 0;		// на сколько 32-битных блоков (включая неполный) можно разбить текст
	int lastBlockLength;	// длина последнего блока
	unsigned int *tmp;

	//посчитали blockCount и lastBlockLength
	blockCount = chipherText.length() / BLOCK_SIZE;
	lastBlockLength = chipherText.length() % BLOCK_SIZE;
	if (lastBlockLength != 0)
	{
		//Если последний блок неполный, то считать будем N-2 блоков
		blockCount--;
	}

	//Получаем блок и разшифровываем (если последний блок неполный, то последние 2 блока отдельно)
	for (int i = 0; i < blockCount; i++, position += 4)
	{
		blockString = chipherText.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		decryptBlock(&block, key, IV);
		appendBlockToString(text, block, 0);
	}

	//Если последний блок не был кратен BLOCK_SIZE
	if ((chipherText.length() % BLOCK_SIZE) != 0)
	{
		// Вычисляем Y* - Получили предпоследний блок, расшифровали, взяли его правую часть
		blockString = chipherText.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		decryptLastBlocks(&block, key, IV);
		tmpBlock = getRigthPart(block, BLOCK_SIZE - lastBlockLength);
		lastBlock = getLeftPart(block, lastBlockLength);

		// Расшифровываем предпоследний блок - для этого получаем последний блок, дополяем его правой стороной прошлого блока и шифруем
		position += 4;
		blockString = chipherText.substr(position, BLOCK_SIZE);
		block = getBlock(blockString);
		block = block << 8 * (BLOCK_SIZE - lastBlockLength);
		block = block | tmpBlock;
		decryptLastBlocks(&block, key, IV);
		appendBlockToString(text, block, 0);

		// Добавили последний блок
		appendBlockToString(text, lastBlock, BLOCK_SIZE - lastBlockLength);
	}
}
