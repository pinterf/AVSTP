/*****************************************************************************

        avs_filter_init.cpp
        Author: Laurent de Soras, 2012

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

#define NOGDI
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "avstp/ThreadPool.h"
#include "avisynth.h"

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



::AVSValue __cdecl	avstp_set_threads (::AVSValue args, void * /*user_data_ptr*/, ::IScriptEnvironment *env_ptr)
{
	int				actual_nbr_threads = 0;

	const int		nbr_threads = args [1].AsInt ();
	if (nbr_threads < 0)
	{
		env_ptr->ThrowError (
			"avstp_set_threads: Number of threads must be greater or equal to 0."
		);
	}

	try
	{
		avstp::ThreadPool &	pool = avstp::ThreadPool::use_instance (nbr_threads);
		actual_nbr_threads = pool.set_nbr_threads (nbr_threads);
	}
	catch (std::exception &e)
	{
		env_ptr->ThrowError (
			"avstp_set_threads: Exception: %s",
			e.what ()
		);
	}
	catch (...)
	{
		env_ptr->ThrowError (
			"avstp_set_threads: Exception."
		);
	}

	return (args [0].Defined () ? args [0] : actual_nbr_threads);
}



extern "C" __declspec (dllexport) const char * __stdcall	AvisynthPluginInit2 (::IScriptEnvironment *env_ptr)
{
	env_ptr->AddFunction (
		"avstp_set_threads",
		".i",
		avstp_set_threads,
		0
	);

	// A freeform name of the plugin.
	return ("AVSTP - Avisynth thread pool");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
