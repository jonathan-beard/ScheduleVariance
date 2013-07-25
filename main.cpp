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

#include "datadefs.hpp"

/** One static struct for all **/
static ProcSettings settings;

int
main( int argc, char **argv )
{
   CmdArgs cmd;
   /* add help option */
   bool help( false );
   cmd.addOption( 
         new Option< bool >( help,
                             "-h",
                             "Print this message",
                             [](const char *x, bool &v){
                                 v = true;
                                 return( true );
                             },
                             true ) );
   /* add number of prcesses option */                             
   cmd.addOption( 
         new Option< int64_t >( settings.num_children,
                                "-p#",
                                "Number of processes to spawn" ) );
   /* add set processor option */
   cmd.addOption(
         new Option< int64_t >( settings.which_processor,
                                "-core#",
                                "Which processor to run on" ) );
   /* set service time */
   cmd.addOption( 
         new Option< double >( settings.
   /** START FORKING **/
   pid_t child_id( EXIT_FAILURE );
   
   return( EXIT_SUCCESS );
}
