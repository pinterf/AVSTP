/*****************************************************************************

        ThreadMgr.cpp
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define	WIN32_LEAN_AND_MEAN
#define	NOMINMAX
#define	NOGDI

#include	"avstp/ThreadMgr.h"

#include	<windows.h>

#include	<cassert>



namespace avstp
{



static void	ThreadMgr_set_thread_name_internal (::DWORD size, const ::ULONG_PTR *info_ptr)
{
	__try
	{
		::RaiseException (0x406D1388, 0, size, info_ptr);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
		// Nothing
	}
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	ThreadMgr::count_nbr_logical_proc ()
{
	int            nbr_proc = 0;

	::DWORD_PTR    mask_proc;
	::DWORD_PTR    mask_sys;
	const ::HANDLE proc_hnd = ::GetCurrentProcess ();
	const ::BOOL   res =
		::GetProcessAffinityMask (proc_hnd, &mask_proc, &mask_sys);
	if (res != 0)
	{
		if (mask_proc == 0 && mask_sys == 0)
		{
			nbr_proc = 64;
		}
		else
		{
			while (mask_proc != 0)
			{
				nbr_proc += int (mask_proc) & 1;
				mask_proc >>= 1;
			}
		}
	}

	if (nbr_proc <= 0)
	{
		nbr_proc = 1;
	}

	return (nbr_proc);
}



// See: How to: Set a Thread Name in Native Code
// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
void	ThreadMgr::set_thread_name (const char *name_0)
{
	assert (name_0 != 0);

	struct
	{
		::DWORD			_type;		// must be 0x1000
		::LPCSTR			_name;		// pointer to name (in user addr space)
		::DWORD			_thread_id;	// thread ID (-1=caller thread)
		::DWORD			_flags;		// reserved for future use, must be zero
	}					info =
	{
		0x1000,
		name_0,
		-1,
		0
	};

	ThreadMgr_set_thread_name_internal (
		sizeof (info) / sizeof (::DWORD),
		reinterpret_cast <const ::ULONG_PTR *> (&info)
	);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
