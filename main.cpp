/**
 * main.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 10:58:40 2013
 */
#include <unistd.h>
#include <cstdint>
#include <cinttypes>

#include "command_arguments.h"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

#include "process.hpp"
#include "heavy_process.hpp"

#include "load.hpp"


int
main( int argc, char **argv )
{
   /* get command line arguments */
   CmdArgs cmd( "svar",
                std::cout,
                std::cerr );

   /* add help option */
   static bool help( false );
   cmd.addOption( 
         new Option< bool >( help,
                             "-h",
                             "Print this message",
                             [](const char *x, bool &v){
                                 v = true;
                                 return( true );
                             },
                             true ) );
  
   /* initialize processes & tests */
   Process *process( nullptr );
   Load    *load( nullptr );

   process = new HeavyProcess( cmd );
   assert( process != nullptr );

   /* process args */
   cmd.processArgs( argc, argv );

   /* check help */
   if( help ){  cmd.printArgs(); exit( EXIT_SUCCESS ); }

   process->Launch();

   return( EXIT_SUCCESS );

