/**
 * sysinfo.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Dec 12 11:40:14 2013
 */
#ifndef _SYSINFO_HPP_
#define _SYSINFO_HPP_  1
#include <string>
#include <array>

/**
 * assume linux kernel is >= 2.3.48 and has exactly 12 fields
 */
#define SystemInfoFields 12

namespace SystemInfo{

class SysInfo {
public:
   SysInfo()   = delete;
   ~SysInfo()  = delete;

   static std::array< std::string, SystemInfoFields> getFieldNames();

   static std::array< std::string, SystemInfoFields> getFieldData();
};

} /* end namespace SystemInfo */

#endif /* END _SYSINFO_HPP_ */
