#pragma once

#include <string>


struct Element
{

   Element(long long weight, char symbol) : weight(weight), symbol(symbol) {};

   Element(long long weight) : weight(weight) { symbol = '.'; };

   long long weight;
   char symbol;
   std::string rightWay;
   std::string leftWay;

   Element* leftChild;
   Element* rightChild;


   bool operator < ( const Element& right) const
   {
      return this->weight < right.weight;
   }
};