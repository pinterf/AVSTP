/*****************************************************************************

        TaskDispPool.h
        Author: Laurent de Soras, 2012

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (avstp_TaskDispPool_HEADER_INCLUDED)
#define	avstp_TaskDispPool_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"avstp/TaskDispatcher.h"
#include	"conc/ObjFactoryInterface.h"
#include	"conc/ObjPool.h"

#include	<memory>



namespace avstp
{



class TaskDispPool
:	public conc::ObjFactoryInterface <TaskDispatcher>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual			~TaskDispPool ();

	static TaskDispPool &
						use_instance ();

	conc::ObjPool <TaskDispatcher>
						_pool;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

						TaskDispPool ();

	// conc::ObjFactoryInterface <TaskDispatcher>
	virtual TaskDispatcher *
						do_create ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:


	static std::auto_ptr <TaskDispPool>
                  _singleton_aptr;
   static volatile bool
						_singleton_init_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						TaskDispPool (const TaskDispPool &other);
	TaskDispPool &	operator = (const TaskDispPool &other);
	bool				operator == (const TaskDispPool &other) const;
	bool				operator != (const TaskDispPool &other) const;

};	// class TaskDispPool



}	// namespace avstp



//#include	"avstp/TaskDispPool.hpp"



#endif	// avstp_TaskDispPool_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
