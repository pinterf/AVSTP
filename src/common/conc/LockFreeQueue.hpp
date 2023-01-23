/*****************************************************************************

        LockFreeQueue.hpp
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (conc_LockFreeQueue_CODEHEADER_INCLUDED)
#define	conc_LockFreeQueue_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <mutex>

// Bug reproduction: undefine FIX + use debug build of avstp
// Assertions will soon come: e.g.
// TaskData.cpp line 45: _task_ptr == 0
// TaskData.cpp line 69: _task_ptr != 0
// TaskDispather task_count == 0
// cell_pool.hpp like 79: _m_ptr->_nbr_avail_cells == _nbr_total_cells
// LockFreeQueue.hpp: expression false (loop_cnt >= max_loop case)
// Such asserts should not never occur, they mean memory corruption, double free, whatever
//#define FIX

#ifdef FIX
// final solution: enqueue mutex + dequeue mutex outside the loop
#define FIX_USE_ENQUEUE_MUTEX
#define FIX_USE_DEQUEUE_MUTEX_OUTSIDE
//#define FIX_USE_DEQUEUE_MUTEX_INSIDE
#endif

//#define DEBUF_PF_OUTPUTDEBUG

// Another try to find out when the bug occurs:
// tamper with internal timing conditions in the hope of a more frequent bug occurance
//#define DEBUG_PF_SMALL_DELAY

#ifdef DEBUG_PF_SMALL_DELAY
#include <chrono>
#include <thread>
#endif

#ifdef DEBUF_PF_OUTPUTDEBUG
#include "Processthreadsapi.h"
#endif



namespace conc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
LockFreeQueue <T>::LockFreeQueue ()
:	_m_ptr ()
{
	_m_ptr->_dummy._next_ptr = nullptr;
	_m_ptr->_head.set (&_m_ptr->_dummy, 0);
	_m_ptr->_tail.set (&_m_ptr->_dummy, 0);
}

/*
Theory:
ENQUEUE(in value: data_type)
E1:  node := new node()
E2:  node.value := value
E3:  node.next := null
E4:  loop
E5:    tail := Tail
E6:    next := tail.next
E7:    if tail = Tail then
E8:      if next = null then
E9:        if CAS(tail.next (target), next (old_to_compare), node (change to this)) then
E10:         break
E11:       endif
E12:     else
E13:       CAS(Tail (target), tail (old_to_compare), next (change to this))
E14:     endif
E15:   endif
E16: endloop
E17: CAS(Tail (target), tail (old_to_compare), node (change to this))
*/
template <class T>
void	LockFreeQueue <T>::enqueue(CellType& cell)
{
	#ifdef FIX_USE_ENQUEUE_MUTEX
	// enqueue mutex alone does not solve freezing
	::std::lock_guard<::std::mutex> lock(mx_lockfree_enqueue);
	#endif

#ifdef DEBUF_PF_OUTPUTDEBUG
	const ::DWORD		cur_thread_id = ::GetCurrentThreadId();
	char debugbuf[200];
	sprintf_s(debugbuf, "entered into enqueue %d this=%p\n", (int)cur_thread_id, (void*)this);
	OutputDebugStringA(debugbuf);
#endif

	cell._next_ptr = nullptr;  // set the cell next pointer to NULL

	CellType *     tail_ptr = nullptr;
	intptr_t       icount   = 0;

	bool           cont_flag = true;

#ifdef FIX
	do	// try until enqueue is done
	{
		// read the tail modification count
		// read the tail cell (ptr)
		_m_ptr->_tail.get(tail_ptr, icount);

		CellType* next_ptr = tail_ptr->_next_ptr;
		const intptr_t icount_tst = _m_ptr->_tail.get_val();
		if (icount == icount_tst) { // if tail = Tail : ensures that next is still a valid pointer
			if (next_ptr == nullptr) {

				// try to link the cell to the tail cell
				void* old_ptr = tail_ptr->_next_ptr.cas(&cell, nullptr);

				if (old_ptr == nullptr)
				{
					cont_flag = false;	// enqueue is done, exit the loop
				}
			}
			else
			{
				// tail was not pointing to the last cell, try to set tail to the next cell
				_m_ptr->_tail.cas2(tail_ptr->_next_ptr, icount + 1, tail_ptr, icount);
			}
		}
	} while (cont_flag);

	_m_ptr->_tail.cas2(&cell, icount + 1, tail_ptr, icount);
#else
	do	// try until enqueue is done
	{
		// read the tail modification count
		// read the tail cell
		_m_ptr->_tail.get (tail_ptr, icount);

		// try to link the cell to the tail cell
		void *         old_ptr = tail_ptr->_next_ptr.cas (&cell, nullptr);

		if (old_ptr == nullptr)
		{
			cont_flag = false;	// enqueue is done, exit the loop
		}
		else
		{
			// tail was not pointing to the last cell, try to set tail to the next cell
			_m_ptr->_tail.cas2 (tail_ptr->_next_ptr, icount + 1, tail_ptr, icount);
		}
	}
	while (cont_flag);

	_m_ptr->_tail.cas2 (&cell, icount + 1, tail_ptr, icount);
#endif
#ifdef DEBUF_PF_OUTPUTDEBUG
	sprintf_s(debugbuf, "exited from enqueue %d this=%p\n", (int)cur_thread_id, (void*)this);
	OutputDebugStringA(debugbuf);
#endif
}

#ifdef FIX
// static in class
template <class T>
::std::mutex LockFreeQueue <T>::mx_lockfree_dequeue;
template <class T>
::std::mutex LockFreeQueue <T>::mx_lockfree_enqueue;
#endif

// Returns 0 if the queue is empty.
template <class T>
typename LockFreeQueue <T>::CellType *	LockFreeQueue <T>::dequeue ()
{
#ifdef FIX
	CellType* out_ptr = nullptr;
#endif

#ifdef DEBUF_PF_OUTPUTDEBUG
	const ::DWORD		cur_thread_id = ::GetCurrentThreadId();
	char debugbuf[200];
	sprintf_s(debugbuf, "entered into dequeue %d this=%p\n", (int)cur_thread_id, (void *)this );
	OutputDebugStringA(debugbuf);
#endif

	constexpr int  max_loop = 100;
	int            loop_cnt = 0;
	intptr_t       ocount   = 0;
	intptr_t       icount   = 0;
	CellType *     head_ptr = nullptr;
	CellType *     tail_ptr = nullptr;
	CellType *     next_ptr = nullptr;


	/* Theoretical pseudo code
	out pvalue : data_type
	* loop
	*   head_ptr = _head
	*   tail = Tail
	*   next_ptr = head_ptr._next_ptr
	*   if head_ptr == _head then // (ocount == ocount_tst) means nobody changed next_ptr meanwhile
	*     if head_ptr == tail then // head_ptr == _m_ptr->_tail.get_ptr ()
	*       if next_ptr == null then
	*         return false
	*       endif
	*       CAS(Tail (target), tail (old-to-compare), next_ptr (change-to-this)) // ??? _m_ptr->_tail.cas2 (next_ptr, icount + 1, head_ptr, icount); // // tail is pointing to head in a non empty queue, try to set tail to the next cell
	*     else
	*       pvalue = next_ptr.value
	*       if CAS(_head (target), head_ptr (old-to-compare), next_ptr (change-to-this)) then // _m_ptr->_head.cas2 (next_ptr, ocount + 1, head_ptr, ocount)
	*         break
	*       endif
	*     endif
	*   endif
	* endloop
	* return True
	*
	*/

	{
#ifdef FIX_USE_DEQUEUE_MUTEX_OUTSIDE
		// Should we put the mutex before the loop or inside the while()?
		// dequeue mutex alone does not solve freezing
		::std::lock_guard<::std::mutex> lock(mx_lockfree_dequeue);
#endif

#ifdef DEBUG_PF_SMALL_DELAY
		// debug: artificial small delay, because the freeze is very-very timing sensitive
		// Later note: it is not needed even not in debug build, asserts will some soon
		// when mvtools mt=true is enabled
		::std::this_thread::sleep_for(::std::chrono::microseconds(2));
#endif

		do	// try until dequeue is done
		{
#ifdef FIX_USE_DEQUEUE_MUTEX_INSIDE
			// Should we put the mutex before the loop or inside the while()?
			::std::lock_guard<::std::mutex> lock(mx_lockfree_dequeue);
#endif
			// read the head modification count
			// read the head cell
			_m_ptr->_head.get (head_ptr, ocount);
			_m_ptr->_tail.get (tail_ptr, icount);
			//icount   = _m_ptr->_tail.get_val ();   // read the tail modification count
			next_ptr = head_ptr->_next_ptr;        // read the next cell

			const intptr_t ocount_tst = _m_ptr->_head.get_val ();
			if (ocount == ocount_tst)  // ensures that next is a valid pointer to avoid failure when reading next value
			{

#ifdef FIX
				if (head_ptr == tail_ptr)   // is queue empty or tail falling behind ?
#else
				if (head_ptr == /*tail_ptr */_m_ptr->_tail.get_ptr ())   // is queue empty or tail falling behind ?
#endif
				{
					if (next_ptr == nullptr)   // is queue empty ?
					{
#ifdef DEBUG_PF_OUTPUTDEBUG
						sprintf_s(debugbuf, "Exited from dequeue (empty) %d this=%p\n", (int)cur_thread_id, (void*)this);
						OutputDebugStringA(debugbuf);
#endif
						return nullptr; // queue is empty: return NULL
					}
					// tail is pointing to head in a non empty queue, try to set tail to the next cell
					// newptr newval; compptr compval. If does not match with the sample during the exchange cas2 returns false
					// Here the return value is ignored
#ifdef FIX
					_m_ptr->_tail.cas2 (next_ptr, icount + 1, tail_ptr, icount); // tail_ptr is equal to head_ptr
#else
					_m_ptr->_tail.cas2 (next_ptr, icount + 1, head_ptr, icount);
#endif
				}
				else if (next_ptr != nullptr) // if we are not competing on the dummy next
				{
					// try to set tail to the next cell
					// newptr newval; compptr compval. If does not match with the sample during the exchange cas2 returns false
#ifdef FIX
					out_ptr = head_ptr; // save it, it maybe overwritten later. difference from Theory white paper
#endif
					if (_m_ptr->_head.cas2 (next_ptr, ocount + 1, head_ptr, ocount))
					{
						break;   // dequeue done, exit the loop
					}
				}
			}

			++ loop_cnt;
			if (loop_cnt >= max_loop)
			{
				// This could indicate that the queue is:
				// - corrupted
				// - or in heavy contention
				assert (false);
				// PF test notes: when only dequeue mutex is applied and no enqueue mutex, then this part was reached
				return nullptr;
			}
		}
		while (true);
	} // for ending the scope of the mutex (if any)

	// PF: non thread safe because of this extra?
#ifdef FIX
	// We could even change back the usage of head_ptr instead of out_ptr
	if (out_ptr == &_m_ptr->_dummy)   // check whether we’re trying to pop the dummy cell
	{
		enqueue(*out_ptr);    // this is the dummy cell: push it back to the fifo
		//head_ptr = dequeue ();  // and pop a cell again
		out_ptr = dequeue();  // and pop a cell again
	}
	return out_ptr;
#else
	if (head_ptr == &_m_ptr->_dummy)   // check wether we’re trying to pop the dummy cell
	{
		enqueue (*head_ptr);    // this is the dummy cell: push it back to the fifo
		// ugyanaz: enqueue(_m_ptr->_dummy /* *head_ptr*/);    // this is the dummy cell: push it back to the fifo
		head_ptr = dequeue ();  // and pop a cell again
	}
#ifdef DEBUG_PF_OUTPUTDEBUG
	sprintf_s(debugbuf, "Exited from dequeue %d this=%p\n", (int)cur_thread_id, (void*)this);
	OutputDebugStringA(debugbuf);
#endif

	return head_ptr;
#endif

}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace conc



#endif	// conc_LockFreeQueue_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
