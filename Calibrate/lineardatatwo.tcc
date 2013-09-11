#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>

/* gsl functions */
#include <gsl/gsl_fit.h>

#include "sample.hpp"

template < class T > class Data {
public:
   Data( std::vector< Sample > &samples ) : x( nullptr ),
                                            y( nullptr )
   {
      x = new T[ samples.size() ];
      y = new T[ samples.size() ];

      for( size_t i( 0 ); i < samples.size(); i++ )
      {
         x[i] = samples[i].index;
         y[i] = samples[i].measure;
      }
      size = samples.size();
   }

   ~Data()
   {
      delete[]( x );
      x = nullptr;
      delete[]( y );
      y = nullptr;
   }

   size_t
   get_size()
   {
      return( size );
   }

   T* get_x()
   {
      return( x );
   }

   T* get_y()
   {
      return( y );
   }

   size_t   get_stride()
   {
      return( 1 );
   }

   std::ostream& print( std::ostream &stream )
   {
      for( size_t i( 0 ); i < size; i++ )
      {
         stream << x[i] << " - " << y[i] << "\n";
      }
      return( stream );
   }

private:
   T *x;
   T *y;
   size_t size;
};
