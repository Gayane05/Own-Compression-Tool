// Huffman EncoderDecoder.cpp

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <map>
#include <queue>
#include <bitset>

#include "Element.h"
#include "UnableToProcessFile.h"

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

void readContentFromFile(std::string& fileContent, const std::string& fileName, std::string filePathName = {""})
{
   //if (!std::filesystem::exists(path))
   //{
   //   throw std::runtime_error("File does not exist: " + path.string());
   //}
   //fs::path filePath = filePathName;

   std::ifstream file(fileName);

   if (!file.is_open())
   {
      throw UnableToProcessFile("Can not open mentioned file.", __LINE__);
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
      Element min_1 = elements.top(); // taking smallest.
      elements.pop();
      Element min_2 = elements.top(); // taking second smallest.
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

std::string saveInFile(std::string& initialText)
{
   std::cout << "Insert output file name ";
   std::string outputFileName;
   std::cin >> outputFileName;

   std::ofstream outputFile(outputFileName, std::ofstream::out);

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

   std::string fillFalsePrefix;
   size_t fillFalsePrefixCount = 8 - (encodedText.size() % 8);
   outputFile << fillFalsePrefixCount;

   while (fillFalsePrefixCount--)
   {
      fillFalsePrefix += "0";
   }

   encodedText = fillFalsePrefix + encodedText;

   std::vector<uint8_t> encodedPackedText;
   for (size_t i = 0; i < encodedText.size(); i +=8)
   {
      // Binary to decimal
      std::string sub = encodedText.substr(i, 8);
      unsigned int number = std::bitset<8>(sub).to_ulong();
      // int number = binaryToDecimal(encodedText.substr(i, 8));  // Number as result of packed bits.

      encodedPackedText.push_back(number);
   }

   for (size_t i = 0; i < encodedPackedText.size(); ++i)
   {
      std::cout << "Encoded packed text " << encodedPackedText[i] << " " << '\n';
      outputFile << encodedPackedText[i];
   }
   outputFile.close();

   std::cout << "Total Bytes : " << encodedPackedText.size() * sizeof(encodedPackedText[0]) << "\n";

   return outputFileName;
}

std::string encoding()
{
   std::cout << "Insert file path ";
   std::string filePath;
   //std::cin >> filePath;

   std::cout << "Insert file name ";
   std::string fileName;
   std::cin >> fileName;

   std::string initialText;
   try
   {
      readContentFromFile(initialText, fileName, filePath);
   }
   catch (UnableToProcessFile& exp)
   {
      std::cout << exp.what();
      return "";
   }

   std::cout << "Initial text " << initialText << " Total Bytes : " << initialText.size() * sizeof(initialText[0]) << '\n';

   std::priority_queue<Element, std::vector<Element>, std::greater<Element>> elementsHeap;
   createFrequencyHeap(elementsHeap, initialText);

   Element* rootNode = creatingBinaryTree(elementsHeap);

   std::string str;
   createPrefixCodeTable(rootNode, str);

   return saveInFile(initialText);
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

std::string findTheValueSymbols(std::map<std::string, char, std::greater<>>& prefixCodeTable, std::string & byteKey, int max_length, int falseElCount)
{
   // Need to refactor.
   std::string initialText;

   int charCount = max_length;

   for (size_t i = falseElCount; i < byteKey.size();)
   {
      std::string key = byteKey.substr(i, charCount);

      while (charCount > 0 && prefixCodeTable.find(key) == prefixCodeTable.end())
      {
         key = byteKey.substr(i, --charCount);
      }
      initialText += prefixCodeTable[key];
      i += charCount;
      charCount = max_length;
   }

   return initialText;
}

std::string decodeEncodedMessage(const std::string& encodedMessage, std::map<std::string, char, std::greater<>>& prefixCodeTable, char falseElCount)
{
   std::string initialText;
   std::string binaryString;

   for (uint8_t byte : encodedMessage)
   {
      binaryString += std::bitset<8>(byte).to_string();
   }


   //for (size_t i = 0; i < encodedMessage.size(); ++i)
   //{
   //   std::string binary = std::bitset<8>(encodedMessage[i]).to_string(); //to binary

   //   std::string binary = toBinary(encodedMessage[i]);
   //   binaryString += binary;
   //}

   size_t max_length = prefixCodeTable.begin()->first.size();
   return findTheValueSymbols(prefixCodeTable, binaryString, static_cast<int>(max_length), falseElCount - '0');
}

void decoding(const std::string& /*outputFileName*/)
{
   std::cout << "Insert out file path ";
   std::string filePath;
   ////std::cin >> filePath;

   std::cout << "Insert file name of encoded text";
   std::string fileName;
   std::cin >> fileName;

   std::cout << "Insert output file name";
   std::string outputFileName;
   std::cin >> outputFileName;

   std::string encodedContent;

   try
   {
      readContentFromFile(encodedContent, std::string{"out.txt"}/*, filePath*/);
   }
   catch (UnableToProcessFile& exp)
   {
      throw;
   }

   const auto headerEndsPos = encodedContent.find_last_of('\\');
   std::string encodedText = encodedContent.substr(headerEndsPos + 2); // Taking only encoded text and ignoring false elements count.

   std::map<std::string, char, std::greater<>> prefixCodeTable;
   recreatePrefixCodeTable(encodedContent, headerEndsPos, prefixCodeTable);

   std::string initialText = decodeEncodedMessage(encodedText, prefixCodeTable, encodedContent[headerEndsPos + 1]);

   std::cout << "Initial Message was: " << initialText;

   std::ofstream file(outputFileName);
   if (!file.is_open()) 
   {
      throw UnableToProcessFile("Can not open mentioned file.", __LINE__);
   }

   file << initialText;
}


int main()
{
   std::string outputFileName = encoding();

   if (outputFileName.empty())
   {
      std::cout << "File hasn't been processed!";
   }
   else
   {
      try
      {
         decoding(outputFileName);
      }
      catch (UnableToProcessFile& exp)
      {
         std::cout << exp.what();
      }
   }

   return 0;
}