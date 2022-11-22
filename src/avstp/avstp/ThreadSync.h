/*****************************************************************************

        ThreadSync.h
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (avstp_ThreadSync_HEADER_INCLUDED)
#define	avstp_ThreadSync_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define	NOGDI
#define	NOMINMAX
#define	WIN32_LEAN_AND_MEAN
#include	<windows.h>



namespace avstp
{



class ThreadSync
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						ThreadSync ();
	virtual			~ThreadSync ();

	void				signal (int cnt = 1);
	bool				wait (long timeout = -1);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	::HANDLE			_sema_hnd;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ThreadSync (const ThreadSync &other)        = delete;
						ThreadSync (ThreadSync &&other)             = delete;
	ThreadSync &	operator = (const ThreadSync &other)        = delete;
	ThreadSync &	operator = (ThreadSync &&other)             = delete;
	bool				operator == (const ThreadSync &other) const = delete;
	bool				operator != (const ThreadSync &other) const = delete;

};	// class ThreadSync



}	// namespace avstp



//#include	"avstp/ThreadSync.hpp"



#endif	// avstp_ThreadSync_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
