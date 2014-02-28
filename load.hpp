/**
 * load.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:35 2013
 */
#ifndef _LOAD_HPP_
#define _LOAD_HPP_  1


#include <ostream>
#include <cstdint>
#include "command_arguments.h"
#include "command_option_base.hpp"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

class Process;
class GateKeeper;

class Load {
public:
   /**
    * Data - extend this struct to save state 
    */
   class Data{
   public:
      Data() = default;
      Data( const Data &d ) = default;
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

   void  Run( Process &p, GateKeeper &g );

   /**
    * AllSet - to be overrided by sub-classes if needed, called
    * by the process object to ensure that the load is in fact 
    * all set to go and that the current settings are sane.  The
    * default version here simply returns true so if no custom
    * behavior is needed, feel free not to implement it.  This 
    * function will be called before the processes are spawned
    * and executed.
    *
    * @return  bool - true if all set and ready to run load.
    */
   virtual bool  AllSet();

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
    * RunLoad - Run the load, take in a process reference for
    * data storage and synchronization.  Will be called the 
    * specified number of iterations.
    *
    * @param   p - Process&
    * @param   g - GateKeeper Object
    * @param   i - current iteration
    */
   virtual void RunLoad( Process &p, GateKeeper &g, int64_t i) = 0;

   /**
    * local reference to cmd args 
    */
   CmdArgs  &cmd_args;
};

#endif /* END _LOAD_HPP_ */
