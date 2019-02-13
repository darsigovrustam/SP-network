#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#define D 4				//число тактов
#define BLOCK_SIZE 4	//размер блока M

using namespace std;


//encrypt.cpp

void encryptText(string & text, string & chipherText, unsigned int key, unsigned int *IV);

//decrypt.cpp

void decryptText(string & chipherText, string & text, unsigned int key, unsigned int *IV);

//basicFunctions.cpp
void readTextFromFile(string & fileName, string & text);
void saveTextToFile(string fileName, string & text);
int is_FileExist(string fileName);
unsigned int readKeyFromFile(string fileName);

unsigned int getRigthPart(unsigned int block, short partSize);
unsigned int getLeftPart(unsigned int block, short partSize);
void appendBlockToString(string & text, unsigned int block, int incompletenessFlag);
unsigned int getBlock(string & blockString);