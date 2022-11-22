/*****************************************************************************

        ThreadBase.cpp
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

#include	"avstp/ThreadBase.h"

#include	<process.h>

#include	<algorithm>
#include	<stdexcept>

#include	<cassert>



namespace avstp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ThreadBase::ThreadBase ()
:	_handle (0)
,	_id (0)
,	_run_flag (false)
{
	_handle = reinterpret_cast < ::HANDLE> (::_beginthreadex (
		0,
		0,
		dispatch_and_run,
		reinterpret_cast < ::LPVOID> (this),
		CREATE_SUSPENDED,
		&_id
	));

	if (_handle == 0)
	{
		assert (false);
		throw std::runtime_error (
			"avstp::ThreadBase::ThreadBase(): "
			"Cannot create thread."
		);
	}
}



ThreadBase::~ThreadBase ()
{
	if (_run_flag)
	{
		kill ();
	}

	if (::CloseHandle (_handle) == 0)
	{
		const unsigned long	err_code = ::GetLastError ();
		assert (false);
	}
	_handle = 0;
	_id = 0;
}



int	ThreadBase::run ()
{
	assert (_handle != 0);

	int				ret_val = 0;

	if (::ResumeThread (_handle) == 0xFFFFFFFFUL)
	{
		ret_val = -1;
	}

	return (ret_val);
}



// timeout is measured in microseconds, and negative if no timeout required.
// Returns true if the thread is dead (no timeout).
bool	ThreadBase::wait_for_death (long timeout)
{
	assert (_handle != 0);
	assert (timeout < 1000L * 1000L*1000L);	// More than 1000 s is very likely an error.

	::DWORD			timeout_ms = INFINITE;
	if (timeout >= 0)
	{
		timeout_ms = std::max (::DWORD ((timeout + 500) / 1000), ::DWORD (1));
	}

	bool				dead_flag = false;
	const ::DWORD	result = ::WaitForSingleObject (_handle, timeout_ms);
	if (result == WAIT_FAILED)
	{
		const unsigned long	err_code = ::GetLastError ();
		assert (false);
	}
	else if (result == WAIT_TIMEOUT)
	{
		// Nothing
	}
	else
	{
		dead_flag = true;
	}

	return (dead_flag);
}



void	ThreadBase::kill ()
{
	const ::BOOL		kill_res = ::TerminateThread (_handle, -1);
	const ::DWORD		cur_thread_id = ::GetCurrentThreadId ();
	if (kill_res == 0)
	{
		const unsigned long	err_code = ::GetLastError ();
		assert (false);
	}
	
	_run_flag = false;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



unsigned int __stdcall	ThreadBase::dispatch_and_run (void *thread_ptr)
{
	ThreadBase *	real_thread_ptr =
		reinterpret_cast <ThreadBase *> (thread_ptr);

	real_thread_ptr->_run_flag = true;
	real_thread_ptr->do_run ();
	real_thread_ptr->_run_flag = false;

	return (0);
}



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
