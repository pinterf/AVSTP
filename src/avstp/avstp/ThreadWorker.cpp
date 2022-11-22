/*****************************************************************************

        ThreadWorker.cpp
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

#include	"avstp/ThreadMgr.h"
#include	"avstp/ThreadPool.h"
#include	"avstp/ThreadWorker.h"

#include	<cassert>



namespace avstp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ThreadWorker::ThreadWorker (ThreadPool &pool)
:	_pool (pool)
,	_exit_flag (false)
{
	assert (&pool != 0);
}



ThreadWorker::~ThreadWorker ()
{
	// Nothing
}



void	ThreadWorker::request_exit ()
{
	_exit_flag = true;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ThreadWorker::do_run ()
{
	ThreadMgr::set_thread_name ("avstp worker");

	while (! _exit_flag)
	{
		_pool.worker_wait ();

		if (! _exit_flag)
		{
			ThreadPool::TaskCell *	cell_ptr = 0;
			do
			{
				cell_ptr = _pool.dequeue_task ();
				if (cell_ptr != 0)
				{
					_pool.execute_task (cell_ptr->_val);
					_pool.return_task_cell (*cell_ptr);
				}
			}
			while (! _exit_flag && cell_ptr != 0);
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
