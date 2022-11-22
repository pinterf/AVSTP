/*****************************************************************************

        TaskDispPool.cpp
        Author: Laurent de Soras, 2012

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

#include	"avstp/TaskDispPool.h"
#include	"avstp/ThreadPool.h"

#include <mutex>

#include	<cassert>



namespace avstp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TaskDispPool::~TaskDispPool ()
{
	// Nothing
}



TaskDispPool &	TaskDispPool::use_instance ()
{
	// First check
	if (! _singleton_init_flag)
	{
		// Ensure serialization (guard constructor acquires mutex_new).
		static std::mutex mutex_new;
		std::lock_guard <std::mutex>  lock (mutex_new);

		// Double check.
		if (! _singleton_init_flag)
		{
			assert (! _singleton_init_flag && _singleton_aptr.get () == 0);
			_singleton_aptr = std::auto_ptr <TaskDispPool> (new TaskDispPool);
			_singleton_init_flag = true;
		}

		// guard destructor releases mutex_new.
	}

	return (*_singleton_aptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TaskDispPool::TaskDispPool ()
:	_pool ()
{
	_pool.set_factory (*this);
}



TaskDispatcher *	TaskDispPool::do_create ()
{
	TaskDispatcher *	disp_ptr = 0;

	try
	{
		ThreadPool &	pool = ThreadPool::use_instance ();
		disp_ptr = new TaskDispatcher (pool);
	}
	catch (...)
	{
		// Nothing
	}

	return (disp_ptr);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::auto_ptr <TaskDispPool>	TaskDispPool::_singleton_aptr;
volatile bool	TaskDispPool::_singleton_init_flag = false;



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
