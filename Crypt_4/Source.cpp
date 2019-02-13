#include "Header.h"

void encryptMenu()
{
	string textFileName;
	string keyFileName;
	string IVName;				// Initialization vector(Синхропосылка)

	string text;
	string chipherText;
	unsigned int IV;
	unsigned int key;


	std::cout << "Enter file name with text: ";
	getline(std::cin, textFileName);
	std::cout << "Enter file name with key: ";
	getline(std::cin, keyFileName);
	std::cout << "Enter file name with initialization vector: ";
	getline(std::cin, IVName);

	readTextFromFile(textFileName, text);
	key = readKeyFromFile(keyFileName);
	IV = readKeyFromFile(IVName);

	encryptText(text, chipherText, key, &IV);
	saveTextToFile(textFileName + "_crypted.txt", chipherText);

	cout << endl << "Text: " << endl << text << endl;
	cout << endl << "Crypted text: " << endl << chipherText << endl;
}

void decryptMenu()
{
	string chipherTextFileName;
	string keyFileName;
	string IVName;				// Initialization vector(Синхропосылка)

	string text;
	string chipherText;
	unsigned int IV;
	unsigned int key;

	std::cout << "Enter file name with chiphertext: ";
	getline(std::cin, chipherTextFileName);
	std::cout << "Enter file name with key: ";
	getline(std::cin, keyFileName);
	std::cout << "Enter file name with initialization vector: ";
	getline(std::cin, IVName);

	readTextFromFile(chipherTextFileName, chipherText);
	key = readKeyFromFile(keyFileName);
	IV = readKeyFromFile(IVName);

	decryptText(chipherText, text, key, &IV);
	saveTextToFile(chipherTextFileName + "_decrypted.txt", text);

	cout << endl << "Crypted text: " << endl << chipherText << endl;
	cout << endl << "Decrypted text: " << endl << text << endl;
}

int main()
{
	string input;
	while (1)
	{
		std::cout << "Choose mode:\n1 - encrypr\n2 - decrypt\nMode: ";
		getline(std::cin, input);
		if (input == "1")
		{
			encryptMenu();
		}
		else if (input == "2")
		{
			decryptMenu();
		}
		else
		{
			cout << "ERROR: Invalue mod." << endl;
		}
	}
	getchar();
	return 1;
}