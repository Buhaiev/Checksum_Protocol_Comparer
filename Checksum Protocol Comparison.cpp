/*The point of this project is to compare data checking protocols used in the Internet*/
#include <iostream>
#include "methods.h"

int main()
{
	/*for (int i = 0; i < 10; i++) {
		std::string text = generateText();
		std::cout << text << "	";
		std::vector<bool> binText = textToBinary(text);
		for (int i = 0; i < binText.size(); i++) {
			if (((i) % 8 == 0)&&(i!=0)) std::cout << ".";
			std::cout << binText[i];
		}
		std::string renewedText = binaryToText(binText);
		std::cout << "	" << renewedText;
		std::cout << std::endl;
	}*/
	std::array<int, 5> totalResult = { 0 };
	for (int i = 0; i < 20; i++) {
		std::string text = generateText();
		std::cout << text << std::endl;
		std::vector<bool> binText = textToBinary(text);
		for (int i = 0; i < binText.size(); i++) {
			if (((i) % 8 == 0) && (i != 0)) std::cout << ".";
			std::cout << binText[i];
		}
		std::cout << "	" << "Simple" << std::endl;

		std::vector<bool> protectedText = parityBit(binText);
		for (int i = 0; i < protectedText.size(); i++) {
			if (((i) % 8 == 0) && (i != 0)) std::cout << ".";
			std::cout << protectedText[i];
		}
		std::cout << "	" << "Protected" << std::endl;

		std::vector<bool> corruptedText = simpleCorrupt(protectedText);
		for (int i = 0; i < corruptedText.size(); i++) {
			if (((i) % 8 == 0) && (i != 0)) std::cout << ".";
			std::cout << corruptedText[i];
		}
		std::cout << "	"<<"Corrupt"<<std::endl;

		std::array<int, 5> result = checkParityBit(corruptedText, binText);
		for (int i = 0; i < 5; i++) {
			totalResult[i] += result[i];
		}
		std::cout << "Bytes" <<"    "<< result[0] << std::endl;
		std::cout << "True Positive" << "    " << result[1] << std::endl;
		std::cout << "True Negative" << "    " << result[2] << std::endl;
		std::cout << "False Positive" << "    " << result[3] << std::endl;
		std::cout << "False Negative" << "    " << result[4] << std::endl;
		//Text renewal for later or just not needed
		/*std::string renewedText = binaryToText(corruptedText);
		std::cout << renewedText;
		std::cout << std::endl;
		std::cout << std::endl;*/
	}
	std::cout << "Total bytes" << "    " << totalResult[0] << std::endl;
	std::cout << "Total True Positive" << "    " << totalResult[1] << std::endl;
	std::cout << "Total True Negative" << "    " << totalResult[2] << std::endl;
	std::cout << "Total False Positive" << "    " << totalResult[3] << std::endl;
	std::cout << "Total False Negative" << "    " << totalResult[4] << std::endl;
}
