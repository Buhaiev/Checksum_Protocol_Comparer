#pragma once
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <random>
#include <array>
#include <functional>
#include <time.h>
#include <chrono>
//Constants, typedefs, etc.
//A pair of functions: one encodes, the other one check efficiency
const int MAX_TEXT_SIZE =1500;
const int NUMBER_OF_DATAGRAMS = 1'000;
typedef std::pair<std::function< std::vector<bool>(std::vector<bool>)>, std::function< std::array<long long int, 5>(std::vector<bool>, std::vector<bool>)>> functionPair;
const int CORRUPT_CHANCE = 1'500'000; //1 error in CORRUPT_CHANCE bits
//Functions 
std::vector<bool> simpleCorrupt(std::vector<bool>text);
std::vector<bool> specialCorrupt(std::vector<bool> textToCorrupt);

//std::string generateText(int size, bool random);
//std::vector<bool> textToBinary(std::string text);
//std::string binaryToText(std::vector<bool> binText);
std::vector<bool> generateBinaryText();
std::array<long long int, 5> checkEfficiency(functionPair funcs);

//A list of protocols for comparison

 /*Answer structure:
 Total byte number, True Positive(correct transmitted and correct accroding to parity bit),
 True Negative(wrongly transmitted, mistake noticed by parity bit), False Positive(wrongly transmitted, mistake not noticed),
 False Negative(correctly transmitted, but incorrectly marked as wrong)
 That refers to whole datagrams.
 */
std::vector<bool> parityBitEncode(std::vector<bool> textToProtect);
std::array<long long int,5> parityBitCheck(std::vector<bool> protectedAndCorrupted, std::vector<bool> trueText);

std::vector<bool>  doubleParityBitEncode(std::vector<bool> textToProtect);
std::array<long long int, 5> doubleParityBitCheck(std::vector<bool>  protectedAndCorrupted, std::vector<bool> trueText);

std::vector<bool>  checksumEncode16(std::vector<bool> textToProtect);
std::array<long long int, 5> checksumCheck16(std::vector<bool>  protectedAndCorrupted, std::vector<bool> trueText);

std::vector<bool> doubleSendEncode(std::vector<bool> textToProtect);
std::array<long long int, 5> doubleSendCheck(std::vector<bool>  protectedAndCorrupted, std::vector<bool> trueText);

std::vector<bool> tripleSendEncode(std::vector<bool> textToProtect);
std::array<long long int, 5> tripleSendCheck(std::vector<bool>  protectedAndCorrupted, std::vector<bool> trueText);

std::vector<bool> crcEncode32(std::vector<bool> textToProtect);
std::array<long long int, 5> crcCheck32(std::vector<bool>  protectedAndCorrupted, std::vector<bool> trueText);