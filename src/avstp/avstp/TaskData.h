/*****************************************************************************

        TaskData.h
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (avstp_TaskData_HEADER_INCLUDED)
#define	avstp_TaskData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"avstp.h"



namespace avstp
{



class TaskDispatcher;

class TaskData
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						TaskData ();
						TaskData (const TaskData &other);
	virtual			~TaskData () {}

	TaskData &		operator = (const TaskData &other);

	void				init (avstp_TaskPtr task_ptr, void *user_data_ptr, TaskDispatcher &dispatcher);
	TaskDispatcher &
						use_dispatcher () const;
	void				execute ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	avstp_TaskPtr	_task_ptr;
	void *			_user_data_ptr;
	TaskDispatcher *
						_dispatcher_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool				operator == (const TaskData &other) const;
	bool				operator != (const TaskData &other) const;

};	// class TaskData



}	// namespace avstp



//#include	"avstp/TaskData.hpp"



#endif	// avstp_TaskData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
