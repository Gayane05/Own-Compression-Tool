#pragma once


#include <iostream>
#include <string>

class UnableToProcessFile : public std::exception
{

public:
   UnableToProcessFile(std::string reason, int line) : line{line}
   {
      exceptionMessage = reason + " Line: " + std::to_string(line) + "\n";
   }

   virtual const char* what() const noexcept
   {
      return exceptionMessage.c_str();
   }

private:
   std::string exceptionMessage;
   int line;
};