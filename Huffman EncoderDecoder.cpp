// Huffman EncoderDecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <map>
#include <queue>
#include <bitset>

#include "Element.h"

//namespace fs = std::filesystem;
//
//bool operator < (const Element& left, const Element& right)
//{
//   return left.weight < right.weight;
//}


int binaryToDecimal(const std::string& in)
{
   int result = 0;
   for (int i = 0; i < in.size(); i++)
      result = result * 2 + in[i] - '0';
   return result;
}

std::string toBinary(int n)
{
   std::string r;
   while (n != 0) { r = (n % 2 == 0 ? "0" : "1") + r; n /= 2; }
   return r;
}

void readContentFromFile(std::string& fileContent, const std::string& fileName, std::string& /*filePathName*/)
{
   //if (!std::filesystem::exists(path))
   //{
   //   throw std::runtime_error("File does not exist: " + path.string());
   //}
   //fs::path filePath = filePathName;

   std::ifstream file(fileName);
   if (!file.is_open()) {
    //  throw InvalidJSONFileFormat("Cannot open file", __LINE__);
   }

   fileContent.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

   file.close();
}

// Store different characters frequencies.
void createFrequencyHeap(std::priority_queue<Element, std::vector<Element>, std::greater<Element>>& queue,
   const std::string& contentString)
{
   std::unordered_map<char, long long> freqTable;
   for (const auto symbol : contentString)
   {
      freqTable[symbol]++;
   }

   for (auto el : freqTable)
   {
      queue.push(Element{el.second ,el.first});
   }
}

// Construct Priority Queue and Huffman Tree
Element* creatingBinaryTree(std::priority_queue<Element, std::vector<Element>, std::greater<Element>>& elements)
{
   // Taking the smallest 2 elements to join them in one Element.

   while (elements.size() != 1)
   {
      Element min_1 = elements.top(); // taking smallest
      elements.pop();
      Element min_2 = elements.top(); // taking second smallest
      elements.pop();

      Element newEl{ min_1.weight + min_2.weight };
      Element* leftChild = new Element(min_1);
     leftChild->path += '0';

      Element* rightChild = new Element(min_2);
     rightChild->path += '1';

      newEl.leftChild = leftChild;
      newEl.rightChild = rightChild;

      elements.push(newEl);
   }

   Element* root = new Element(elements.top());

   return root;
}

std::unordered_map<char, std::string> prefixCodeTable;

// Create Huffman Encode Table
void createPrefixCodeTable(Element* root, std::string& path)
{
   if (!root)
   {
      return;
   }

   path += root->path;

   if (!root->leftChild && !root->rightChild)
   {
      std::cout << root->symbol << " " << path << "\n";
      prefixCodeTable[root->symbol] = path;
   }

   createPrefixCodeTable(root->leftChild, path);
   createPrefixCodeTable(root->rightChild, path);
   if (path.size())
   {
      path.pop_back();
   }
}

void saveInFile(std::string& initialText)
{
   std::cout << "Insert output file name ";
   std::string fileName;
   std::cin >> fileName;

   std::ofstream outputFile(fileName, std::ofstream::out);

   for (const auto& it : prefixCodeTable)
   {
      outputFile << it.first << "-" << it.second << "\\";
   }

   std::string encodedText;
   for (size_t i = 0; i < initialText.size(); ++i)
   {
      encodedText += prefixCodeTable[initialText[i]];  // Taking prefix code from table for concrete character.
   }
   std::cout << "Encoded text " << encodedText  << " Total Bytes : " << encodedText.size() << '\n';

   std::vector<uint8_t> encodedPackedText;
   for (size_t i = 0; i < encodedText.size(); i +=8)
   {
      int number = binaryToDecimal(encodedText.substr(i, 8));  // Number as result of packed bits.
      encodedPackedText.push_back(number);

      // Binary to decimal
      // unsigned long decimal = std::bitset<8>(binary).to_ulong();
      // std::cout << decimal << "\n";
   }

   for (size_t i = 0; i < encodedPackedText.size(); ++i)
   {
      std::cout << "Encoded packed text " << encodedPackedText[i] << " " << '\n';
      outputFile << encodedPackedText[i];
   }
   outputFile.close();

   std::cout << "Total Bytes : " << encodedPackedText.size() * sizeof(encodedPackedText[0]);
}

void encoding()
{
   std::cout << "Insert file path ";
   std::string filePath;
   //std::cin >> filePath;

   std::cout << "Insert file name ";
   std::string fileName;
   std::cin >> fileName;

   std::string initialText;
   readContentFromFile(initialText, fileName, filePath);
   std::cout << "Initial text " << initialText << " Total Bytes : " << initialText.size() * sizeof(initialText[0]) << '\n';

   std::priority_queue<Element, std::vector<Element>, std::greater<Element>> elementsHeap;
   createFrequencyHeap(elementsHeap, initialText);

   Element* rootNode = creatingBinaryTree(elementsHeap);

   std::string str;
   createPrefixCodeTable(rootNode, str);

   saveInFile(initialText);
}

// Recreating Prefix code table for decoding from encoded content's header.
// [in] encodedContent. Encoded content read from file.
// [in] headerEndPos. Position where header ends.
// [out] prefixCodeTable. Reference of prefixCodeTable. Which key is prefixcode and value is actual character.
void recreatePrefixCodeTable(const std::string& encodedContent, size_t headerEndsPos, std::map<std::string, char, std::greater<>>& prefixCodeTable)
{
   int i = 0;
   while (i < headerEndsPos)
   {
      if (encodedContent[i] == '\\')
      {
         ++i;
      }

      char value = encodedContent[i];
      std::string key;

      ++i; // Skipping deliminator '-'.
      while (++i < headerEndsPos && encodedContent[i] != '\\') // Take encoded prefix.
      {
         key += encodedContent[i];
      }

      prefixCodeTable[key] = value;
   }
}


std::string findTheValueSymbols(std::map<std::string, char, std::greater<>>& prefixCodeTable, std::string & byteKey, int max_length)
{
   // Need to refactor.
   std::string valueSymbol;

   std::string key = byteKey.substr(0, max_length);

   while (prefixCodeTable.find(key) == prefixCodeTable.end())
   {
      key = byteKey.substr(0, max_length - 1);
   }
   valueSymbol += prefixCodeTable[key];

   return valueSymbol;
}

std::string decodeEncodedMessage(const std::string& encodedMessage, std::map<std::string, char, std::greater<>>& prefixCodeTable)
{
   std::string initialText;
   std::string notPackedEncodedText;

   for (size_t i = 0; i < encodedMessage.size(); ++i)
   {
      std::string binary = std::bitset<8>(encodedMessage[i]).to_string(); //to binary
      //decimalToBinary();
      notPackedEncodedText += binary;
   }

   size_t max_length = prefixCodeTable.begin()->first.size();

   initialText = findTheValueSymbols(prefixCodeTable, notPackedEncodedText, static_cast<int>(max_length));


   return initialText;
}

void decoding()
{
   std::cout << "Insert file path ";
   std::string filePath;
   ////std::cin >> filePath;

   //std::cout << "Insert file name of encoded text";
   //std::string fileName;
   //std::cin >> fileName;

   std::string encodedContent;
   readContentFromFile(encodedContent, std::string{"out.txt"}, filePath);

   const auto headerEndsPos = encodedContent.find_last_of('\\');
   std::string encodedText = encodedContent.substr(headerEndsPos + 1); // Taking only encoded text.

   std::map<std::string, char, std::greater<>> prefixCodeTable;
   recreatePrefixCodeTable(encodedContent, headerEndsPos, prefixCodeTable);

   decodeEncodedMessage(encodedText, prefixCodeTable);
}


int main()
{
   encoding();

   decoding();

   return 0;
}
