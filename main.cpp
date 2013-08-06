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
#include "heavy_process.tcc"
#include "load.hpp"
#include "noop_loop.hpp"


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

   process = new HeavyProcess<NoOpLoop, NoOpLoop::Data >( cmd );

   assert( process != nullptr );

   /* process args */
   cmd.processArgs( argc, argv );

   /* check help */
   if( help ){  cmd.printArgs(); exit( EXIT_SUCCESS ); }

   process->Launch();

   /* everybody else should be done at this point, get data */
   std::stringstream ss;
   process->PrintHeader( ss );
   process->PrintData( ss );
   std::cout << "TestData:\n";
   std::cout << ss.str() << "\n";

   if( process != nullptr )
   {
      delete( process );
   }
   return( EXIT_SUCCESS );
}
