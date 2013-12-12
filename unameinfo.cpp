/**
 * unameinfo.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Dec 12 11:40:10 2013
 */
#include "unameinfo.hpp"
#include <sys/utsname.h>
#include <cstring>

using namespace SystemInfo;

std::array<std::string, UnameFields>
UnameInfo::getFieldNames()
{
   std::array<std::string, UnameFields> out =
      {
         "SysName",
         "NodeName",
         "OSRelease",
         "OSVersion",
         "MachineName"
      };
   return( out );
}

std::array<std::string, UnameFields>
UnameInfo::getFieldData()
{
   struct utsname un;
   std::memset( &un, 0x0, sizeof( struct utsname ) );
   errno = 0;
   if( uname( &un ) == EXIT_FAILURE )
   {
      perror("Failed to get uname data, all values zeroed");
   }
   std::array<std::string, UnameFields> out;
   out[0] = std::string( un.sysname    );
   out[1] = std::string( un.nodename   );
   out[2] = std::string( un.release    );
   out[3] = std::string( un.version    );
   out[4] = std::string( un.machine    );
   return( out );
}
