
#include	"avstp.h"
#include	"conc/ObjFactoryInterface.h"
#include	"conc/ObjPool.h"

#include	<vector>

#include	<cassert>
#include	<cmath>
#include	<cstdio>
#include	<cstring>



struct TempData
{
	int			_nothing [16384];
};

struct TempDataFactory
:	public conc::ObjFactoryInterface <TempData>
{
protected:
	virtual TempData *	do_create () { return new TempData; }
};



struct AllData;

struct JobInfo
{
	AllData *	_data_ptr;
	int			_pos;
	int			_len;
};

struct AllData
{
	int				_nbr_elt;
	std::vector <int>
						_data_arr;
	std::vector <JobInfo>
						_job_arr;
	conc::ObjPool <TempData>
						_temp_pool;
	TempDataFactory
						_temp_fact;
};



static void		proc_fnc (avstp_TaskDispatcher *td_ptr, void *user_data_ptr)
{
	JobInfo &		info = *reinterpret_cast <JobInfo *> (user_data_ptr);
	AllData &		all_data = *info._data_ptr;

	TempData *		tmp_ptr = all_data._temp_pool.take_obj ();
	if (tmp_ptr == 0)
	{
		throw "Huhuhu";
	}

	int				len = info._len;
	while (len > 1)
	{
		const int		hlen = (len + 1) / 2;
		assert (len - hlen > 0);
		const int		pos = info._pos + hlen;

		all_data._job_arr [pos]._data_ptr = info._data_ptr;
		all_data._job_arr [pos]._pos = pos;
		all_data._job_arr [pos]._len = len - hlen;

		::avstp_enqueue_task (td_ptr, proc_fnc, &all_data._job_arr [pos]);

		len = hlen;
	}

	// The actual work. Dummy calculations
	double		x = info._pos;
	for (int k = 0; k < 10*1000; ++k)
	{
		x = (x + 2) * x + 1;
		x = sqrt (x) - 1;
	}
	all_data._data_arr [info._pos] = int (x + 0.5);	// result == info._pos

	all_data._temp_pool.return_obj (*tmp_ptr);
}



int	main (int argc, char *argv [])
{
	::avstp_TaskDispatcher *	td_ptr = ::avstp_create_dispatcher ();

	AllData			all_data;
	all_data._nbr_elt = 123;
	all_data._data_arr.resize (all_data._nbr_elt);
	all_data._job_arr.resize (all_data._nbr_elt);	// Should be enough
	all_data._temp_pool.set_factory (all_data._temp_fact);

	for (int tst_count = 0; tst_count < 2000; ++tst_count)
	{
		if ((tst_count % 100) == 0)
		{
			printf ("pass %d...\n", tst_count);
		}

		memset (
			&all_data._data_arr [0],
			-1,
			sizeof (all_data._data_arr [0]) * all_data._data_arr.size ()
		);
		all_data._job_arr [0]._data_ptr = &all_data;
		all_data._job_arr [0]._pos = 0;
		all_data._job_arr [0]._len = all_data._nbr_elt;

		::avstp_enqueue_task (td_ptr, proc_fnc, &all_data._job_arr [0]);
		::avstp_wait_completion (td_ptr);
	}

	for (int pos = 0; pos < all_data._nbr_elt; ++pos)
	{
		printf ("%d ", all_data._data_arr [pos]);
	}
	printf ("\n");

	::avstp_destroy_dispatcher (td_ptr);

	return (0);
}

