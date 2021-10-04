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
	bool stillTrue = true;
	bool parityKept = true;
	res[0] = MAX_TEXT_SIZE;
	for (int byte = 0; byte < MAX_TEXT_SIZE * 8; byte += 8) {
		bool sum = 0;
		for (int bit = 0; bit < 8; bit++) {
			sum ^= protectedAndCorrupted[byte + bit];
			if ((bit > 0)&& (protectedAndCorrupted[byte + bit] != trueText[byte + bit])) {
				stillTrue = false;
			}
		}
		if (sum) parityKept = false;
	}
	if (stillTrue && parityKept) {
		res[1]++;
	}
	else if (!(stillTrue) && !(parityKept)) {
		res[2]++;
	}
	else if (!(stillTrue) && parityKept) {
		res[3]++;
	}
	else if (stillTrue && !(parityKept)) {
		res[4]++;
	}
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
	bool parityKept = true;
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
			if (rowParity[i]) parityKept = false;
			if (colParity[i]) parityKept = false;
		}
		if (!(textTrue) && !(parityKept)) {
			break;
		}

	}
	if (textTrue && parityKept) res[1]++;
	else if (!(textTrue) && !(parityKept)) res[2]++;
	else if (!(textTrue) && parityKept) res[3]++;
	else if (textTrue && !(parityKept)) res[4]++;
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
		else {
			checksum[bit] = 0;
			bit--;
		}
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
		textToProtect[textToProtect.size() + 15 - i] = (checksum[i]);
	}
	return textToProtect;
}

std::array<int64_t, 5> checksumCheck16(std::deque<bool> protectedAndCorrupted, std::deque<bool> trueText) {
	std::array<int64_t, 5> res = { 0 };
	res[0] = protectedAndCorrupted.size() / 8;
	std::deque<bool> newChecksum = checksumEncode16(trueText);
	for (int i = 0; i < 16; i++) {
		protectedAndCorrupted.pop_back();
	}
	std::deque<bool> corruptedChecksum = checksumEncode16(protectedAndCorrupted);
	bool checksumCorrect = true;
	bool dataCorrect = true;
	for (int i = 0; i < 16; i++) {
		if ((newChecksum[newChecksum.size() - 1 - i]) != (corruptedChecksum[corruptedChecksum.size() - 1 - i])) checksumCorrect = false;
	}
	for (int i = 0; i < protectedAndCorrupted.size() - 16; i++) {
		if (protectedAndCorrupted[i] != trueText[i]) dataCorrect = false;
	}

	if (dataCorrect && checksumCorrect) res[1]++;
	else if (!(dataCorrect) && !(checksumCorrect)) res[2]++;
	else if (!(dataCorrect) && checksumCorrect) {
		res[3]++;

#ifdef _DEBUG

		std::cout << "										!!!!!!!!" << std::endl;
		std::cout << "true text" << std::endl;
		for (int i = 0; i < trueText.size(); i++) {
			std::cout << trueText[i];
		}
		std::cout << std::endl;
		std::cout << "corrupted" << std::endl;
		for (int i = 0; i < protectedAndCorrupted.size(); i++) {
			std::cout << protectedAndCorrupted[i];
		}
		std::cout << std::endl;
		std::cout << "true encoded" << std::endl;
		for (int i = 0; i < newChecksum.size(); i++) {
			std::cout << newChecksum[i];
		}
		std::cout << std::endl;
#endif
	}
	else if (dataCorrect && !(checksumCorrect)) res[4]++;
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
	bool textCorrect = true;
	bool protectedTextIsSame = true;
	for (int i = 0; i < trueText.size(); i++) {
		if (protectedAndCorrupted[i] != trueText[i]) textCorrect = false;
		if (protectedAndCorrupted[i] != protectedAndCorrupted[trueText.size() + i]) protectedTextIsSame = false;
	}
	if (textCorrect && protectedTextIsSame) {
		res[1]++;
	}
	else if (!(textCorrect) && !(protectedTextIsSame)) {
		res[2]++;
	}
	else if (!(textCorrect) && protectedTextIsSame) {
		res[3]++;
	}
	else if (textCorrect && !(protectedTextIsSame)) {
		res[4]++;
	}
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
	bool textCorrect = true;
	bool protectedTextIsSame = true;
	for (int i = 0; i < trueText.size(); i++) {
		if (protectedAndCorrupted[i] != trueText[i]) textCorrect = false;
		if (protectedAndCorrupted[i] != protectedAndCorrupted[trueText.size() + i]) protectedTextIsSame = false;
		if (protectedAndCorrupted[i] != protectedAndCorrupted[trueText.size() * 2 + i]) protectedTextIsSame = false;
		if (protectedAndCorrupted[trueText.size() + i] != protectedAndCorrupted[trueText.size() * 2 + i]) protectedTextIsSame = false;
	}
	if (textCorrect && protectedTextIsSame) {
		res[1]++;
	}
	else if (!(textCorrect) && !(protectedTextIsSame)) {
		res[2]++;
	}
	else if (!(textCorrect) && protectedTextIsSame) {
		res[3]++;
	}
	else if (textCorrect && !(protectedTextIsSame)) {
		res[4]++;
	}
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

	bool textCorrect = true;
	bool checkSequenceTrue = true;

	std::deque<bool> trueEncoded = crcEncode32(trueText);
	for (int i = 0; i < trueText.size(); i++) {
		if (protectedAndCorrupted[i] != trueText[i]) textCorrect = false;
	}

	std::deque<bool> corruptedChecksum(protectedAndCorrupted);
	for (int i = 0; i < 32; i++) {
		corruptedChecksum.pop_back();
	}

	corruptedChecksum = crcEncode32(corruptedChecksum);
	for (int i = 0; i < 32; i++) {
		if (corruptedChecksum[trueText.size() + i] != protectedAndCorrupted[trueText.size() + i]) checkSequenceTrue = false;
	}

	if (textCorrect && checkSequenceTrue) {
		res[1]++;
	}
	else if (!(textCorrect) && !(checkSequenceTrue)) {
		res[2]++;
	}
	else if (!(textCorrect) && checkSequenceTrue) {
		res[3]++;
#ifdef _DEBUG
		std::cout << "										!!!!!!!!" << std::endl;
		std::cout << "true text" << std::endl;
		for (int i = 0; i < trueText.size(); i++) {
			std::cout << trueText[i];
		}
		std::cout << std::endl;
		std::cout << "corrupted" << std::endl;
		for (int i = 0; i < protectedAndCorrupted.size(); i++) {
			std::cout << protectedAndCorrupted[i];
		}
		std::cout << std::endl;
		std::cout << "true encoded" << std::endl;
		for (int i = 0; i < trueEncoded.size(); i++) {
			std::cout << trueEncoded[i];
		}
		std::cout << std::endl;
#endif
	}
	else if (textCorrect && !(checkSequenceTrue)) {
		res[4]++;
	}

	return res;
}

std::array<int64_t, 5> checkEfficiency(functionPair funcs) {
	std::deque<bool>text = generateBinaryText();
	std::deque<bool>protectedText = (funcs.first)(text);
	std::deque<bool> corruptedText = simpleCorrupt(protectedText);
	std::array<int64_t, 5> result = (funcs.second)(corruptedText, text);

	/*for (int i = 0; i < text.size(); i++) {
		if (((i) % 8 == 0) && (i != 0)) std::cout << "." << std::endl;
		std::cout << text[i];
	}
	std::cout << "	" << "Simple" << std::endl;
	std::cout << std::endl;
	for (int i = 0; i < protectedText.size(); i++) {
		if (((i) % 8 == 0) && (i != 0)) std::cout << "." << std::endl;
		std::cout << protectedText[i];
	}
	std::cout << "	" << "Protected" << std::endl;
	std::cout << std::endl;
	for (int i = 0; i < corruptedText.size(); i++) {
		if (((i) % 8 == 0) && (i != 0)) std::cout << "." << std::endl;
		std::cout << corruptedText[i];
	}
	std::cout << "	" << "Corrupt" << std::endl;

	std::cout << "Bytes" << "    " << result[0] << std::endl;
	std::cout << "True Positive" << "    " << result[1] << std::endl;
	std::cout << "True Negative" << "    " << result[2] << std::endl;
	std::cout << "False Positive" << "    " << result[3] << std::endl;
	std::cout << "False Negative" << "    " << result[4] << std::endl;
	std::cout << std::endl;*/
	return result;
}

//A function to test any special types of corruption
std::deque<bool> specialCorrupt(std::deque<bool> textToCorrupt) {
	textToCorrupt[7] = 0;
	return textToCorrupt;
}
