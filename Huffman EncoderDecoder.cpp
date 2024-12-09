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

void createFrequencyHeap(std::priority_queue<Element, std::vector<Element>, std::greater<Element>>& queue, const std::string& contentString)
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

void createPrefixCodeTable(Element* root, char symbol, std::string& path)
{
   if (!root)
   {
      return;
   }

   path += root->path;

   if (!root->leftChild && !root->rightChild)
   {
      std::cout << root->symbol << " " << path << "\n";
      prefixCodeTable[symbol] = path;
   }

   createPrefixCodeTable(root->leftChild, symbol, path);
   createPrefixCodeTable(root->rightChild, symbol, path);
   if (path.size())
   {
      path.pop_back();
   }
}

void printTree(Element* root)
{
   if (root)
   {
      printTree(root->leftChild);
      //    path += root->path;
      std::cout << root->symbol << "  " << root->path << "\n";

      printTree(root->rightChild);
   }
}

void outputInFile()
{
   std::cout << "Insert output file name ";
   std::string fileName;
   std::cin >> fileName;

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

   std::priority_queue<Element, std::vector<Element>, std::greater<Element>> elementsHeap;
   createFrequencyHeap(elementsHeap, fileContent);

   Element * rootNode = creatingBinaryTree(elementsHeap);

   std::string str;
   createPrefixCodeTable(rootNode, 'x', str);

   printTree(rootNode);

   return 0;
}
