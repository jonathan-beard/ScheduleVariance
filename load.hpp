/**
 * load.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:35 2013
 */
#ifndef _LOAD_HPP_
#define _LOAD_HPP_  1

struct ProcSettings;

class Load {
public:
   virtual void Launch( ProcSettings &s ) = 0;
};

#endif /* END _LOAD_HPP_ */
