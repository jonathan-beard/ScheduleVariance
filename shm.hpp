/**
 * shm.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Aug  1 14:26:34 2013
 */
#ifndef _SHM_HPP_
#define _SHM_HPP_  1

class SHM{
public:
   static void*   Init( const char *key, 
                        size_t nbytes,
                        size_t nitems );

   static void*   Open( const char *key );

   static bool    Close( const char *key );

private:
   SHM();
   ~SHM();
};

#endif /* END _SHM_HPP_ */
