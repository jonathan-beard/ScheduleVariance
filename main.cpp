/**
 * main.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 10:58:40 2013
 */
#include <unistd.h>
#include <cstdint>
#include <cinttypes>
#include <fstream>

#include "command_arguments.h"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

#include "process.hpp"
#include "heavy_process.tcc"
#include "load.hpp"
#include "noop_loop.hpp"
#include "noop_loop_unrolled.hpp"


int
main( int argc, char **argv )
{
   /* get command line arguments */
   CmdArgs cmd( "svar",
                std::cout,
                std::cerr );

   /* add help option */
   static bool help( false );
   std::string output_file( "/tmp/output.csv" );
   cmd.addOption( 
         new Option< bool >( help,
                             "-h",
                             "Print this message",
                             [](const char *x, bool &v){
                                 v = true;
                                 return( true );
                             },
                             nullptr,
                             true ) );
  
   static bool print_header( false );
   cmd.addOption( 
         new Option< bool >( print_header,
                             "-print_header",
                             "Print a header before data" ) );

   cmd.addOption( 
         new Option< std::string >( output_file,
                                    "-f",
                                    "Write output to file" ) );

   /* initialize processes & tests */
   Process *process( nullptr );

   process = 
      new HeavyProcess<NoOpLoop, NoOpLoop::Data >( cmd );

   /* process args */
   cmd.processArgs( argc, argv );

   /* check help */
   if( help || argc == 1){  cmd.printArgs(); exit( EXIT_SUCCESS ); }

   std::ofstream ofs( output_file, std::ofstream::out | std::ofstream::app );
   if( ! ofs.is_open() )
   {
      std::cerr << "Failed to open output file \"" << output_file << "\", exiting!!\n";
      exit( EXIT_FAILURE );
   }
   if( print_header ) 
      process->PrintHeader( ofs );
   if( ! print_header )
   {
      process->Launch( argv );
      /* child prcesses will have exited */
      /* everybody else should be done at this point, get data */
      process->PrintData( ofs );
   }
   ofs.close();
   delete( process );
   return( EXIT_SUCCESS );
}
