/**
 * calibrate.cpp - 
 * @author: Jonathan Beard
 * @version: Fri Sep  6 14:51:13 2013
 */

#include <gsl/gsl_fit.h>
#include <errno.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif  
#include <sched.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>
#include <cstdint>
#include <sstream>
#include <cassert>
#include <cinttypes>
#include <cstring>

#include "calibrate.hpp"
#include "formula.hpp"
#include "linearformula.hpp"
#include "lineardatatwo.tcc"
#include "profilename.hpp"

//STARTCALIBRATEDECL
extern Sample forty();
extern Sample threethousand();
extern Sample hundred();
extern Sample twothousand();
extern Sample fourhundred();
extern Sample twohundred();
extern Sample sixty();
extern Sample eighty();
extern Sample thousand();
extern Sample fifty();
extern Sample fourthousand();
extern Sample seventy();
extern Sample twenty();
extern Sample fivethousand();
extern Sample ninety();
extern Sample threehundred();
extern Sample thirty();
extern Sample onehundredthousand();
extern Sample ten();
extern Sample twentythousand();
extern Sample tenthousand();
extern Sample fivehundred();
//ENDCALIBRATEDECL

Calibrate::Calibrate( double seconds, 
                      char **argv ) : basedirectory( "./Profiles" )
{
   
//STARTFUNCTIONINSERT
samplefunctions.push_back( forty );
samplefunctions.push_back( threethousand );
samplefunctions.push_back( hundred );
samplefunctions.push_back( twothousand );
samplefunctions.push_back( fourhundred );
samplefunctions.push_back( twohundred );
samplefunctions.push_back( sixty );
samplefunctions.push_back( eighty );
samplefunctions.push_back( thousand );
samplefunctions.push_back( fifty );
samplefunctions.push_back( fourthousand );
samplefunctions.push_back( seventy );
samplefunctions.push_back( twenty );
samplefunctions.push_back( fivethousand );
samplefunctions.push_back( ninety );
samplefunctions.push_back( threehundred );
samplefunctions.push_back( thirty );
samplefunctions.push_back( onehundredthousand );
samplefunctions.push_back( ten );
samplefunctions.push_back( twentythousand );
samplefunctions.push_back( tenthousand );
samplefunctions.push_back( fivehundred );
//ENDFUNCTIONINSERT

/* check for profile already in existence */
   const std::string profile_name( 
                        ProfileName::GetProfileName( basedirectory ) );
   struct stat st;
   memset( &st, 0x0, sizeof( struct stat ) );
   errno = 0;
   Formula *formula( nullptr );
   if( stat( profile_name.c_str() , &st ) == 0 )
   {
      /* open profile and use it */
      formula = new LinearFormula();
      formula->InitFromFile( profile_name.c_str() );
   }
   else /* create a new profile */
   {
      /* set affinity */
      const int success( 0 );
      /* pin processor */
      /* the cpuset should be inherited by forked processes */
      cpu_set_t   *cpuset( nullptr );
      const int8_t processors_to_allocate( 1 );
      size_t cpu_allocate_size( -1 );
#if   (__GLIBC_MINOR__ > 9 ) && (__GLIBC__ == 2 )
      cpuset = CPU_ALLOC( processors_to_allocate );
      assert( cpuset != nullptr );
      cpu_allocate_size =  CPU_ALLOC_SIZE( processors_to_allocate );
      CPU_ZERO_S( cpu_allocate_size, cpuset );
#else
      cpu_allocate_size = sizeof( cpu_set_t );
      cpuset = (cpu_set_t*) malloc( cpu_allocate_size );
      assert( cpuset != nullptr );
      CPU_ZERO( cpuset );
#endif
      /* TODO: unhard-code this */
      CPU_SET( 1, cpuset );
      auto setaffinity_ret_val( success );
      errno = success;
      setaffinity_ret_val = sched_setaffinity( 0 /* self */,
                                               cpu_allocate_size,
                                               cpuset );
      if( setaffinity_ret_val != success )
      {
         perror( "Failed to set processor affinity" );
         exit( EXIT_FAILURE );
      }

      /* collect samples */
      std::vector< Sample > samples;
      for( SampleFunction &sf : samplefunctions )
      {
         for( int i( 0 ); i < 1000; i++ )
         {
            samples.push_back( sf() );
         }
      }
      /* run linear regression */
      formula = Regress( samples );
      assert( formula != nullptr );
      
      /* write profile for future usage */
      std::ofstream  profile_output( profile_name );
      if( profile_output.is_open() )
      {
         formula->print( profile_output );
         profile_output.close();
      }
      else
      {
         std::cerr << "Failed to save profile, continueing though!!\n";
      }

   }
   assert( formula != nullptr );
   std::vector< double > params;
   params.push_back( seconds );
   /* now we have the number fo params */
   uint32_t  num_instructions = ( uint32_t )ceil( formula->solve( params ) );

   std::stringstream cmd_gen;
   cmd_gen << "./gen_noop_load.pl " << num_instructions << " " << seconds;
   /* use system to call perl script */
   if( system( cmd_gen.str().c_str() ) != 0 )
   {
      std::cerr << "Failed to execute system command to regenerate load!!\n";
      exit( EXIT_FAILURE );
   }
   /* now we have the new load, execute make */
   std::stringstream cmd_make_clean;
   cmd_make_clean << "make clean";
   if( system( cmd_make_clean.str().c_str() ) != 0 )
   {
      std::cerr << "Failed to execute system command to clean!!\n";
      exit( EXIT_FAILURE );
   }
   std::stringstream cmd_make_all;
   cmd_make_all << "make -j all";
   if( system( cmd_make_all.str().c_str() ) != 0 )
   {
      std::cerr << "Failed to execute system command to build all!!\n";
      exit( EXIT_FAILURE );
   }
   /* now we're ready to re-execute */
   errno = 0;
   if( execvp( "./svar", argv ) != 0 )
   {
      perror( "Failed to exec program!!" );
      exit( EXIT_FAILURE );
   }
}

Calibrate::~Calibrate()
{

}


/**
 * Regress - runs gsl linear regression routine on data collected, 
 * writes the appropriate formula to a profile file which is saved
 * for future use so this process doesn't have to be done over and 
 * over on a new machine.
 */
Formula*
Calibrate::Regress( std::vector< Sample > &samples )
{
   Data< double > data( samples );
   const size_t stride( data.get_stride() );
   double c0( 0.0 );
   double c1( 0.0 );
   double cov00( 0.0 );
   double cov01( 0.0 );
   double cov11( 0.0 );
   double sumsq( 0.0 );
   const int ret_val( gsl_fit_linear( data.get_x(), stride,
                                      data.get_y(), stride,
                                      data.get_size(),
                                      &c0, &c1,
                                      &cov00, &cov01, &cov11,
                                      &sumsq ) );
   if( ret_val != 0 )
   {
      std::cerr << "Error calling linear regress function!!\n";
      exit( EXIT_FAILURE );
   }
   Formula *output = new LinearFormula( c0, c1, cov00, cov01, cov11, sumsq );
   return( output );
}

