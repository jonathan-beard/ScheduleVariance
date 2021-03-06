/**
 * command_option_single.hpp - 
 * @author: Jonathan Beard
 * @version: Tue May 21 15:12:42 2013
 */
#ifndef _COMMAND_OPTION_SINGLE_TCC_
#define _COMMAND_OPTION_SINGLE_TCC_  1

#include <functional>
#include <typeinfo>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "command_option_base.hpp"

template <class T> class Option : public OptionBase {
   public:
      /**
       * Option - An option constructor for an option that
       * sets a single value.  It takes a reference to what
       * we're setting (in), a flag (Flag), a description of
       * what the user is expected to be setting (Description),
       * a function that is used if any special type coversion is
       * needed (TypeConversion) which can be set to nullptr if no
       * type conversion is wanted, next there's another function
       * that can specify custum print behavior (PrintBehavior) which 
       * is also set to nullptr by default
       * @param   in - T&
       * @param   Flag - std::string
       * @param   Description - std::string
       * @param   Help - bool - set true if this is intended to be a help 
       *          function
       */
      Option(T &in, 
             std::string Flag, 
             std::string Description,
             bool Help = false) : OptionBase( Flag, Description, Help ),
                                  item( in )
      { 
         /* nothing really to do */
      }
      /**
       * Option - An option constructor for an option that
       * sets a single value.  It takes a reference to what
       * we're setting (in), a flag (Flag), a description of
       * what the user is expected to be setting (Description),
       * a function that is used if any special type coversion is
       * needed (TypeConversion) which can be set to nullptr if no
       * type conversion is wanted, next there's another function
       * that can specify custum print behavior (PrintBehavior) which 
       * is also set to nullptr by default
       * @param   in - T&
       * @param   Flag - std::string
       * @param   Description - std::string
       * @param   TypeConversion - custom type conversion function,
       *          takes in the char string and a bool argument as params,
       *          the char* is the cmd line argument, the bool should be
       *          set to true on success or false on failure of conversion
       * @param   PrintBehavior - takes in the current value T& and prints
       *          it with whatever pretty behavior you desire
       * @param   Help - bool - set true if this is intended to be a help 
       *          function
       */
      Option(T &in, 
             std::string Flag, 
             std::string Description,
             std::function< T (const char *, bool&) > TypeConversion,
             std::function< std::string ( T& ) >      PrintBehavior,
             bool Help = false) : OptionBase( Flag, Description, Help ),
                                  item( in ),
                                  F( TypeConversion ),
                                  PrettyPrint( PrintBehavior )
      { 
         /* nothing really to do */
      }

      virtual ~Option()
      { 
      };

      bool  
      setValue( const char* value )
      { 
         if( F != nullptr )
         {
            bool success( true );
            item = F( value , success );
            return( success );
         }
         T theRealValue;
         if( typeid(T) == typeid(bool) )
         {
            if( strcmp( value,  "true" ) == 0 )
            {
               (*(bool*)(&theRealValue)) = true;
            }else if( strcmp( value, "false" ) == 0 )
            {
               (*(bool*)(&theRealValue)) = false;
            }else{
               return( false );
            }
         }else if( typeid( T ) == typeid( int64_t ) )
         {
            errno = 0;
            (*(int64_t*)(&theRealValue)) = strtoll( value, NULL, 10 );
            if( errno != EXIT_SUCCESS ) return( false );
         }else if( typeid( T ) == typeid( double ) )
         {
            errno = 0;
            (*(double*)(&theRealValue)) = strtod( value, NULL );
            if( errno != EXIT_SUCCESS ) return( false );
         }
         else if( typeid( T ) == typeid( std::string ) )
         {
            (*(std::string*)(&theRealValue)) = std::string( value );
         }
         else{
            /* invalid type conversion */
            return( false );
         }
         item = theRealValue; 
         return( true );
      }

      std::string 
      toString()
      {
         return( OptionBase::toString( format_item( item ) ) );
      }

   private:
      /**
       * one stop shopping (hopefully) for printing
       * everything we could want 
       */
      std::string format_item( T &x ){
         if( PrettyPrint != nullptr )
         {
            return( PrettyPrint( x ) );
         }
         else if( typeid(x) == typeid(bool) )
         {
            if( *((bool*)(&x))  == true){
               return("true");
            }else{
               return("false");
            }
         }
         else if( typeid(x) == typeid(double) )
         {
            char buffer[100];
            memset(buffer, '\0', sizeof(char) * 100);
            snprintf(buffer, 100, "%0.3f", *((double*)(&x)));
            std::stringstream ss;
            ss << buffer;
            return( ss.str() );
         }
         else /* try to print and see if it works */
         {
            std::stringstream out;
            out << x;
            return( out.str() );
         }
      }

      T &item;
      std::function<T (const char*, bool&) > F;
      /** TODO, this func should throw an exception **/
      std::function< std::string( T& ) >     PrettyPrint;
};
#endif /* END _COMMAND_OPTION_SINGLE_TCC_ */
