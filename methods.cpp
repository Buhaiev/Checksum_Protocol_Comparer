#include "methods.h"

std::vector<bool> simpleCorrupt(std::vector<bool> text) {
	for (int i = 0; i < text.size(); i++) {
		if (rand() % CORRUPT_CHANCE == 0) {
			text[i] = !(text[i]);
		}
	}
	return text;
}

std::string generateText() {
	std::string res = "";
	int len = rand() % MAX_TEXT_SIZE+1;
	for (int i = 0; i < len; i++) {
		char symb = rand() % 60+65;   //covers letters+ some specials in ASCII table
		res += symb;
	}
	return res;
}

std::vector<bool> textToBinary(std::string text) {
	std::vector<bool> res = {};
	for (char symb : text) {
		int iter = 128;
		do {
			res.push_back(bool(symb / iter));
			symb %= iter;
			iter /= 2;
		} while (iter >= 1);
	}
	return res;
}

std::string binaryToText(std::vector<bool> binText) {
	std::string res = "";
	for (int byte = 0; byte < binText.size(); byte += 8) {
		char symb = 0;
		int iter = 128;
		for (int bit = 0; bit < 8; bit++) {
			symb += iter * binText[byte + bit];
			iter /= 2;
		}
		res += symb;
	}
	return res;
}

std::vector<bool> parityBit(std::vector<bool> textToProtect) {
	for (int byte = 0; byte < textToProtect.size(); byte += 8) {
		int sum = 0;
		for (int bit = 0; bit < 8; bit++) {
			sum += textToProtect[byte + bit];
		}
		textToProtect[byte] = sum % 2;
	}
	return textToProtect;
}

std::array<int, 5> checkParityBit(std::vector<bool> protectedAndCorrupted, std::vector<bool> trueText) {
	std::array<int, 5> res = { 0 };
	res[0] = protectedAndCorrupted.size() / 8;
	std::cout << protectedAndCorrupted.size() << " " << trueText.size() << std::endl;
	for (int byte = 0; byte < protectedAndCorrupted.size(); byte += 8) {
		int sum = 0;
		bool stillTrue = true;
		for (int bit = 0; bit < 8; bit++) {
			sum += protectedAndCorrupted[byte + bit];
			if (bit > 0) {
				if (protectedAndCorrupted[byte + bit] != trueText[byte + bit]) {
					stillTrue = false;
				}
			}
		}
		bool parityKept = !bool(sum % 2);
		if (stillTrue && parityKept) {
			res[1]++;
		}
		else if (!(stillTrue) && !(parityKept)){
			res[2]++;
		}
		else if (!(stillTrue) && parityKept){
			res[3]++;
		}
		else if (stillTrue && !(parityKept)){
			res[4]++;
		}
	}
	return res;
}