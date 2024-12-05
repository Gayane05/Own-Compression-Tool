// Huffman EncoderDecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <queue>

#include "Element.h"
//namespace fs = std::filesystem;
//
//bool operator < (const Element& left, const Element& right)
//{
//   return left.weight < right.weight;
//}

void readContentFromFile(std::string& fileContent, std::string& fileName, std::string& /*filePathName*/)
{
   //if (!std::filesystem::exists(path))
   //{
   //   throw std::runtime_error("File does not exist: " + path.string());
   //}
//   fs::path filePath = filePathName;

   std::ifstream file(fileName);
   if (!file.is_open()) {
    //  throw InvalidJSONFileFormat("Cannot open file", __LINE__);
   }

   fileContent.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

   file.close();
}

void createFrequencyHeap(std::priority_queue<Element>& queue, const std::string& contentString)
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

void creatingTree(std::priority_queue<Element>& elements)
{

}

int main()
{
   std::cout << "Insert file path ";
   std::string filePath;
   //std::cin >> filePath;

   std::cout << "Insert file name ";
   std::string fileName;
   std::cin >> fileName;

   std::string fileContent;
   readContentFromFile(fileContent, fileName, filePath);

   std::priority_queue<Element> elementsHeap;
   createFrequencyHeap(elementsHeap, fileContent);

   creatingTree(elementsHeap);


   return 0;
}
