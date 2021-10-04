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
#include <deque>

constexpr int MAX_TEXT_SIZE = 1500;
constexpr  int NUMBER_OF_DATAGRAMS = 1'000;
constexpr  int CORRUPT_CHANCE = 1'500'000; //1 error in CORRUPT_CHANCE bits
typedef std::pair<std::function< std::deque<bool>(std::deque<bool>)>, std::function< std::array<int64_t, 5>(std::deque<bool>, std::deque<bool>)>> functionPair;

//Functions 
std::deque<bool> simpleCorrupt(std::deque<bool> text);
std::deque<bool> specialCorrupt(std::deque<bool> textToCorrupt);

std::deque<bool> generateBinaryText();
std::array<int64_t, 5> checkEfficiency(functionPair funcs);

//A list of protocols for comparison

 /*Answer structure:
 Total byte number, True Positive(correct transmitted and correct accroding to parity bit),
 True Negative(wrongly transmitted, mistake noticed by parity bit), False Positive(wrongly transmitted, mistake not noticed),
 False Negative(correctly transmitted, but incorrectly marked as wrong)
 That refers to whole datagrams.
 */
std::deque<bool> parityBitEncode(std::deque<bool> textToProtect);
std::array<int64_t, 5> parityBitCheck(std::deque<bool> protectedAndCorrupted, std::deque<bool> trueText);

std::deque<bool>  doubleParityBitEncode(std::deque<bool> textToProtect);
std::array<int64_t, 5> doubleParityBitCheck(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText);

std::deque<bool>  checksumEncode16(std::deque<bool> textToProtect);
std::array<int64_t, 5> checksumCheck16(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText);

std::deque<bool> doubleSendEncode(std::deque<bool> textToProtect);
std::array<int64_t, 5> doubleSendCheck(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText);

std::deque<bool> tripleSendEncode(std::deque<bool> textToProtect);
std::array<int64_t, 5> tripleSendCheck(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText);

std::deque<bool> crcEncode32(std::deque<bool> textToProtect);
std::array<int64_t, 5> crcCheck32(std::deque<bool>  protectedAndCorrupted, std::deque<bool> trueText);