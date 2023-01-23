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



template <class T>
void	LockFreeQueue <T>::enqueue (CellType &cell)
{
	::std::lock_guard<::std::mutex> lock(mx_lockfree_enqueue);

	cell._next_ptr = nullptr;  // set the cell next pointer to NULL

	CellType *     tail_ptr = nullptr;
	intptr_t       icount   = 0;

	bool           cont_flag = true;
	do	// try until enqueue is done
	{
		// read the tail modification count
		// read the tail cell (ptr)
		_m_ptr->_tail.get (tail_ptr, icount);

		CellType* next_ptr = tail_ptr->_next_ptr;
		const intptr_t icount_tst = _m_ptr->_tail.get_val();
		if (icount == icount_tst) { // if tail = Tail : ensures that next is still a valid pointer
			if (next_ptr == nullptr) {
				// try to link the cell to the tail cell
				void *         old_ptr = tail_ptr->_next_ptr.cas (&cell, nullptr);

				if (old_ptr == nullptr)
				{
					cont_flag = false;	// enqueue is done, exit the loop
				}
			}
			else
			{
				// tail was not pointing to the last cell, try to set tail to the next cell
				_m_ptr->_tail.cas2 (tail_ptr->_next_ptr, icount + 1, tail_ptr, icount);
			}
		}
	}
	while (cont_flag);

	_m_ptr->_tail.cas2 (&cell, icount + 1, tail_ptr, icount);
}

// static in class
template <class T>
::std::mutex LockFreeQueue <T>::mx_lockfree_dequeue;
template <class T>
::std::mutex LockFreeQueue <T>::mx_lockfree_enqueue;

// Returns 0 if the queue is empty.
template <class T>
typename LockFreeQueue <T>::CellType *	LockFreeQueue <T>::dequeue ()
{
	CellType* out_ptr = nullptr;

	constexpr int  max_loop = 100;
	int            loop_cnt = 0;
	intptr_t       ocount   = 0;
	intptr_t       icount   = 0;
	CellType *     head_ptr = nullptr;
	CellType *     tail_ptr = nullptr;
	CellType *     next_ptr = nullptr;

	{ // mutex scope block begin
		::std::lock_guard<::std::mutex> lock(mx_lockfree_dequeue);

		do	// try until dequeue is done
		{
			// read the head modification count
			// read the head cell
			_m_ptr->_head.get (head_ptr, ocount);
			_m_ptr->_tail.get (tail_ptr, icount); // read the tail modification count
			next_ptr = head_ptr->_next_ptr;        // read the next cell

			const intptr_t ocount_tst = _m_ptr->_head.get_val ();
			if (ocount == ocount_tst)  // ensures that next is a valid pointer to avoid failure when reading next value
			{
				if (head_ptr == tail_ptr)   // is queue empty or tail falling behind ?
				{
					if (next_ptr == nullptr)   // is queue empty ?
					{
						return nullptr; // queue is empty: return NULL
					}
					// tail is pointing to head in a non empty queue, try to set tail to the next cell
					// newptr newval; compptr compval. If does not match with the sample during the exchange cas2 returns false
					// Here the return value is ignored
					_m_ptr->_tail.cas2 (next_ptr, icount + 1, tail_ptr, icount); // tail_ptr is equal to head_ptr
				}
				else if (next_ptr != nullptr) // if we are not competing on the dummy next
				{
					// try to set tail to the next cell
					// newptr newval; compptr compval. If does not match with the sample during the exchange cas2 returns false
					out_ptr = head_ptr;
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
				return nullptr;
			}
		}
		while (true);
	} // for ending the scope of the mutex

	if (out_ptr == &_m_ptr->_dummy)   // check whether we’re trying to pop the dummy cell
	{
		enqueue(*out_ptr);    // this is the dummy cell: push it back to the fifo
		out_ptr = dequeue();  // and pop a cell again
	}

	return out_ptr;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace conc



#endif	// conc_LockFreeQueue_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
