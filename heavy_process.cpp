/**
 * heavy_process.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:03:38 2013
 */
#include "heavy_process.hpp"

/* static initializers */
int64_t HeavyProcess::number_of_children = 0;
int64_t HeavyProcess::assigned_processor = 0;
int64_t HeavyProcess::load_length = 0;
int64_t HeavyProcess::number_of_iterations = 0;

HeavyProcess::HeavyProcess()
{
   /* nothing really to do here */
}

HeavyProcess::~HeavyProcess()
{
   /* nothing really to do here */
}

void
HeavyProcess::Launch( )
{
   
}

std::ostream&
Print( std::ostream &stream )
{

}
