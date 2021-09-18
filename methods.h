#pragma once
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <random>
#include <array>
//Constants
const int MAX_TEXT_SIZE = 50;
const int CORRUPT_CHANCE = 25;
//Functions 
std::vector<bool> simpleCorrupt(std::vector<bool> text);
std::string generateText();
std::vector<bool> textToBinary(std::string text);
std::string binaryToText(std::vector<bool> binText);
double checkEfficiency();
//A list of protocols for comparison

 /*ASCII symbols are 7-bit long, and we have 1 spare bit. We can use it as a method to check the correctness!
 The 1st bit in each byte is now a parity bit; it is set so, that the sum of bits with it is even*/
std::vector<bool> parityBit(std::vector<bool> textToProtect);  
std::array<int,5> checkParityBit(std::vector<bool> protectedAndCorrupted, std::vector<bool> trueText);
/*Answer structure: 
Total byte number, True Positive(correct transmitted and correct accroding to parity bit),
True Negative(wrongly transmitted, mistake noticed by parity bit), False Positive(wrongly transmitted, mistake not noticed), 
False Negative(correctly transmitted, but incorrectly marked as wrong)
*/