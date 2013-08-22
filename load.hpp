/**
 * load.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:35 2013
 */
#ifndef _LOAD_HPP_
#define _LOAD_HPP_  1


#include <ostream>
#include "command_arguments.h"
#include "command_option_base.hpp"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

class Process;


class Load {
public:
   /**
    * Data - extend this struct to save state 
    */
   struct Data{
      Data()
      {
         /* nothing really to do */
      }

      Data( const Data &d )
      {
         /* nothing really to do */
      }
   }; 
   
   /**
    * Constructor - set the CmdArgs object that is created in 
    * main.  Internally this will add whatever options this
    * particular load object wants to throw in.
    * @param   args - CmdArgs&
    */
   Load( CmdArgs &args );

   /**
    * Destructor - clean up 
    */
   virtual ~Load();

   /**
    * Run - Run the load, take in a process reference for
    * data storage and synchronization
    * @param   p - Process&
    */
   virtual void Run( Process &p ) = 0;

   /**
    * PrintHeader - print a header to this function to the stream.
    * @param   stream - std::ostream&
    * @return  std::ostream&
    */
   virtual std::ostream& PrintHeader( std::ostream &stream ) = 0;

   /**
    * PrintData - method that lets the user define specific print
    * methods for each data struct.  The struct itself is passed
    * in as a void ptr, and is converted to the appropriate struct
    * within each sub-class method.  The first param to this function
    * is a valid stream to write the data.  This function will be
    * called over and over for each available data element that
    * is stored in the process object.
    * @param   stream - std::ostream&
    * @param   d      - void ptr
    * @return  std::ostream&
    */
   virtual std::ostream& PrintData( std::ostream &stream, void *d ) = 0;
   
protected:
   /**
    * local reference to cmd args 
    */
   CmdArgs  &cmd_args;

};

#endif /* END _LOAD_HPP_ */
