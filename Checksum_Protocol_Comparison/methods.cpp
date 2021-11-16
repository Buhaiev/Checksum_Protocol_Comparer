#include "methods.h"

std::deque<bool> generateBinaryText() {
	std::deque<bool> res;
	res.resize(MAX_TEXT_SIZE * 8);
	for (int byte = 0; byte < MAX_TEXT_SIZE * 8; byte += 8) {
		res[byte] = false;
		for (int bit = 1; bit < 8; bit++) {
			res[byte + bit] = bool(rand() % 2);
		}
	}
	return res;
}

std::deque<bool> simpleCorrupt(std::deque<bool> text) {
	for (int i = 0; i < text.size(); i++) {
		if (rand() % CORRUPT_CHANCE == 0) {
			text[i] = !(text[i]);
		}
	}
	return text;
}

std::deque<bool> parityBitEncode(std::deque<bool> textToProtect) {
	for (int byte = 0; byte < MAX_TEXT_SIZE * 8; byte += 8) {
		bool sum = false;
		for (int bit = 0; bit < 8; bit++) {
			sum ^= textToProtect[byte + bit];
		}
		textToProtect[byte] = sum;
	}
	return textToProtect;
}

std::array<int64_t, 5> parityBitCheck(std::deque<bool> protectedAndCorrupted, std::deque<bool> trueText) {
	std::array<int64_t, 5> res = { 0 };
	bool textTrue = true;
	bool flaggedAsCorrect = true;
	res[0] = MAX_TEXT_SIZE;
	for (int byte = 0; byte < MAX_TEXT_SIZE * 8; byte += 8) {
		bool sum = 0;
		for (int bit = 0; bit < 8; bit++) {
			sum ^= protectedAndCorrupted[byte + bit];
			if ((bit > 0) && (protectedAndCorrupted[byte + bit] != trueText[byte + bit])) {
				textTrue = false;
			}
		}
		if (sum) flaggedAsCorrect = false;
	}
	if (textTrue && flaggedAsCorrect) res[1]++;
	else if (!textTrue && !flaggedAsCorrect) res[2]++;
	else if (!textTrue && flaggedAsCorrect) res[3]++;
	else if (textTrue && !flaggedAsCorrect) res[4]++;
	return res;
}

std::deque<bool> doubleParityBitEncode(std::deque<bool> textToProtect) {
	int newSize = ceil(double(textToProtect.size() / 8) / 7) * 8 + textToProtect.size();
	std::deque<bool> res;
	res.resize(newSize);
	textToProtect = parityBitEncode(textToProtect);

	for (int byteBlock = 0; byteBlock < newSize; byteBlock += 64) {
		bool colSums[8] = { 0 };
		for (int byte = 0; ((byte < 64) && (byteBlock + byte < newSize)); byte += 8) {
			if ((byte == 7 * 8) || (byteBlock + byte == newSize - 8)) {
				for (int bit = 0; bit < 8; bit++) {
					res[byteBlock + byte + bit] = colSums[bit];
					colSums[bit] = 0;
				}
			}
			else {
				for (int bit = 0; bit < 8; bit++) {
					res[byteBlock + byte + bit] = (textToProtect)[byteBlock / 8 * 7 + byte + bit];
					colSums[bit] ^= (textToProtect)[byteBlock / 8 * 7 + byte + bit];
				}
			}
		}
	}
	return res;
}

std::array<int64_t, 5> doubleParityBitCheck(std::deque<bool> protectedAndCorrupted, std::deque<bool> trueText) {
	std::array<int64_t, 5> res = { 0 };
	res[0] = protectedAndCorrupted.size() / 8;
	bool textTrue = true;
	bool flaggedAsCorrect = true;
	for (int byteBlock = 0; byteBlock < (protectedAndCorrupted).size(); byteBlock += 64) {
		std::vector<std::pair<int, int>> corruptedBits;
		bool rowParity[8] = { false };//True means even, false- uneven
		bool colParity[8] = { false };
		for (int byte = 0; ((byte < 64) && (byteBlock + byte < (protectedAndCorrupted).size())); byte += 8) {
			for (int bit = 0; bit < 8; bit++) {
				rowParity[byte / 8] ^= (protectedAndCorrupted)[byteBlock + byte + bit];
				colParity[bit] ^= (protectedAndCorrupted)[byteBlock + byte + bit];
				if ((bit != 0) && ((byte != 7 * 8)) && (byteBlock + byte < (protectedAndCorrupted).size() - 8)) {
					if ((protectedAndCorrupted)[byteBlock + byte + bit] != (trueText)[byteBlock / 8 * 7 + byte + bit]) {
						textTrue = false;
					}
				}
			}
		}
		for (int i = 0; i < 8; i++) {
			if (rowParity[i]) flaggedAsCorrect = false;
			if (colParity[i]) flaggedAsCorrect = false;
		}
		if (!(textTrue) && !(flaggedAsCorrect)) {
			break;
		}

	}
	if (textTrue && flaggedAsCorrect) res[1]++;
	else if (!textTrue && !flaggedAsCorrect) res[2]++;
	else if (!textTrue && flaggedAsCorrect) res[3]++;
	else if (textTrue && !flaggedAsCorrect) res[4]++;
	return res;
}

std::bitset<16> carryBit(std::bitset<16> checksum, int bit) {
	while (1) {
		if (bit == -1) {
			bit = 15;
		}
		if (checksum[bit] == 0) {
			checksum[bit] = 1;
			return checksum;
		}
		checksum[bit] = 0;
		bit--;
	}
}

std::deque<bool> checksumEncode16(std::deque<bool> textToProtect) {
	std::bitset<16> checksum = { 0 };
	for (int byteBlock = 0; byteBlock < textToProtect.size(); byteBlock += 16) {
		for (int bit = 0; (bit < 16) && (bit + byteBlock < textToProtect.size()); bit++) {
			if (checksum[bit] == textToProtect[byteBlock + bit]) {
				if (checksum[bit] == 1) {
					checksum = carryBit(checksum, bit);
				}
				checksum[bit] = 0;

			}
			else {
				checksum[bit] = 1;
			}
		}
	}
	for (int i = 0; i < 16; i++) {
		//textToProtect[textToProtect.size() + 15 - i] = (checksum[i]);
		textToProtect.push_back(checksum[i]);
	}
	return textToProtect;
}

std::array<int64_t, 5> checksumCheck16(std::deque<bool> protectedAndCorrupted, std::deque<bool> trueText) {
	std::array<int64_t, 5> res = { 0 };
	res[0] = protectedAndCorrupted.size() / 8;
	std::bitset<16> corruptedChecksum;
	for (int i = 0; i < 16; i++) {
		corruptedChecksum[i] = protectedAndCorrupted[protectedAndCorrupted.size() - 1];
		protectedAndCorrupted.pop_back();
	}
	std::deque<bool> newChecksum = checksumEncode16(protectedAndCorrupted);
	bool textTrue = true;
	bool flaggedAsCorrect = true;
	for (int i = 0; i < 16; i++) {
		if (corruptedChecksum[i]!=newChecksum[protectedAndCorrupted.size()+15-i]) flaggedAsCorrect = false;
	}
	for (int i = 0; i < protectedAndCorrupted.size() - 16; i++) {
		if (protectedAndCorrupted[i] != trueText[i]) textTrue = false;
	}

	if (textTrue && flaggedAsCorrect) res[1]++;
	else if (!textTrue && !flaggedAsCorrect) res[2]++;
	else if (!textTrue && flaggedAsCorrect) {
		res[3]++;
		/*std::cout << std::endl << "ProtectedAndCorrupted" << std::endl;
		for (int i = 0; i < protectedAndCorrupted.size(); i++) {
			std::cout << protectedAndCorrupted[i];
		}
		std::cout << std::endl;
		std::cout << std::endl<<"New Checksum"<<std::endl;
		for (int i = 0; i < newChecksum.size(); i++) {
			std::cout << newChecksum[i];
		}
		std::cout << std::endl << "Corrupted Checksum" << std::endl;
		for (int i = 0; i < corruptedChecksum.size(); i++) {
			std::cout << corruptedChecksum[i];
		}
		std::cout << std::endl;*/
	}
	else if (textTrue && !flaggedAsCorrect) res[4]++;
	return res;
}

std::deque<bool> doubleSendEncode(std::deque<bool> textToProtect) {
	int oldSize = textToProtect.size();
	textToProtect.resize(oldSize * 2);
	for (int i = 0; i < oldSize; i++) {
		textToProtect[oldSize + i] = textToProtect[i];
	}
	return textToProtect;
}

std::array<int64_t, 5> doubleSendCheck(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText) {
	std::array<int64_t, 5> res = { 0 };
	res[0] = protectedAndCorrupted.size() / 8;
	bool textTrue = true;
	bool flaggedAsCorrect = true;
	for (int i = 0; i < trueText.size(); i++) {
		if (protectedAndCorrupted[i] != trueText[i]) textTrue = false;
		if (protectedAndCorrupted[i] != protectedAndCorrupted[trueText.size() + i]) flaggedAsCorrect = false;
	}
	if (textTrue && flaggedAsCorrect) res[1]++;
	else if (!textTrue && !flaggedAsCorrect) res[2]++;
	else if (!textTrue && flaggedAsCorrect) res[3]++;
	else if (textTrue && !flaggedAsCorrect) res[4]++;
	return res;
}

std::deque<bool> tripleSendEncode(std::deque<bool> textToProtect) {
	int oldSize = textToProtect.size();
	textToProtect.resize(oldSize * 3);
	for (int i = 0; i < oldSize; i++) {
		textToProtect[oldSize + i] = textToProtect[i];
		textToProtect[oldSize * 2 + i] = textToProtect[i];
	}
	return textToProtect;
}

std::array<int64_t, 5> tripleSendCheck(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText) {
	std::array<int64_t, 5> res = { 0 };
	res[0] = protectedAndCorrupted.size() / 8;
	bool textTrue = true;
	bool flaggedAsCorrect = true;
	for (int i = 0; i < trueText.size(); i++) {
		if (protectedAndCorrupted[i] != trueText[i]) textTrue = false;
		if (protectedAndCorrupted[i] != protectedAndCorrupted[trueText.size() + i]) flaggedAsCorrect = false;
		if (protectedAndCorrupted[i] != protectedAndCorrupted[trueText.size() * 2 + i]) flaggedAsCorrect = false;
		if (protectedAndCorrupted[trueText.size() + i] != protectedAndCorrupted[trueText.size() * 2 + i]) flaggedAsCorrect = false;
	}
	if (textTrue && flaggedAsCorrect) res[1]++;
	else if (!textTrue && !flaggedAsCorrect) res[2]++;
	else if (!textTrue && flaggedAsCorrect) res[3]++;
	else if (textTrue && !flaggedAsCorrect) res[4]++;
	return res;
}

std::deque<bool> crcEncode32(std::deque<bool> textToProtect) {
	std::bitset <33> gp("111011011011100010000011001000001"); //inverted GP
	std::deque<bool> text(textToProtect);
	for (int i = 0; i < 32; i++) {
		text.push_back(0);
	}
	for (int i = 0; i < (text.size() - 32); i++) {
		if (text[i] != 0) {
			for (int j = 0; j < 33; j++) {
				text[j + i] = text[i + j] ^ gp[j];
			}
		}
	}

	for (int i = 0; i < textToProtect.size(); i++) {
		text[i] = textToProtect[i];
	}
	return text;
}

std::array<int64_t, 5> crcCheck32(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText) {
	std::array<int64_t, 5> res = { 0 };
	res[0] = protectedAndCorrupted.size() / 8;

	bool textTrue = true;
	bool flaggedAsCorrect = true;

	std::deque<bool> trueEncoded = crcEncode32(trueText);
	for (int i = 0; i < trueText.size(); i++) {
		if (protectedAndCorrupted[i] != trueText[i]) textTrue = false;
	}

	std::deque<bool> corruptedChecksum(protectedAndCorrupted);
	for (int i = 0; i < 32; i++) {
		corruptedChecksum.pop_back();
	}

	corruptedChecksum = crcEncode32(corruptedChecksum);
	for (int i = 0; i < 32; i++) {
		if (corruptedChecksum[trueText.size() + i] != protectedAndCorrupted[trueText.size() + i]) flaggedAsCorrect = false;
	}

	if (textTrue && flaggedAsCorrect) res[1]++;
	else if (!textTrue && !flaggedAsCorrect) res[2]++;
	else if (!textTrue && flaggedAsCorrect) res[3]++;
	else if (textTrue && !flaggedAsCorrect) res[4]++;

	return res;
}

std::array<int64_t, 5> checkEfficiency(functionPair funcs) {
	std::deque<bool>text = generateBinaryText();
	std::deque<bool>protectedText = (funcs.first)(text);
	std::deque<bool> corruptedText = simpleCorrupt(protectedText);
	std::array<int64_t, 5> result = (funcs.second)(corruptedText, text);

	return result;
}

//A function to test any special types of corruption
std::deque<bool> specialCorrupt(std::deque<bool> textToCorrupt) {
	textToCorrupt[7] = 0;
	return textToCorrupt;
}
