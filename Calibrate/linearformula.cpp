/**
 * linearformula.cpp - 
 * @author: Jonathan Beard
 * @version: Tue Sep 10 10:57:10 2013
 */
#include "linearformula.hpp"

/** for sscanf **/
#include <cstdio>
#include <cassert>
#include <sstream>

LinearFormula::LinearFormula() : Formula(),
                                 c0( 0.0 ),
                                 c1( 0.0 ),
                                 cov00( 0.0 ),
                                 cov01( 0.0 ),
                                 cov11( 0.0 ),
                                 sumsq( 0.0 )
{
   /* nothing really to do */
}

LinearFormula::LinearFormula( double C0, 
                              double C1, 
                              double COV00,
                              double COV01,
                              double COV11,
                              double SUMSQ ) : Formula(),
                                               c0( C0 ),
                                               c1( C1 ),
                                               cov00( COV00 ),
                                               cov01( COV01 ),
                                               cov11( COV11 ),
                                               sumsq( SUMSQ )
{
   /* nothing really to do */
}


LinearFormula::~LinearFormula()
{
   c0    = 0.0;
   c1    = 0.0;
   cov00 = 0.0;
   cov01 = 0.0;
   cov11 = 0.0;
   sumsq = 0.0;
}

std::ostream&
LinearFormula::print( std::ostream &stream )
{
   stream << "c0: " << c0 << "\n";
   stream << "c1: " << c1 << "\n";
   stream << "cov00: " << cov00 << "\n";
   stream << "cov01: " << cov01 << "\n";
   stream << "cov11: " << cov11 << "\n";
   stream << "sumsq: " << sumsq << "\n";
   return( stream );
}

void
LinearFormula::load( std::istream &formula )
{
   std::string line;
   /* TODO: hacky way to do this, fix so they can be out of order */
   int index( 0 );
   while( formula.good() )
   {
      std::getline( formula, line, '\n' );
      switch( index++ )
      {
         case( 0 ):
         {
            const char *str( line.c_str() );
            assert( sscanf( str, "c0: %lg", &c0 ) == 1 );
         }
         break;
         case( 1 ):
         {
            const char *str( line.c_str() );
            assert( sscanf( str, "c1: %lg", &c1 ) == 1 );

         }
         break;
         case( 2 ):
         {
            const char *str( line.c_str() );
            assert( sscanf( str, "cov00: %lg", &cov00 ) == 1 );

         }
         break;
         case( 3 ):
         {
            const char *str( line.c_str() );
            assert( sscanf( str, "cov01: %lg", &cov01 ) == 1 );

         }
         break;
         case( 4 ):
         {
            const char *str( line.c_str() );
            assert( sscanf( str, "cov11: %lg", &cov11 ) == 1 );

         }
         break;
         case( 5 ):
         {
            const char *str( line.c_str() );
            assert( sscanf( str, "sumsq: %lg", &sumsq ) == 1 );
         }
         break;
         default:
         {
            std::cerr << "Invalid characters \"" << line << "\", skipping!!\n";
         }

      }
   }
}

double
LinearFormula::solve( std::vector< double > &parameters )
{
   /* linear formula c0 + c1*x */
   assert( parameters.size() == 1 );
   double out = c0 + (c1 * parameters[0] );
   return( out );
}
