/**
 * sysinfo.cpp -
 * @author: Jonathan Beard
 * @version: Thu Dec 12 11:40:14 2013
 */
#include "sysinfo.hpp"
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <sys/sysinfo.h>
using namespace SystemInfo;

std::array<std::string, SystemInfoFields>
SysInfo::getFieldNames()
{
   std::array<std::string, SystemInfoFields> out =
      {
         "UpTime",
         "Loads",
         "TotalMainMemory",
         "FreeRam",
         "SharedRam",
         "BufferRam",
         "TotalSwap",
         "FreeSwap",
         "CurrentProcessesRunning",
         "TotalHighMemory",
         "FreeHighMemory",
         "MemoryUnit"
      };

   return( out );
}


std::array<std::string, SystemInfoFields>
SysInfo::getFieldData()
{
   struct sysinfo info;
   std::memset( &info, 0x0, sizeof( struct sysinfo ) );
   errno = 0;
   if( sysinfo( &info ) == EXIT_FAILURE )
   {
      perror( "Sysinfo call failed, returning all zeros" );
   }
   std::array< std::string, SystemInfoFields > out;
   out[0] = std::to_string( info.uptime );
   /* three loads, seperate by '-' */
   std::stringstream uptimes;
   uptimes << info.loads[0] << "-" << info.loads[1] << "-" << info.loads[2];
   out[1]  = uptimes.str();
   out[2]  = std::to_string( info.totalram );
   out[3]  = std::to_string( info.freeram ) ;
   out[4]  = std::to_string( info.sharedram );
   out[5]  = std::to_string( info.bufferram );
   out[6]  = std::to_string( info.totalswap );
   out[7]  = std::to_string( info.freeswap );
   out[8]  = std::to_string( info.procs );
   out[9]  = std::to_string( info.totalhigh );
   out[10] = std::to_string( info.freehigh );
   out[11] = std::to_string( info.mem_unit );
   return( out );
}
