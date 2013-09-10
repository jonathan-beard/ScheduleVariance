/**
 * linearformula.hpp - 
 * @author: Jonathan Beard
 * @version: Tue Sep 10 10:57:10 2013
 */
#ifndef _LINEARFORMULA_HPP_
#define _LINEARFORMULA_HPP_  1

#include "formula.hpp"
#include <vector>
#include <iostream>
#include <string>

class LinearFormula : public Formula
{
public:
   LinearFormula();


   LinearFormula( double C0, 
                  double C1, 
                  double COV00,
                  double COV01,
                  double COV11,
                  double SUMSQ );
   
   virtual ~LinearFormula();

   virtual std::ostream& print( std::ostream &stream );

   virtual void load( std::stringstream &formula );

   virtual double solve( std::vector< double > &parameters );

protected:
   double c0;
   double c1;
   double cov00;
   double cov01;
   double cov11;
   double sumsq;
};
#endif /* END _LINEARFORMULA_HPP_ */
