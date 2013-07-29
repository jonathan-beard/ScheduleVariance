/**
 * heavy_process.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:03:38 2013
 */
#include "heavy_process.hpp"
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

/* static initializers */
int64_t HeavyProcess::spawn = 0;
/* might want to update this later to check interrupt mask */ 
int64_t HeavyProcess::assigned_processor = 0;
/* default is multi-level q on most systems */
int64_t HeavyProcess::schedule = SCHED_OTHER; 

HeavyProcess::HeavyProcess(CmdArgs &cmd) : 
                               Process( cmd ),
                               list( nullptr )
{
   /* register cmd arguments */
   cmd_args.addOption( 
         new Option< int64_t >( HeavyProcess::spawn,
                                "-p#",
                                "Number of processes to spawn" ) );
   /* add set processor option */
   cmd_args.addOption(
         new Option< int64_t >( HeavyProcess::assigned_processor,
                                "-core#",
                                "Which processor to run on" ) );

   cmd_args.addOption(
         new Option< int64_t >( HeavyProcess::schedule,
                                "-sched",
                         std::string("Schedule, should be one of: SCHED_OTHER,")+
                         std::string("SCHED_FIFO,SCHED_RR"),
                                [&](const char *val, bool &succ)
                                {
                                    const std::string str( val );
                                    succ = true;
                                    if( str.compare( "SCHED_OTHER" ) == 0 )
                                    {
                                       return( SCHED_OTHER );
                                    }
                                    else if( str.compare( "SCHED_FIFO" ) == 0 )
                                    {
                                       return( SCHED_FIFO );
                                    }
                                    else if( str.compare( "SCHED_RR" ) == 0 )
                                    {
                                       return( SCHED_RR );
                                    } /* else */
                                    succ = false;
                                    return( SCHED_OTHER );
                                }) );

}

HeavyProcess::~HeavyProcess()
{
   /* nothing really to do here */
}

void
HeavyProcess::Launch( )
{
   /* schedule can be inherited so lets do it here for all */
   errno = EXIT_SUCCESS;
   const auto priority( sched_get_priority_max( schedule ) );
   assert( errno == EXIT_SUCCESS );
   errno = EXIT_SUCCESS;
   const struct sched_param sp = { .sched_priority = priority };
   auto sch_ret_val( sched_setscheduler(0x0 /* this */,
                                        HeavyProcess::schedule,
                                        &sp ) );
   if( sch_ret_val )
   {
      perror( "Failed to set scheduler" );
      exit( EXIT_FAILURE );
   }

   pid_t child( 0 ); 
   for( int64_t j( 1 ); j < HeavyProcess::spawn; j++ )
   {
      child = fork();
      switch( child )
      {
         case( 0 /* CHILD */ ):
         {  
            /* go to end */
            fprintf(stderr, "spawn\n");
            goto END;
         }
         break;
         case( -1 /* FAILURE, THIS IS PARENT */ ):
         {
            std::stringstream err;
            /* need to keep a list of all child processes and kill them */
            for( pid_t id : *list )
            {
               if( kill( id, SIGQUIT ) != EXIT_SUCCESS )
               {
                  err << "Failed to kill pid ( " << id << " )\n";
               }
            }
            const std::string err_str( err.str() );
            if( err_str.length() > 0 )
            {
               std::cerr << err_str << "\n";
            }
            exit( EXIT_FAILURE );
         }
         break;
         default: /* this is the PARENT */
         {
            if( list == nullptr )
            {
               list = new std::vector< pid_t >();
            }
            list->push_back( child );
            continue;
         }
      }
   }
   END:; /* goto empty statement */
}

std::ostream&
HeavyProcess::Print( std::ostream &stream )
{
   stream << "Not implemented\n"; 
   return( stream );
}
