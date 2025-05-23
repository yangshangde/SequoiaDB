/******************************************************************************/
// util/base64.h

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include "pd.hpp"
#include <boost/scoped_array.hpp>
#include <sstream>
#include <string>
#include <string.h>
#include "ossMemPool.hpp"

using namespace std;

namespace engine
{
   namespace base64
   {

      #define BASE64_CODETABLE_STANDARD (unsigned char*)\
                                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                                        "abcdefghijklmnopqrstuvwxyz" \
                                        "0123456789+/"

      class Alphabet
      {
      public:
         Alphabet( unsigned char* encodeTable = BASE64_CODETABLE_STANDARD )
         : encode( encodeTable ), decode( new unsigned char[257] )
         {

            memset( decode.get() , 0 , 256 );
            for ( int i = 0 ; i < 64 ; i++ )
            {
               decode[ encode[i] ] = i;
            }
            test();
         }
         void test()
         {
             SDB_ASSERT( strlen( (char*)encode ) == 64,
                         "encode is not 64 bytes" ) ;
             for ( int i = 0 ; i < 26 ; i++ )
             {
                 SDB_ASSERT( encode[i] == toupper( encode[i+26] ),
                             "bad encode" ) ;
             }
         }

         char e( int x ) const
         {
             return encode[x&0x3f];
         }

      private:
         const unsigned char * encode;
      public:
         boost::scoped_array<unsigned char> decode;
      };

      extern Alphabet alphabet;

      void encode( stringstream& ss, const char* data, int size ) ;
      string encode( const char* data, int size ) ;
      string encode( const string& s ) ;
      void decode( stringstream& ss, const string& s ) ;
      string decode( const string& s ) ;

      void encodeEx( ossPoolStringStream& ss, const char* data, int size,
                     const Alphabet* pAlphabet = &alphabet ) ;
      ossPoolString encodeEx( const char* data, int size,
                              const Alphabet* pAlphabet = &alphabet ) ;
      ossPoolString encodeEx( const ossPoolString& s,
                              const Alphabet* pAlphabet = &alphabet ) ;
      void decodeEx( ossPoolStringStream& ss, const char* data, int size,
                     const Alphabet* pAlphabet = &alphabet ) ;
      ossPoolString decodeEx( const char* data, int size,
                              const Alphabet* pAlphabet = &alphabet ) ;
      ossPoolString decodeEx( const ossPoolString& s,
                              const Alphabet* pAlphabet = &alphabet ) ;

   }
}
