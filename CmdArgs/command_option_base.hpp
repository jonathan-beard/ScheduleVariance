/**
 * command_option_base.hpp - 
 * @author: Jonathan Beard
 * @version: Tue May 21 13:56:51 2013
 */
#ifndef _COMMAND_OPTION_BASE_HPP_
#define _COMMAND_OPTION_BASE_HPP_  1

#include <string>

class OptionBase{
public:
   OptionBase( const std::string flag,
               const std::string description,
               bool  isHelp = false );

   virtual ~OptionBase();


   virtual bool setValue( const char *value ) = 0;
   virtual std::string toString() = 0;

   bool is_help();
   std::string& get_flag();
   std::string& get_description();

protected:
   std::string toString( const std::string defaultValue );

private:
   std::string flag;
   std::string description;
   bool        help;
};
#endif /* END _COMMAND_OPTION_BASE_HPP_ */
