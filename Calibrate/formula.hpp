/**
 * formula.hpp - 
 * @author: Jonathan Beard
 * @version: Tue Sep 10 09:11:09 2013
 */
#ifndef _FORMULA_HPP_
#define _FORMULA_HPP_  1

#include <iostream>
#include <string>
#include <vector>

class Formula{
public:
   /**
    * Formula - create a formula object from a stored representation,
    * calls the sub-class load function which is expected to initialize
    * all necessary class parameters.
    */
   Formula( );
  
   void InitFromFile( const char *filename );

   virtual ~Formula();

   /**
    * print - must be implemented to print out a correct 
    * representation of a formula
    */
   virtual std::ostream& print( std::ostream &stream ) = 0;
   /**
    * load - load a formula from a std::string representation,
    * sub-classes must implement this method since it is how
    * we will re-use calibration profiles. Throw an appropriate
    * exception if the string doesn't match.
    * @param   formula - const std::string
    */
   virtual void     load( std::stringstream &formula ) = 0;
   /**
    * solve - provides a way for sub-classes to give a unified
    * way to solve using this formula
    */
   virtual double   solve( std::vector< double > &parameters ) = 0;
   
   /**
    * write - calls print and writes that stream
    * to a file at filename, throws an appropriate
    * exception if the file cannot be written to.
    *
    * @param   filename - const char*
    */
   void write( const char *filename );
private:
   std::stringstream *the_formula; 
};

#endif /* END _FORMULA_HPP_ */
