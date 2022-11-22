/*****************************************************************************

        TaskDispatcher.h
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (avstp_TaskDispatcher_HEADER_INCLUDED)
#define	avstp_TaskDispatcher_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"conc/AtomicInt.h"
#include	"avstp.h"



namespace avstp
{



class ThreadPool;

class TaskDispatcher
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						TaskDispatcher (ThreadPool &pool);
	virtual			~TaskDispatcher ();

	int				enqueue_task (avstp_TaskPtr task_ptr, void *user_data_ptr);
	int				wait_completion ();

	// Called by TaskData
	void				inc ();
	void				dec ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ThreadPool &	_pool;
	conc::AtomicInt <int>
						_task_cnt;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						TaskDispatcher ();
						TaskDispatcher (const TaskDispatcher &other);
	TaskDispatcher &
						operator = (const TaskDispatcher &other);
	bool				operator == (const TaskDispatcher &other) const;
	bool				operator != (const TaskDispatcher &other) const;

};	// class TaskDispatcher



}	// namespace avstp



//#include	"avstp/TaskDispatcher.hpp"



#endif	// avstp_TaskDispatcher_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
