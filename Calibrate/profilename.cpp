/**
 * profilename.cpp - 
 * @author: Jonathan Beard
 * @version: Wed Sep 11 12:40:20 2013
 */
#include "profilename.hpp"
#include "queryCPUID.h"

#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <vector>
#include <sstream>

#include <sys/utsname.h>
#include <errno.h>
ProfileName::~ProfileName()
{
   /* nothing to do here */
}

std::string
ProfileName::GetProfileName( const std::string basedir )
{
   struct utsname sysname;
   memset( &sysname, 
           0x0, 
           sizeof( struct utsname ) );
   errno = 0;
   if( uname( &sysname ) != 0 )
   {
      perror( "Failed to get systen name when calling uname!!\n" );
      exit( EXIT_FAILURE );
   }
   
   std::vector< std::string > values;

   /* kernel name */
   values.push_back( std::string( sysname.sysname ) );
   /* get kernel release */
   values.push_back( std::string( sysname.release ) );
   /* get nodename */
   values.push_back( std::string( sysname.nodename ) );
   /* get cpu brand */
   std::string brand("BrandUnknown");
   if( is_cpu_id_supported() )
   {
      Reg input, output;
      zero_registers( &input );
      zero_registers( &output );
      input.eax = CPUID_BASIC;
      get_cpuid( &input, &output );
      brand = std::string( getBrandName( &output ) );
   }
   values.push_back( brand );
   /* get cpu model, family, stepping id(s) */
   std::string model( "UnknownModel" );
   std::string family( "UnknownFamily" );
   std::string stepping( "UnknownStepping" );
   if( is_cpu_id_supported() )
   {
      Reg input, output;
      zero_registers( &input );
      zero_registers( &output );
      input.eax = CPUID_LEVEL1;
      get_cpuid( &input, &output );
      VersionInfoTwo vi;
      vi.u_int = (output.eax );  
      model = std::to_string( vi.bits.MODEL );
      family = std::to_string( vi.bits.FAMILY_ID );
      stepping = std::to_string( vi.bits.STEPPING_ID );
   }
   values.push_back( model );
   values.push_back( family );
   values.push_back( stepping );
   std::stringstream profilename;
   profilename << "Profile";
   for( std::string &str : values )
   {
      profilename << "_" << str;
   }
   /* remove characters that aren't supposed to be in a file name */
   std::string draftoutput( profilename.str() );
   const std::string illchars( "\\/:?\"<>|." );
   for( auto it( draftoutput.begin() ); it != draftoutput.end(); ++it )
   {
      const bool found( illchars.find( (*it) ) != std::string::npos );
      if( found )
         (*it) = '_';
   }
   std::stringstream finaloutput;
   finaloutput << basedir << "/" << draftoutput;
   return( finaloutput.str() );
}
