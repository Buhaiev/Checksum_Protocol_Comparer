/*The point of this project is to compare data checking protocols used in the Internet*/
#include <iostream>
#include "methods.h"
//typedef std::pair< std::function< std::vector<bool>(std::vector<bool>)>, std::function< std::array<int, 5>(std::vector<bool>, std::vector<bool>)>> functionPair;
functionPair parityBit = { parityBitEncode, parityBitCheck };
functionPair doubleParityBit = { doubleParityBitEncode, doubleParityBitCheck};
functionPair checksum16 = { checksumEncode16, checksumCheck16 };
functionPair doubleSend = { doubleSendEncode, doubleSendCheck };
functionPair tripleSend = { tripleSendEncode, tripleSendCheck };
functionPair crc32 = { crcEncode32, crcCheck32 };

std::vector<bool> getVectFromStr(std::string str) {
	std::vector<bool> res;
	for (int i = 0; i < str.size(); i++) {
		res.push_back(int(str[i])-48);
	}
	return res;
}

int main()
{
	srand(time(NULL));
//#ifdef _DEBUG
	auto allStart = std::chrono::steady_clock::now();
//#endif
	std::array<long long int, 5> totalResult = { 0 };
	for (int i = 0; i < NUMBER_OF_DATAGRAMS; i++) {
		std::array<long long int, 5> result = checkEfficiency(tripleSend);
		for (int i = 0; i < 5; i++) {
			totalResult[i] += result[i];
		}
	}
	std::cout << "Total bytes" << "    " << totalResult[0] << std::endl;
	std::cout << "Total True Positive" << "    " << totalResult[1] << "		Persentage: " <<  double(totalResult[1]) / NUMBER_OF_DATAGRAMS*100 << std::endl;
	std::cout << "Total True Negative" << "    " << totalResult[2] << "		Persentage: " << double(totalResult[2]) / NUMBER_OF_DATAGRAMS * 100 << std::endl;
	std::cout << "Total False Positive" << "    " << totalResult[3] << "		Persentage: " << double(totalResult[3]) / NUMBER_OF_DATAGRAMS * 100 << std::endl;
	std::cout << "Total False Negative" << "    " << totalResult[4] << "		Persentage: " << double(totalResult[4]) / NUMBER_OF_DATAGRAMS * 100 << std::endl;

//#ifdef _DEBUG
	auto allEnd = std::chrono::steady_clock::now();
	auto elSec = std::chrono::duration_cast<std::chrono::milliseconds>( allEnd - allStart);
	std::cout << "	" << elSec.count() << std::endl;
//#endif
}

