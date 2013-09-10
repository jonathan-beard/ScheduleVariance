/**
 * calibrate.cpp - 
 * @author: Jonathan Beard
 * @version: Fri Sep  6 14:51:13 2013
 */
#include "calibrate.hpp"

//STARTCALIBRATEDECL
extern Sample threethousand();
extern Sample hundred();
extern Sample twothousand();
extern Sample fourhundred();
extern Sample twohundred();
extern Sample fourty();
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
extern Sample ten();
extern Sample twentythousand();
extern Sample tenthousand();
extern Sample fivehundred();
//ENDCALIBRATEDECL

Calibrate::Calibrate( double seconds )
{

//STARTFUNCTIONINSERT
samplefunctions.push_back( threethousand );
samplefunctions.push_back( hundred );
samplefunctions.push_back( twothousand );
samplefunctions.push_back( fourhundred );
samplefunctions.push_back( twohundred );
samplefunctions.push_back( fourty );
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
samplefunctions.push_back( ten );
samplefunctions.push_back( twentythousand );
samplefunctions.push_back( tenthousand );
samplefunctions.push_back( fivehundred );
//ENDFUNCTIONINSERT

/* set affinity */

for( SampleFunction &sf : samplefunctions )
{
   for( int i( 0 ); i < 1000; i++ )
   {
      samples.push_back( sf() );
   }
}

/* run linear regression */

}

Calibrate::~Calibrate()
{

}


