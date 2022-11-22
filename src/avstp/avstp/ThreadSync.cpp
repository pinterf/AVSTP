/*****************************************************************************

        ThreadSync.cpp
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

#include	"avstp/ThreadSync.h"

#include	<algorithm>
#include	<stdexcept>

#include	<cassert>



namespace avstp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ThreadSync::ThreadSync ()
:	_sema_hnd (0)
{
	_sema_hnd = ::CreateSemaphoreW (0, 0, 0x7FFFFFFF, 0);
	if (_sema_hnd == 0)
	{
		const unsigned long	err_code = ::GetLastError ();
		assert (false);
		throw std::runtime_error (
			"avstp::ThreadSync::ThreadSync(): Cannot create semaphore."
		);
	}
}



ThreadSync::~ThreadSync ()
{
	assert (_sema_hnd != 0);
	const ::BOOL	result = ::CloseHandle (_sema_hnd);
	if (result == 0)
	{
		const unsigned long	err_code = ::GetLastError ();
		assert (false);
	}
	_sema_hnd = 0;
}



void	ThreadSync::signal (int cnt)
{
	assert (cnt >= 0);

	if (cnt > 0)
	{
		::LONG			prev_cnt;
		const ::BOOL	result = ::ReleaseSemaphore (_sema_hnd, cnt, &prev_cnt);
		if (result == 0)
		{
			const unsigned long	err_code = ::GetLastError ();
			assert (false);
			throw std::runtime_error ("avstp::ThreadSync::signal(): failed.");
		}
	}
}



// timeout is measured in microseconds, a negative value means
// that no timeout is required.
bool	ThreadSync::wait (long timeout)
{
	::DWORD			timeout_ms = INFINITE;
	if (timeout >= 0)
	{
		timeout_ms = std::max (::DWORD ((timeout + 500) / 1000), ::DWORD (1));
	}

	const ::DWORD	result =
		::WaitForSingleObjectEx (_sema_hnd, timeout_ms, TRUE);

	if (   result == WAIT_OBJECT_0
	    || result == WAIT_TIMEOUT)
	{
		// nothing
	}
	else if (result == WAIT_FAILED)
	{
		const unsigned long	err_code = ::GetLastError ();
		assert (false);
		throw std::runtime_error ("avstp::ThreadSync::wait(): wait failed.");
	}
	else
	{
		assert (false);
		throw std::runtime_error ("avstp::ThreadSync::wait(): unexpected error.");
	}

	return (result == WAIT_TIMEOUT);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
