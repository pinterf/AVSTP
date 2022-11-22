/*****************************************************************************

        TaskData.cpp
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

#include	"avstp/TaskData.h"
#include	"avstp/TaskDispatcher.h"

#include	<cassert>



namespace avstp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TaskData::init (avstp_TaskPtr task_ptr, void *user_data_ptr, TaskDispatcher &dispatcher)
{
	assert (_task_ptr == 0);
	assert (task_ptr != 0);

	_task_ptr       = task_ptr;
	_user_data_ptr  = user_data_ptr;
	_dispatcher_ptr = &dispatcher;

	dispatcher.inc ();
}



TaskDispatcher &	TaskData::use_dispatcher () const
{
	assert (_task_ptr != 0);
	assert (_dispatcher_ptr != 0);

	return (*_dispatcher_ptr);
}



void	TaskData::execute ()
{
	assert (_task_ptr != 0);
	assert (_dispatcher_ptr != 0);

	(*_task_ptr) (_dispatcher_ptr, _user_data_ptr);

	_dispatcher_ptr->dec ();
	_task_ptr = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
