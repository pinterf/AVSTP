/*****************************************************************************

        ThreadWorker.h
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (avstp_ThreadWorker_HEADER_INCLUDED)
#define	avstp_ThreadWorker_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"avstp/ThreadBase.h"



namespace avstp
{



class ThreadPool;

class ThreadWorker
:	public ThreadBase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						ThreadWorker (ThreadPool &pool);
	virtual			~ThreadWorker () = default;

	void				request_exit ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// avstp::ThreadBase
	virtual void	do_run ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ThreadPool &	_pool;
	volatile bool	_exit_flag;		// Indicates exit is requested



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ThreadWorker ()                               = delete;
						ThreadWorker (const ThreadWorker &other)      = delete;
						ThreadWorker (ThreadWorker &&other)           = delete;
	ThreadWorker &	operator = (const ThreadWorker &other)        = delete;
	ThreadWorker &	operator = (ThreadWorker &&other)             = delete;
	bool				operator == (const ThreadWorker &other) const = delete;
	bool				operator != (const ThreadWorker &other) const = delete;

};	// class ThreadWorker



}	// namespace avstp



//#include	"avstp/ThreadWorker.hpp"



#endif	// avstp_ThreadWorker_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
