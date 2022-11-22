/*****************************************************************************

        ThreadPool.cpp
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
#include	"avstp/ThreadMgr.h"
#include	"avstp/ThreadPool.h"

#include	<cassert>



namespace avstp
{



std::auto_ptr <ThreadPool>	ThreadPool::_singleton_aptr;
volatile bool   ThreadPool::_singleton_init_flag = false;
const long	ThreadPool::_security_timeout = 100L*1000L;	// 0.1 s



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ThreadPool::~ThreadPool ()
{
	for (int cnt = 0; cnt < int (_worker_arr.size ()); ++cnt)
	{
		WorkerSPtr &		worker_sptr = _worker_arr [cnt];
		if (worker_sptr.get () != 0)
		{
			ThreadWorker &		worker = *worker_sptr;
			worker.request_exit ();
		}
	}
	
	_sync_workers.signal (int (_worker_arr.size ()));

	for (int cnt = 0; cnt < int (_worker_arr.size ()); ++cnt)
	{
		WorkerSPtr &		worker_sptr = _worker_arr [cnt];
		if (worker_sptr.get () != 0)
		{
			ThreadWorker &		worker = *worker_sptr;
			worker.wait_for_death (100L*1000L);

			worker_sptr.reset ();
		}
	}
}



ThreadPool &	ThreadPool::use_instance (int nbr_threads)
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
			_singleton_aptr = std::auto_ptr <ThreadPool> (
				new ThreadPool (nbr_threads)
			);
			_singleton_init_flag = true;
		}

		// guard destructor releases mutex_new.
	}

	return (*_singleton_aptr);
}



// The call may be ignored.
// Returns the actual number of threads
int	ThreadPool::set_nbr_threads (int nbr_threads)
{
	assert (_nbr_threads >= 1);
	assert (nbr_threads >= 0);
	if (nbr_threads < 1)
	{
		nbr_threads = _nbr_threads_detected;
	}

	static std::mutex mutex_set;
	std::lock_guard <std::mutex>  lock (mutex_set);

	// Thread reduction: worker are not destructed, just asked to quit.
	// Actually they could be processing a task at the moment.
	// The array is not resized.
	if (nbr_threads < _nbr_threads)
	{
		for (int cnt = nbr_threads - 1; cnt < _nbr_threads - 1; ++cnt)
		{
			ThreadWorker &		worker = *(_worker_arr [cnt]);
			worker.request_exit ();
		}
		_sync_workers.signal (_nbr_threads - nbr_threads);

		_nbr_threads = nbr_threads;
	}

	// Condition: to allow the number of threads to grow, we must
	// never have reduced this number before.
	else if (   nbr_threads > _nbr_threads
	         && _nbr_threads == int (_worker_arr.size () + 1))
	{
		_worker_arr.resize (nbr_threads);
		for (int cnt = _nbr_threads - 1; cnt < nbr_threads - 1; ++cnt)
		{
			WorkerSPtr		worker_sptr (new ThreadWorker (*this));
			_worker_arr [cnt] = worker_sptr;
			worker_sptr->run ();
		}

		_nbr_threads = nbr_threads;
	}

	return (_nbr_threads);
}



int	ThreadPool::get_nbr_threads () const
{
	return (_nbr_threads);
}



void	ThreadPool::enqueue_task_no_signal (avstp_TaskPtr task_ptr, void *user_data_ptr, TaskDispatcher &dispatcher)
{
	assert (task_ptr != 0);

	TaskCell *		cell_ptr = _task_pool.take_cell (true);
	if (cell_ptr == 0)
	{
		assert (false);
	}
	else
	{
		cell_ptr->_val.init (task_ptr, user_data_ptr, dispatcher);
		_task_queue.enqueue (*cell_ptr);
	}
}



void	ThreadPool::signal_workers (int nbr_tasks)
{
	assert (nbr_tasks > 0);

	_sync_workers.signal (nbr_tasks);
}



void	ThreadPool::wait_completion (conc::AtomicInt <int> &counter)
{
	while (counter > 0)
	{
		TaskCell *		task_ptr = _task_queue.dequeue ();
		if (task_ptr == 0)
		{
			if (is_mt ())
			{
				if (counter > 0)
				{
					_sync_pool.wait (_security_timeout);
				}
			}
			else
			{
				assert (counter <= 0);
			}
		}
		else
		{
			execute_task (task_ptr->_val);
			return_task_cell (*task_ptr);
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// nbr_threads == 0: auto-detect
ThreadPool::ThreadPool (int nbr_threads)
:	_task_pool ()
,	_task_queue ()
,	_sync_pool ()
,	_sync_workers ()
,	_worker_arr ()
,	_nbr_threads (1)
,	_nbr_threads_detected (ThreadMgr::count_nbr_logical_proc ())
{
	assert (nbr_threads >= 0);

	_task_pool.expand_to (16384);

	if (nbr_threads <= 0)
	{
		nbr_threads = _nbr_threads_detected;
	}

	set_nbr_threads (nbr_threads);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	ThreadPool::is_mt () const
{
	return (_nbr_threads > 1);
}



void	ThreadPool::worker_wait ()
{
	_sync_workers.wait (_security_timeout);
}



ThreadPool::TaskCell *	ThreadPool::dequeue_task ()
{
	return (_task_queue.dequeue ());
}



void	ThreadPool::return_task_cell (TaskCell &cell)
{
	_task_pool.return_cell (cell);

	if (is_mt ())
	{
		_sync_pool.signal ();
	}
}



void	ThreadPool::execute_task (TaskData &task)
{
	task.execute ();
}



}	// namespace avstp



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
