/*****************************************************************************

        main.cpp
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (4 : 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define	NOGDI
#define	NOMINMAX
#define	WIN32_LEAN_AND_MEAN

#include	"AvstpFinder.h"

#include	<windows.h>

#if defined (_MSC_VER)
#include	<crtdbg.h>
#include	<new.h>
#endif	// _MSC_VER

#include	<new>

#include	<cassert>



/*\\\ DEFINITIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#if defined (_MSC_VER)
static int __cdecl	avstp_new_handler_cb (size_t /*dummy*/)
{
	throw std::bad_alloc ();
//	return 0;
}
#endif	// _MSC_VER



static void	avstp_dll_load (::HINSTANCE hinst)
{
#if defined (_MSC_VER)
	::_set_new_handler (::avstp_new_handler_cb);
#endif	// _MSC_VER

#if defined (_MSC_VER) && ! defined (NDEBUG)
	{
		const int	mode =   (1 * _CRTDBG_MODE_DEBUG)
						       | (1 * _CRTDBG_MODE_WNDW);
		::_CrtSetReportMode (_CRT_WARN, mode);
		::_CrtSetReportMode (_CRT_ERROR, mode);
		::_CrtSetReportMode (_CRT_ASSERT, mode);

		const int	old_flags = ::_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
		::_CrtSetDbgFlag (  old_flags
		                  | (1 * _CRTDBG_LEAK_CHECK_DF)
		                  | (1 * _CRTDBG_CHECK_ALWAYS_DF));
		::_CrtSetBreakAlloc (-1);	// Specify here a memory bloc number
	}
#endif	// _MSC_VER, NDEBUG

	AvstpFinder::publish_lib (hinst);
}



static void	avstp_dll_unload (::HINSTANCE /*hinst*/)
{
#if defined (_MSC_VER) && ! defined (NDEBUG)
	{
		const int	mode =   (1 * _CRTDBG_MODE_DEBUG)
						       | (0 * _CRTDBG_MODE_WNDW);
		::_CrtSetReportMode (_CRT_WARN, mode);
		::_CrtSetReportMode (_CRT_ERROR, mode);
		::_CrtSetReportMode (_CRT_ASSERT, mode);

		::_CrtMemState	mem_state;
		::_CrtMemCheckpoint (&mem_state);
		::_CrtMemDumpStatistics (&mem_state);
	}
#endif	// _MSC_VER, NDEBUG
}



::BOOL WINAPI DllMain (::HINSTANCE hinst, ::DWORD reason, ::LPVOID /*reserved_ptr*/)
{
	switch (reason)
	{
	case	DLL_PROCESS_ATTACH:
		avstp_dll_load (hinst);
		break;

	case	DLL_PROCESS_DETACH:
		avstp_dll_unload (hinst);
		break;
	}

	return (TRUE);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
