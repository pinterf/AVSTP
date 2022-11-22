/*****************************************************************************

        ThreadPool.h
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (avstp_ThreadPool_HEADER_INCLUDED)
#define	avstp_ThreadPool_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"avstp/TaskData.h"
#include	"avstp/ThreadSync.h"
#include	"avstp/ThreadWorker.h"
#include	"conc/CellPool.h"
#include	"conc/LockFreeCell.h"
#include	"conc/LockFreeQueue.h"
#include	"avstp.h"

#include	<memory>
#include	<vector>



namespace avstp
{



class TaskDispatcher;

class ThreadPool
{

	friend class ThreadWorker;

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual			~ThreadPool ();

	static ThreadPool &
						use_instance (int nbr_threads = 0);

	int				set_nbr_threads (int nbr_threads);
	int				get_nbr_threads () const;

	void				enqueue_task_no_signal (avstp_TaskPtr task_ptr, void *user_data_ptr, TaskDispatcher &dispatcher);
	void				signal_workers (int nbr_tasks);
	void				wait_completion (conc::AtomicInt <int> &counter);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	explicit			ThreadPool (int nbr_threads);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	conc::LockFreeCell <TaskData>	TaskCell;
	typedef	conc::CellPool <TaskData>	TaskPool;
	typedef	conc::LockFreeQueue <TaskData>	TaskQueue;

	typedef	std::shared_ptr <ThreadWorker>	WorkerSPtr;
	typedef	std::vector <WorkerSPtr>	WorkerArray;

	bool				is_mt () const;

	// Functions reserved to worker threads
	void				worker_wait ();
	TaskCell *		dequeue_task ();
	void				return_task_cell (TaskCell &cell);
	void				execute_task (TaskData &task);

	TaskPool			_task_pool;
	TaskQueue		_task_queue;

	ThreadSync		_sync_pool;
	ThreadSync		_sync_workers;

	WorkerArray		_worker_arr;

	volatile int	_nbr_threads;	// Provided or detected.
	int				_nbr_threads_detected;

	static std::auto_ptr <ThreadPool>
                  _singleton_aptr;
   static volatile bool
						_singleton_init_flag;

	static const long					// Microseconds
						_security_timeout;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ThreadPool ()                               = delete;
						ThreadPool (const ThreadPool &other)        = delete;
						ThreadPool (ThreadPool &&other)             = delete;
	ThreadPool &	operator = (const ThreadPool &other)        = delete;
	ThreadPool &	operator = (ThreadPool &&other)             = delete;
	bool				operator == (const ThreadPool &other) const = delete;
	bool				operator != (const ThreadPool &other) const = delete;

};	// class ThreadPool



}	// namespace avstp



//#include	"avstp/ThreadPool.hpp"



#endif	// avstp_ThreadPool_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
