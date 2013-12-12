#include <iostream>
#include <cstdlib>

#include "unameinfo.hpp"
#include "sysinfo.hpp"

int
main( int argc, char **argv )
{
   auto unamefn( SystemInfo::UnameInfo::getFieldNames() );
   for( std::string str : unamefn )
   {
      std::cout << str << "\n";
   }

   auto unamedata( SystemInfo::UnameInfo::getFieldData() );
   for( std::string str : unamedata )
   {
      std::cout << str << "\n";
   }
   
   auto sysfn( SystemInfo::SysInfo::getFieldNames() );
   for( std::string str : sysfn )
   {
      std::cout << str << "\n";
   }

   auto sysdata( SystemInfo::SysInfo::getFieldData() );
   for( std::string str : sysdata )
   {
      std::cout << str << "\n";
   }
   return( EXIT_FAILURE );
}
