/*****************************************************************************

        def.h
        Author: Laurent de Soras, 2010

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_def_HEADER_INCLUDED)
#define	fstb_def_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{



#define fstb_ARCHI_X86	1
#define fstb_ARCHI_ARM	2

#if defined (__i386__) || defined (_M_IX86) || defined (_X86_) || defined (_M_X64) || defined (__x86_64__) || defined (__INTEL__)
	#define fstb_ARCHI	fstb_ARCHI_X86
#elif defined (__arm__) || defined (_M_ARM) || defined (__aarch64__)
	#define fstb_ARCHI	fstb_ARCHI_ARM
#else
	#error
#endif



// Native word size, in power of 2 bits
#if defined (_WIN64) || defined (__64BIT__) || defined (__amd64__) || defined (__x86_64__)
	#define fstb_WORD_SIZE_L2      6
	#define fstb_WORD_SIZE        64
	#define fstb_WORD_SIZE_BYTE    8
#else
	#define fstb_WORD_SIZE_L2      5
	#define fstb_WORD_SIZE        32
	#define fstb_WORD_SIZE_BYTE    4
#endif



// Function inlining
#if defined (_MSC_VER)
   #define  fstb_FORCEINLINE   __forceinline
#elif defined (__GNUC__)
	#define  fstb_FORCEINLINE   inline __attribute__((always_inline))
#else
   #define  fstb_FORCEINLINE   inline
#endif



#if defined (_MSC_VER)
	#define	fstb_TYPEDEF_ALIGN( alignsize, srctype, dsttype)	\
		typedef __declspec (align (alignsize)) srctype dsttype
#elif defined (__GNUC__)
	#define	fstb_TYPEDEF_ALIGN( alignsize, srctype, dsttype)	\
		typedef srctype __attribute__ ((aligned (alignsize))) dsttype
#else
	#error Undefined for this compiler
#endif



const long double PI  = 3.1415926535897932384626433832795L;
const long double LN2 = 0.69314718055994530941723212145818L;



}	// namespace fstb



//#include "fstb/def.hpp"



#endif	// fstb_def_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
