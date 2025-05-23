/*******************************************************************************


   Copyright (C) 2023-present SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Source File Name = ossAtomicBase.hpp

   Descriptive Name = Operating System Services Atomic Base Header

   When/how to use: this program may be used on binary and text-formatted
   versions of OSS component. This file contains declare for atomic operations.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          09/14/2012  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef OSS_ATOMIC_BASE_HPP
#define OSS_ATOMIC_BASE_HPP

/*
 *  Primative atomic types and routines shared by the OSS spin lock and 
 *  atomic counters.
 *  Current supported/target platforms are X86, X68_64
 */

#include "ossFeat.hpp"
#include "ossTypes.hpp"

#if defined ( _LINUX )

   #define ossCompilerFence() __asm__ __volatile__ ( "" ::: "memory" )

   // We are using gcc built-in __sync_*** functions for atomic memory
   // access, these builtins are considered as 'full barrier' in most
   // cases, i.e., no memory operand will be moved across the operation,
   // either forward or backword.  Further, instructions will be issued
   // as necessary to prevent the processor from speculating loads across
   // the operation and from queuing stores after the operation.

   // This guarantees that all load-from-memory instructions issued prior
   // the LFENCE instruction complete before subsequent load that follows
   // this instruction 
   #define ossX86LoadFence()  __asm__ __volatile__ ( "lfence" ::: "memory" )

   // This guarantees that all store-to-memory instructions issued prior
   // the SFENCE instruction complete before subsequent store that follows
   // this instruction 
   #define ossX86StoreFence() __asm__ __volatile__ ( "sfence" ::: "memory" )

   // This guarantees all load-from-memory and store-to-memory instsructions
   // complete before subsequent load and store that follow this instruction
   #define ossX86MemoryFence()  __asm__ __volatile__ ( "mfence" ::: "memory" )

#if defined (_PPCLIN64)
   #define ossYield() __asm__ __volatile__ ( "or 27,27,27" )
   #define ossAtomicExchange8(pAddr,iNewValue) \
        __sync_lock_test_and_set((volatile CHAR*)pAddr, (CHAR)iNewValue )
#else
   #if defined (_ARMLIN64)
      #define ossYield() __asm__ __volatile__ ( "yield": : :"memory" )
   #elif defined (_ALPHALIN64)
      #define ossYield()
   #else
      #define ossYield() __asm__ __volatile__ ( "pause\n": : :"memory" )
   #endif
   static OSS_INLINE char ossAtomicExchange8( volatile char* pAddr, char iNewValue)
   {
      char result ;
       __asm__ __volatile__
                       ("xchg %0, %1":"+m"(*pAddr),"=a"(result):"1"(iNewValue):
                        "memory") ;
      return result ;
   }
#endif
#elif defined ( _AIX )
   #define ossCompilerFence() __sync_synchronize()
   #define ossYield() __asm__ __volatile__ ( "or 27,27,27" )
   #define ossAtomicExchange8(pAddr,iNewValue) \
        __sync_lock_test_and_set((volatile CHAR*)pAddr, (CHAR)iNewValue )
#elif defined ( _WINDOWS )
   #include <intrin.h>
   #define ossCompilerFence() _ReadWriteBarrier()
   #define ossYield()         YieldProcessor()
#endif

// all atomic operations require aligned memory access,
// either 4bytes( 32bit ) or 8byte( 64bit ) boundary depends on
// the atomic type 

// atomic compare and swap of a 32bit value.
// return: the previous value before the operation
#if defined (_LINUX) || defined (_AIX)
#define ossCompareAndSwap32WithReturn(pAddr,iCompareValue,iNewValue) \
          __sync_val_compare_and_swap((volatile SINT32*)pAddr,\
                                      (SINT32)iCompareValue,\
                                      (SINT32)iNewValue)
#elif defined (_WINDOWS)
#define ossCompareAndSwap32WithReturn(pAddr,iCompareValue,iNewValue) \
   InterlockedCompareExchange((long*)pAddr,(long)iNewValue,(long)iCompareValue)
#endif

/*
static OSS_INLINE SINT32 ossCompareAndSwap32WithReturn
(
   volatile SINT32 * const pAddr,
   SINT32                  iCompareValue,
   SINT32                  iNewValue
)
{
#if defined ( _LINUX )
   return ( __sync_val_compare_and_swap( pAddr, iCompareValue, iNewValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedCompareExchange( (long*)pAddr, (long)iNewValue, (long)iCompareValue ) ) ;
#endif
}*/

// atomic compare and swap of a 32bit value.
// return : 1 success
//          0 failure
#if defined (_LINUX) || defined (_AIX)
#define ossCompareAndSwap32(pAddr,iCompareValue,iNewValue)                    \
   ( __sync_val_compare_and_swap((volatile SINT32*)pAddr,                    \
       (SINT32)iCompareValue, (SINT32)iNewValue ) == (SINT32)iCompareValue )
#elif defined (_WINDOWS)
#define ossCompareAndSwap32(pAddr,iCompareValue,iNewValue)                    \
   ( InterlockedCompareExchange((long*)pAddr,(long)iNewValue,                 \
       (long)iCompareValue) == (UINT32)iCompareValue )
#endif
/*
static OSS_INLINE BOOLEAN ossCompareAndSwap32
(
   volatile SINT32 * const pAddr,
   SINT32                  iCompareValue,
   SINT32                  iNewValue
)
{
#if defined ( _LINUX )
   return ( __sync_bool_compare_and_swap( pAddr, iCompareValue, iNewValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedCompareExchange( (long*)pAddr, (long)iNewValue, (long)iCompareValue ) ==
              (UINT32)iCompareValue ) ;
#endif
}
*/

// atomic compare and swap of a 64bit value.
// return: the previous value before the operation
#if defined (_LINUX) || defined (_AIX)
#define ossCompareAndSwap64WithReturn(pAddr,iCompareValue,iNewValue) \
          __sync_val_compare_and_swap((volatile SINT64*)pAddr,\
                                      (SINT64)iCompareValue,\
                                      (SINT64)iNewValue)
#elif defined (_WINDOWS)
#define ossCompareAndSwap64WithReturn(pAddr,iCompareValue,iNewValue) \
   InterlockedCompareExchange64((long long *)pAddr, (long long)iNewValue, \
                                (long long)iCompareValue )
#endif
/*
static OSS_INLINE SINT64 ossCompareAndSwap64WithReturn
(
   volatile SINT64 * const pAddr,
   SINT64                  iCompareValue,
   SINT64                  iNewValue
)
{
#if defined ( _LINUX )
   return (  __sync_val_compare_and_swap( pAddr, iCompareValue, iNewValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedCompareExchange64( (long long *)pAddr, (long long)iNewValue, (long long)iCompareValue ) ) ;
#endif
}*/


// atomic compare and swap of a 64bit value.
// return : 1 success
//          0 failure
#if defined (_LINUX) || defined (_AIX)
#define ossCompareAndSwap64(pAddr,iCompareValue,iNewValue)                   \
   ( __sync_val_compare_and_swap((volatile SINT64*)pAddr,                    \
       (SINT64)iCompareValue,(SINT64)iNewValue) == (SINT64)iCompareValue )
#elif defined (_WINDOWS)
#define ossCompareAndSwap64(pAddr,iCompareValue,iNewValue)                   \
   (InterlockedCompareExchange64( (long long*)pAddr,(long long)iNewValue,    \
      (long long)iCompareValue ) == (UINT64)iCompareValue )
#endif
/*
static OSS_INLINE BOOLEAN ossCompareAndSwap64
(
   volatile SINT64 * const pAddr,
   SINT64                  iCompareValue,
   SINT64                  iNewValue
)
{
#if defined ( _LINUX )
   return ( __sync_bool_compare_and_swap( pAddr, iCompareValue, iNewValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedCompareExchange64( (long long*)pAddr, (long long)iNewValue, (long long)iCompareValue ) ==
              (UINT64)iCompareValue ) ;
#endif
}*/


// atomic compare and swap operation of a pointer size variable
#if defined (OSS_ARCH_64)
#define ossCompareAndSwapPtrWithReturn(pAddr, compareValue,newValue) \
        (ossValuePtr)ossCompareAndSwap64WithReturn((volatile SINT64*) pAddr, \
                     (ossValuePtr)compareValue, (ossValuePtr)newValue)
#elif defined (OSS_ARCH_32)
#define ossCompareAndSwapPtrWithReturn(pAddr, compareValue,newValue) \
        (ossValuePtr)ossCompareAndSwap32WithReturn((volatile SINT32*) pAddr, \
                     (ossValuePtr)compareValue, (ossValuePtr)newValue)
#endif
/*
static OSS_INLINE ossValuePtr ossCompareAndSwapPtrWithReturn
(
   volatile ossValuePtr * pAddr,
   ossValuePtr            compareValue,
   ossValuePtr            newValue
)
{
#if defined ( OSS_ARCH_64 )
   return ( ( ossValuePtr )ossCompareAndSwap64WithReturn(
                                (volatile SINT64 * ) pAddr,
                                compareValue,
                                newValue ) ) ;
#elif defined ( OSS_ARCH_32 )
   return ( ( ossValuePtr )ossCompareAndSwap32WithReturn(
                                (volatile SINT32 * )pAddr,
                                compareValue,
                                newValue ) ) ;
#endif
}*/


// atomic compare and swap operation of a pointer size variable
// return : 1 success
//          0 failure
#if defined (OSS_ARCH_64)
#define ossCompareAndSwapPtr(pAddr, compareValue,newValue) \
         ossCompareAndSwap64((volatile SINT64*) pAddr, \
                     (ossValuePtr)compareValue, (ossValuePtr)newValue)
#elif defined (OSS_ARCH_32)
#define ossCompareAndSwapPtr(pAddr, compareValue,newValue) \
         ossCompareAndSwap32((volatile SINT32*) pAddr, \
                     (ossValuePtr)compareValue, (ossValuePtr)newValue)
#endif
/*
static OSS_INLINE BOOLEAN ossCompareAndSwapPtr
(
   volatile ossValuePtr * pAddr,
   ossValuePtr            compareValue,
   ossValuePtr            newValue
)
{
#if defined ( OSS_ARCH_64 )
   return ( ossCompareAndSwap64( ( volatile SINT64 * )pAddr,
                 compareValue,
                                 newValue ) ) ;
#elif defined ( OSS_ARCH_32 )
   return ( ossCompareAndSwap32( ( volatile SINT32* )pAddr,
                                 compareValue,
                                 newValue ) ) ;
#endif
}*/


// atomic 32bit add
// return : the value before the addition.
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndAdd32(pAddr, iAddVale) \
        __sync_fetch_and_add( (volatile SINT32*)pAddr, (SINT32)iAddVale )
#elif defined (_WINDOWS)
#define ossFetchAndAdd32(pAddr, iAddVale) \
         InterlockedExchangeAdd( (long*)pAddr, (long)iAddVale )
#endif
/*
static OSS_INLINE SINT32 ossFetchAndAdd32
(
   volatile SINT32 * const pAddr,
   SINT32                  iAddVale
)
{
#if defined ( _LINUX )
   return (  __sync_fetch_and_add( pAddr, iAddVale ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedExchangeAdd( (long*)pAddr, (long)iAddVale ) ) ;
#endif
}*/


// atomic 32bit OR operation
// return : the value before the OR operation
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndOR32(pAddr, iValue) \
        __sync_fetch_and_or( (volatile SINT32*)pAddr, (SINT32)iValue )
#elif defined (_WINDOWS)
#define ossFetchAndOR32(pAddr, iValue) \
        _InterlockedOr( (long*)pAddr, (long)iValue )
#endif
/*
static OSS_INLINE SINT32 ossFetchAndOR32
(
   volatile SINT32 * const pAddr,
   SINT32                  iValue
)
{
#if defined ( _LINUX )
   return ( __sync_fetch_and_or( pAddr, iValue ) ) ;
#elif defined ( _WINDOWS )
   return ( _InterlockedOr( (long*)pAddr, (long)iValue ) ) ;
#endif
}*/


// atomic 32bit AND operation
// return : the value before the AND operation
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndAND32(pAddr,iValue) \
        __sync_fetch_and_and((volatile SINT32*)pAddr,(SINT32)iValue)
#elif defined (_WINDOWS)
#define ossFetchAndAND32(pAddr,iValue) \
        _InterlockedAnd( (long*)pAddr, (long)iValue )
#endif
/*
static OSS_INLINE SINT32 ossFetchAndAND32
(
   volatile SINT32 * const pAddr,
   SINT32                  iValue
)
{
#if defined ( _LINUX )
   return ( __sync_fetch_and_and( pAddr, iValue ) ) ;
#elif defined ( _WINDOWS )
   return ( _InterlockedAnd( (long*)pAddr, (long)iValue )  ) ;
#endif
}*/


// atomic 32bit XOR operation
// return : the value before the XOR operation
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndXOR32(pAddr, iValue) \
        __sync_fetch_and_xor((volatile SINT32*)pAddr, (SINT32)iValue)
#elif defined (_WINDOWS)
#define ossFetchAndXOR32(pAddr, iValue) \
        _InterlockedXor( (long*)pAddr, (long)iValue )
#endif
/*
static OSS_INLINE SINT32 ossFetchAndXOR32
(
   volatile SINT32 * const pAddr,
   SINT32                  iValue
)
{
#if defined ( _LINUX )
   return ( __sync_fetch_and_xor( pAddr, iValue ) ) ;
#elif defined ( _WINDOWS )
   return (  _InterlockedXor( (long*)pAddr, (long)iValue ) ) ;
#endif
}*/


// atomic 32bit increment
#define ossFetchAndIncrement32(pAddr) \
        ossFetchAndAdd32((volatile SINT32*)pAddr,1)
/*
static OSS_INLINE SINT32 ossFetchAndIncrement32( volatile SINT32 * const pAddr )
{
   return ossFetchAndAdd32( pAddr, 1 ) ;
}*/


// atomic 32bit decrement
#define ossFetchAndDecrement32(pAddr) \
        ossFetchAndAdd32((volatile SINT32*)pAddr,-1)
/*
static OSS_INLINE SINT32 ossFetchAndDecrement32( volatile SINT32 * const pAddr )
{
   return ossFetchAndAdd32( pAddr, -1 ) ;
}*/


// atomic 64bit add
// return : the value before the addition.
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndAdd64(pAddr,iAddVale) \
        __sync_fetch_and_add((volatile SINT64*)pAddr,(SINT64)iAddVale)
#elif defined (_WINDOWS)
#define ossFetchAndAdd64(pAddr,iAddVale) \
        InterlockedExchangeAdd64( (long long*)pAddr, (long long)iAddVale )
#endif
/*
static OSS_INLINE SINT64 ossFetchAndAdd64
(
   volatile SINT64 * const pAddr,
   SINT64                  iAddVale
)
{
#if defined ( _LINUX )
   return ( __sync_fetch_and_add( pAddr, iAddVale ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedExchangeAdd64( (long long*)pAddr, (long long)iAddVale ) ) ;
#endif
}*/


// atomic 64bit OR operation
// return : the value before the OR operation
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndOR64(pAddr,iValue) \
        __sync_fetch_and_or((volatile SINT64*)pAddr,(SINT64)iValue)
#elif defined (_WINDOWS)
#define ossFetchAndOR64(pAddr,iValue) \
        InterlockedOr64( (long long*)pAddr, (long long)iValue )
#endif
/*
static OSS_INLINE SINT64 ossFetchAndOR64
(
   volatile SINT64 * const pAddr,
   SINT64                  iValue
)
{
#if defined ( _LINUX ) || defined (_AIX)
   return ( __sync_fetch_and_or( pAddr, iValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedOr64( (long long*)pAddr, (long long)iValue ) ) ;
#endif
}*/


// atomic 64bit XOR operation
// return : the value before the XOR operation
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndXOR64(pAddr, iValue) \
        __sync_fetch_and_xor((volatile SINT64*)pAddr,(SINT64)iValue)
#elif defined (_WINDOWS)
#define ossFetchAndXOR64(pAddr, iValue) \
        InterlockedXor64( (long long*)pAddr, (long long)iValue )
#endif
/*
static OSS_INLINE SINT64 ossFetchAndXOR64
(
   volatile SINT64 * const pAddr,
   SINT64                  iValue
)
{
#if defined ( _LINUX )
   return ( __sync_fetch_and_xor( pAddr, iValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedXor64( (long long*)pAddr, (long long)iValue ) ) ;
#endif
}*/


// atomic 64bit AND operation
// return : the value before the AND operation
#if defined (_LINUX) || defined (_AIX)
#define ossFetchAndAND64(pAddr, iValue) \
        __sync_fetch_and_and((volatile SINT64*)pAddr,(SINT64)iValue)
#elif defined (_WINDOWS)
#define ossFetchAndAND64(pAddr, iValue) \
        InterlockedAnd64( (long long*)pAddr, (long long)iValue )
#endif
/*
static OSS_INLINE SINT64 ossFetchAndAND64
(
   volatile SINT64 * const pAddr,
   SINT64                  iValue
)
{
#if defined ( _LINUX )
   return ( __sync_fetch_and_and( pAddr, iValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedAnd64( (long long*)pAddr, (long long)iValue ) ) ;
#endif
}*/


// atomic 64bit increment
#define ossFetchAndIncrement64(pAddr) \
        ossFetchAndAdd64((volatile SINT64*)pAddr,1)
/*
static OSS_INLINE SINT64 ossFetchAndIncrement64( volatile SINT64 * const pAddr )
{
   return ossFetchAndAdd64( pAddr, 1 ) ;
}*/


// atomic 64bit decrement
#define ossFetchAndDecrement64(pAddr) \
        ossFetchAndAdd64((volatile SINT64*)pAddr,-1)
/*
static OSS_INLINE SINT64 ossFetchAndDecrement64( volatile SINT64 * const pAddr )
{
   return ossFetchAndAdd64( pAddr, -1 ) ;
}*/

// atomic 8bit exchange
// return: previous value
/*#if defined (_LINUX)
#define ossAtomicExchange8(pAddr,iNewValue) \
        __sync_lock_test_and_set((volatile CHAR*)pAddr, (CHAR)iNewValue )
#elif defined (_WINDOWS)
#define ossAtomicExchange8(pAddr,iNewValue) \
       InterlockedExchange8((CHAR*)pAddr,(CHAR)iNewValue)
#endif*/

// atomic 32bit exchange
// return: previous value
#if defined (_LINUX) || defined (_AIX)
#define ossAtomicExchange32(pAddr,iNewValue) \
        __sync_lock_test_and_set((volatile INT32*)pAddr,(SINT32)iNewValue)
#elif defined (_WINDOWS)
#define ossAtomicExchange32(pAddr,iNewValue) \
        InterlockedExchange( (long*)pAddr, (long)iNewValue )
#endif
/*
static OSS_INLINE SINT32 ossAtomicExchange32
(
   volatile SINT32 * const pAddr,
   SINT32                  iNewValue
)
{
#if defined ( _LINUX )
   return ( __sync_lock_test_and_set( pAddr, iNewValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedExchange( (long*)pAddr, (long)iNewValue ) ) ;
#endif
}*/


// atomic 64bit exchange
// return: previous value
#if defined (_LINUX) || defined (_AIX)
#define ossAtomicExchange64(pAddr,iNewValue) \
        __sync_lock_test_and_set((volatile SINT64*)pAddr,(SINT64)iNewValue)
#elif defined (_WINDOWS)
#define ossAtomicExchange64(pAddr,iNewValue) \
        InterlockedExchange64( (long long*)pAddr, (long long)iNewValue )
#endif
/*
static OSS_INLINE SINT64 ossAtomicExchange64
(
   volatile SINT64 * const pAddr,
   SINT64                  iNewValue
)
{
#if defined ( _LINUX )
   return ( __sync_lock_test_and_set( pAddr, iNewValue ) ) ;
#elif defined ( _WINDOWS )
   return ( InterlockedExchange64( (long long*)pAddr, (long long)iNewValue ) ) ;
#endif
}*/


// atomic exchange operation of a pointer size variable
#if defined (OSS_ARCH_64)
#define ossAtomicExchangePtr(pAddr,newValue) \
        ossAtomicExchange64( ( volatile SINT64 * )pAddr, (ossValuePtr)newValue )
#elif defined (OSS_ARCH_32)
#define ossAtomicExchangePtr(pAddr,newValue) \
        ossAtomicExchange32( ( volatile SINT32 * )pAddr, (ossValuePtr)newValue )
#endif
/*
static OSS_INLINE ossValuePtr ossAtomicExchangePtr
(
   volatile ossValuePtr * pAddr,
   ossValuePtr            newValue 
)
{

#if defined ( OSS_ARCH_64 )
   return ( ossAtomicExchange64( ( volatile SINT64 * )pAddr, newValue ) ) ;
#elif defined ( OSS_ARCH_32 )
   return ( ossAtomicExchange32( ( volatile SINT32 * )pAddr, newValue ) ) ;
#endif
}*/


// atomic fetch 32bit data
#define ossAtomicFetch32(pAddr) \
        ossFetchAndAdd32((volatile SINT32*)pAddr, 0)
/*
static OSS_INLINE SINT32 ossAtomicFetch32( volatile SINT32 * const pAddr )
{
   return ( ossFetchAndAdd32( pAddr, 0 ) ) ;
}*/


// atomic fetch 64bit data
#define ossAtomicFetch64(pAddr) \
        ossFetchAndAdd64((volatile SINT64*)pAddr, 0)
/*
static OSS_INLINE SINT64 ossAtomicFetch64( volatile SINT64 * const pAddr )
{
   return ( ossFetchAndAdd64( pAddr, 0 ) ) ;	 
}*/


// un-ordered fetch 32bit data
#define ossAtomicPeek32(pAddr) *(SINT32*)pAddr
/*
static OSS_INLINE SINT32 ossAtomicPeek32( volatile const SINT32 * const pAddr )
{
   return *pAddr ;
}*/


// un-ordered fetch 64bit data
#define ossAtomicPeek64(pAddr) *(SINT64*)pAddr
/*
static OSS_INLINE SINT64 ossAtomicPeek64( volatile const SINT64 * const pAddr )
{
   return *pAddr ;
}*/

#endif // OSS_ATOMIC_BASE_HPP

