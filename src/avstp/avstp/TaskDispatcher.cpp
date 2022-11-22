/*****************************************************************************

        TaskDispatcher.cpp
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

#include	"avstp/TaskDispatcher.h"
#include	"avstp/ThreadPool.h"

#include	<cassert>



namespace avstp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TaskDispatcher::TaskDispatcher (ThreadPool &pool)
:	_pool (pool)
,	_task_cnt (0)
{
	assert (&pool != 0);
}



TaskDispatcher::~TaskDispatcher ()
{
	// If _task_cnt is not 0, this means some tasks are still running and will
	// try to access the dispatcher counter later, causing exceptions or memory
	// corruption.
	assert (_task_cnt == 0);
}



int	TaskDispatcher::enqueue_task (avstp_TaskPtr task_ptr, void *user_data_ptr)
{
	assert (task_ptr != 0);

	int				ret_val = avstp_Err_OK;

	if (task_ptr == 0)
	{
		ret_val = avstp_Err_INVALID_ARG;
	}

	if (ret_val == avstp_Err_OK)
	{
		_pool.enqueue_task_no_signal (task_ptr, user_data_ptr, *this);
		_pool.signal_workers (1);
	}

	return (ret_val);
}



int	TaskDispatcher::wait_completion ()
{
	int				ret_val = avstp_Err_OK;

	_pool.wait_completion (_task_cnt);

	return (ret_val);
}



void	TaskDispatcher::inc ()
{
	++ _task_cnt;
}



void	TaskDispatcher::dec ()
{
	-- _task_cnt;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
