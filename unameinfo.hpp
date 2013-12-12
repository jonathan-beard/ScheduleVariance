/**
 * unameinfo.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Dec 12 11:40:10 20UnameFields
 */
#ifndef _UNAMEINFO_HPP_
#define _UNAMEINFO_HPP_  1
#include <array>
#include <string>

#define UnameFields 5

namespace SystemInfo{

class UnameInfo{
public:
   UnameInfo() = delete;
   ~UnameInfo() = delete;

   static std::array<std::string,UnameFields> getFieldNames();
   static std::array<std::string,UnameFields> getFieldData();
};

} /* end SystemInfo namespace */

#endif /* END _UNAMEINFO_HPP_ */
