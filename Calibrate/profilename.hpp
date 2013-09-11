/**
 * profilename.hpp - 
 * @author: Jonathan Beard
 * @version: Wed Sep 11 12:40:20 2013
 */
#ifndef _PROFILENAME_HPP_
#define _PROFILENAME_HPP_  1

#include <string>

class ProfileName{
public:
   ~ProfileName();

   static std::string GetProfileName( const std::string basedir );
private:
   ProfileName();
};

#endif /* END _PROFILENAME_HPP_ */
