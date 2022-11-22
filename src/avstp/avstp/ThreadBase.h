/*****************************************************************************

        ThreadBase.h
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (avstp_ThreadBase_HEADER_INCLUDED)
#define	avstp_ThreadBase_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define	WIN32_LEAN_AND_MEAN
#define	NOMINMAX
#define	NOGDI

#include	"conc/AtomicInt.h"

#include	<windows.h>



namespace avstp
{



class ThreadBase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						ThreadBase ();
	virtual			~ThreadBase ();

	int				run ();
	bool				wait_for_death (long timeout = -1);
	void				kill ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void	do_run () = 0;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static unsigned int __stdcall
						dispatch_and_run (void *thread_ptr);

	::HANDLE			_handle;
	unsigned int	_id;
	conc::AtomicInt <bool>
						_run_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ThreadBase (const ThreadBase &other);
	ThreadBase &	operator = (const ThreadBase &other);
	bool				operator == (const ThreadBase &other) const;
	bool				operator != (const ThreadBase &other) const;

};	// class ThreadBase



}	// namespace avstp



//#include	"avstp/ThreadBase.hpp"



#endif	// avstp_ThreadBase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
