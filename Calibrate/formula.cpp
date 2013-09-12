/**
 * formula.cpp - 
 * @author: Jonathan Beard
 * @version: Tue Sep 10 09:11:09 2013
 */
#include "formula.hpp"
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <sstream>

Formula::Formula()
{
}

Formula::~Formula()
{
}

void
Formula::InitFromFile( const char *filename )
{
   
   assert( filename != nullptr );
   /** open file **/
   std::ifstream in_file( filename, std::ifstream::in );
   if( in_file.is_open() )
   {
 /*
      std::stringstream ss;
      std::string line;
      while( in_file.good() )
      {
         std::getline( in_file, line );
         ss << line;
      }
      in_file.close();
*/
      load( in_file );
      
      in_file.close();
   }
   else
   {
      /* couldn't open file!! */
      std::cerr << "Failed to open file \"" << filename << "\", exiting!!\n";
      exit( EXIT_FAILURE );
   }
}

void
Formula::write( const char *filename )
{
   assert( filename != nullptr );
   std::ofstream out_file( filename );
   if( out_file.is_open() )
   {
      print( out_file );
   }
   else
   {
      std::cerr << "Failed to open file \"" << filename << "\" to write"
         << " formula, exiting\n";
      exit( EXIT_FAILURE );
   }
   out_file.close();
}
