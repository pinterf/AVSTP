/*****************************************************************************

        avstp.cpp
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

#include	"avstp/TaskDispatcher.h"
#include	"avstp/TaskDispPool.h"
#include	"avstp/ThreadPool.h"
#include	"avstp.h"

#include	<cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int __cdecl	avstp_get_interface_version ()
{
	return (avstp_INTERFACE_VERSION);
}



avstp_TaskDispatcher * __cdecl	avstp_create_dispatcher ()
{
	avstp::TaskDispatcher *	disp_ptr = 0;

	try
	{
		avstp::TaskDispPool &	pool = avstp::TaskDispPool::use_instance ();
		disp_ptr = pool._pool.take_obj ();
	}
	catch (...)
	{
		// Nothing
	}

	return (disp_ptr);
}



void __cdecl	avstp_destroy_dispatcher (avstp_TaskDispatcher *td_ptr)
{
	assert (td_ptr != 0);

	try
	{
		avstp::TaskDispPool &	pool = avstp::TaskDispPool::use_instance ();
		pool._pool.return_obj (*td_ptr);
	}
	catch (...)
	{
		// Nothing
	}
}



// Returns 0 in case of problem
int __cdecl	avstp_get_nbr_threads ()
{
	int				ret_val = 0;

	try
	{
		avstp::ThreadPool &	pool = avstp::ThreadPool::use_instance ();
		ret_val = pool.get_nbr_threads ();
	}
	catch (...)
	{
		// Nothing
	}

	return (ret_val);
}



int __cdecl	avstp_enqueue_task (avstp_TaskDispatcher *td_ptr, avstp_TaskPtr task_ptr, void *user_data_ptr)
{
	assert (td_ptr != 0);

	return (td_ptr->enqueue_task (task_ptr, user_data_ptr));
}



int __cdecl	avstp_wait_completion (avstp_TaskDispatcher *td_ptr)
{
	assert (td_ptr != 0);

	return (td_ptr->wait_completion ());
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
