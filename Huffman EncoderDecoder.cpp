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

// Converting binary to decimal.
// [in] in. Input string represantation of binary number.
// [out] decimalNumber. Number in decimal.
int binaryToDecimal(const std::string& in)
{
   int decimalNumber = 0;
   for (int i = 0; i < in.size(); i++)
      decimalNumber = decimalNumber * 2 + in[i] - '0';
   return decimalNumber;
}

// Converting decimal to binary.
// [in] in. Input decimal number.
// [out] binaryNumber. Binary version represented with std::string.
std::string decimalToBinary(int n)
{
   std::string binaryNumber;
   while (n != 0)
   {
      binaryNumber = (n % 2 == 0 ? "0" : "1") + binaryNumber;
      n /= 2;
   }
   return binaryNumber;
}

// Reading file content into std::string.
// [in/out] fileContent. Variable where content from file will be written.
// [in] fileName. Name of file which content should be read.
// [in] filePath. Path of the file.
// [throws] Throwns an exception if the file couldn't be opened.
void readContentFromFile(std::string& fileContent, const std::string& fileName, std::string filePath = {""})
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

// Store different characters frequencies in min-heap.
// [in/out] minHeap. Heap which contains Elements with their frequencies.
// [in] contentString. String where content from file is.
void createFrequencyHeap(std::priority_queue<Element, std::vector<Element>, std::greater<Element>>& minHeap,
   const std::string& contentString)
{
   std::unordered_map<char, long long> freqTable;
   for (const auto symbol : contentString)
   {
      freqTable[symbol]++;
   }

   for (auto el : freqTable)
   {
      minHeap.push(Element{el.second ,el.first});
   }
}

// Constructs Huffman Tree from elementsMinHeap (priority queue) and returns root of it.
// [in] elementsMinHeap. MinHeap which contains Elements with their frequencies.
// [out] root. Pointer to the root of Huffman Tree.
Element* creatingBinaryTree(std::priority_queue<Element, std::vector<Element>, std::greater<Element>>& elementsMinHeap)
{
   // Taking the smallest 2 elements to join them in one Element.

   while (elementsMinHeap.size() != 1)
   {
      Element min_1 = elementsMinHeap.top(); // taking smallest.
      elementsMinHeap.pop();
      Element min_2 = elementsMinHeap.top(); // taking second smallest.
      elementsMinHeap.pop();

      Element newEl{ min_1.weight + min_2.weight };
      Element* leftChild = new Element(min_1);
      leftChild->path += '0';

      Element* rightChild = new Element(min_2);
      rightChild->path += '1';

      newEl.leftChild = leftChild;
      newEl.rightChild = rightChild;

      elementsMinHeap.push(newEl);
   }

   Element* root = new Element(elementsMinHeap.top());

   return root;
}

std::unordered_map<char, std::string> prefixCodeTable;

// Create Huffman Encode/Prefix code Table and save in prefixCodeTable.
// [in] root. Pointer to Huffman Tree root element.
// [in] path. Prefix code of parent nodes + node's itself.
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

// Encode initial text by seting prefix code for each character from prefixCodeTable.
// [in] initialText. Initial text/content which was read from file and will be encoded.
std::string encodeTheTextTo01s(const std::string& initialText)
{
   std::string encodedText;
   for (size_t i = 0; i < initialText.size(); ++i)
   {
      encodedText += prefixCodeTable[initialText[i]];  // Taking prefix code from table for concrete character.
   }
   std::cout << "Encoded text " << encodedText << " Total Bytes : " << encodedText.size() << '\n';

   return encodedText;
}

// Packing/compressing encoded text into 1 byte char every 8 digit from encodedText.
void packEncodedText(const std::string& encodedText, std::vector<uint8_t>& encodedCompressedText)
{
   // Taking every 8 0-1s from binary represantation string and convert them to number.
   // Saving them as a member in vector of uint8 for saving their char represantation in ascii code (which will be 1 byte each, for compressing),
   // instead of saving them as decimal numbers in uint, which will make 4 bytes each of them.
   for (size_t i = 0; i < encodedText.size(); i += 8)
   {
      // Binary to decimal.
      std::string sub = encodedText.substr(i, 8);
      unsigned int number = std::bitset<8>(sub).to_ulong();
      // int number = binaryToDecimal(encodedText.substr(i, 8));  // Number as result of packed bits.

      encodedCompressedText.push_back(number);
   }
}

// Saves in output file prefix codes(header), fake digits count and encoded+compressed text.
// [in/out] initialText. Initial text/content which was read from file and will be encoded.
// [out] outputFileName. Returns output file's name.
std::string saveInFile(std::string& initialText)
{
   std::cout << "Insert output file name ";
   std::string outputFileName;
   std::cin >> outputFileName;

   std::ofstream outputFile(outputFileName, std::ofstream::out);

   // Write each character and its prefix code to output file.
   for (const auto& it : prefixCodeTable)
   {
      outputFile << it.first << "-" << it.second << "\\";
   }

   std::string encodedText = encodeTheTextTo01s(initialText);

   // For cases when encoded texts digits count isn't eightfold, we should add fake digits from its beginning and do the packing/compressing with it,
   // but ignore those digits during decoding and matching to prefix codes, so we should provide count of fake/false digits.
   std::string fillFalsePrefix;
   size_t fillFalsePrefixCount = 8 - (encodedText.size() % 8);
   outputFile << fillFalsePrefixCount;

   while (fillFalsePrefixCount--)
   {
      fillFalsePrefix += "0";
   }

   // Adding fake digits to encoded text.
   encodedText = fillFalsePrefix + encodedText;

   std::vector<uint8_t> encodedCompressedText;
   packEncodedText(encodedText, encodedCompressedText);


   // Saving in the output file compressed version of already encoded text.
   for (size_t i = 0; i < encodedCompressedText.size(); ++i)
   {
      std::cout << "Encoded packed text " << encodedCompressedText[i] << " " << '\n';
      outputFile << encodedCompressedText[i];
   }
   outputFile.close();

   std::cout << "Total Bytes : " << encodedCompressedText.size() * sizeof(encodedCompressedText[0]) << "\n";

   return outputFileName;
}

// Start encoding, save prefix codes and compressed text in output file.
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

// Recreate initial text by finding and assembling prefix codes of characters.
// [in] prefixCodeTable. PrefixCodeTable. Where key is the prefix code and value is the character.
// [in] binaryText. Binary text which should be decoded.
// [in] max_length. Length of the longest prefix code.
// [in] beginOfBinaryText. Begin index of binary text (false/fake added 01s are already being ignored).
// [out] initialText. Initial decoded text is returned.
std::string assemblePrefixes(std::map<std::string, char, std::greater<>>& prefixCodeTable, std::string & binaryText, int max_length, int beginOfBinaryText)
{
   // Need to refactor.
   std::string initialText;
   int charCount = max_length;

   for (size_t i = beginOfBinaryText; i < binaryText.size();)
   {
      // At first try to find key(prefix code) in prefixCodeTable with maximal length. If not found, then decrease length of the key taken from binary text.
      std::string key = binaryText.substr(i, charCount);

      while (charCount > 0 && prefixCodeTable.find(key) == prefixCodeTable.end())
      {
         key = binaryText.substr(i, --charCount);
      }
      initialText += prefixCodeTable[key];
      i += charCount;
      charCount = max_length;
   }

   return initialText;
}

// Converting each character of packed message to its binary represantation and getting the whole binary string.
// [in] packedMessage. Packed message from file.
// [out] binaryString. Returns encoded binary string.
std::string unpackToBinary(const std::string& packedMessage)
{
   std::string binaryString;

   for (uint8_t byte : packedMessage)
   {
      binaryString += std::bitset<8>(byte).to_string();

     // binaryString += toBinary(packedMessage[i]); // Alternate way.
   }

   return binaryString;
}

// Reads encoded text from the file, decodes it and writes the result to new outpuit file.
// [throws] Throws an exception if output file couldn't be opened.
void decoding()
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
   std::string packedMessage = encodedContent.substr(headerEndsPos + 2); // Taking only encoded text and ignoring false elements count.

   std::map<std::string, char, std::greater<>> prefixCodeTable;
   recreatePrefixCodeTable(encodedContent, headerEndsPos, prefixCodeTable);

   // Text read from the file is encoded and then packed. So, for first step need to unpack it.
   std::string binaryText = unpackToBinary(packedMessage);

   // Getting length of longest prefix code from prefixCodeTable.
   size_t max_length = prefixCodeTable.begin()->first.size();

   // Reconstructing initial text by ignoring false elements.
   std::string initialText = assemblePrefixes(prefixCodeTable, binaryText, static_cast<int>(max_length), encodedContent[headerEndsPos + 1] - '0');

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
         decoding();
      }
      catch (UnableToProcessFile& exp)
      {
         std::cout << exp.what();
      }
   }

   return 0;
}