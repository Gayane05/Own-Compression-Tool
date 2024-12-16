// Huffman EncoderDecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
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

      //std::bitset<3> b(path);
      //  encodedText += b.to_string();
   }

   std::cout << "Encoded text " << encodedText  << " Total Bytes : " << encodedText.size() << '\n';

   std::string encodedPackedText;

   for (size_t i = 0; i < encodedText.size(); i +=8)
   {
      int number = binaryToDecimal(encodedText.substr(i, 8));  // Number as result of packed bits.
      encodedPackedText += std::to_string(number);
   }

   std::cout << "Encoded packed text " << encodedPackedText << " Total Bytes : " << encodedPackedText.size() << '\n';

   outputFile.close();
}

void encoding()
{
   std::cout << "Insert file path ";
   std::string filePath;
   //std::cin >> filePath;

   std::cout << "Insert file name ";
   std::string fileName;
   std::cin >> fileName;

   std::string intialText;
   readContentFromFile(intialText, fileName, filePath);

   std::cout << "Initial text " << intialText << " Total Bytes : " << intialText.size() << '\n';

   std::priority_queue<Element, std::vector<Element>, std::greater<Element>> elementsHeap;
   createFrequencyHeap(elementsHeap, intialText);

   Element* rootNode = creatingBinaryTree(elementsHeap);

   std::string str;
   createPrefixCodeTable(rootNode, str);


   saveInFile(intialText);
}

void decoding()
{
   std::cout << "Insert file path ";
   std::string filePath;
   //std::cin >> filePath;

   std::cout << "Insert file name ";
   std::string fileName;
   std::cin >> fileName;

   //std::string encodedText;

   //readContentFromFile(encodedText, fileName, filePath);


   //const auto headerEndsPos = std::find(encodedText.rbegin(), encodedText.rend(), '\\');

}


int main()
{
   encoding();

 //  decoding();


   return 0;
}
