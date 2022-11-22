
#define	NOMINMAX
#define	WIN32_LEAN_AND_MEAN
#include	"Windows.h"

#include	"avisynth.h"
#include	"avstp.h"
#include	"MTSlicer.h"



class AvstpAverage
:	public ::GenericVideoFilter
{
public:

	explicit			AvstpAverage (::IScriptEnvironment *env_ptr, ::PClip src1_sptr, ::PClip src2_sptr);

	// ::GenericVideoFilter
	virtual ::PVideoFrame __stdcall
						GetFrame (int n, ::IScriptEnvironment *env_ptr);

private:

	class TaskDataGlobal
	{
	public:
		AvstpAverage *	_this_ptr;
		::BYTE *			_dst_ptr;
		const ::BYTE *	_sr1_ptr;
		const ::BYTE *	_sr2_ptr;
		int				_stride_dst;
		int				_stride_sr1;
		int				_stride_sr2;
		int				_w;
	};

	typedef	MTSlicer <AvstpAverage, TaskDataGlobal>	Slicer;

	class PlaneProc
	{
	public:
		TaskDataGlobal	_tdg;
		Slicer			_slicer;
	};

	void				process_subplane (Slicer::TaskData &td);
	bool				is_same_clip_format (const ::PClip &test_clip_sptr) const;

	::PClip			_src1_sptr;
	::PClip			_src2_sptr;
	const int		_nbr_planes;
};



AvstpAverage::AvstpAverage (::IScriptEnvironment *env_ptr, ::PClip src1_sptr, ::PClip src2_sptr)
:	::GenericVideoFilter (src1_sptr)
,	_src1_sptr (src1_sptr)
,	_src2_sptr (src2_sptr)
,	_nbr_planes (((vi.pixel_type & ::VideoInfo::CS_INTERLEAVED) != 0) ? 1 : 3)
{
	if (! is_same_clip_format (src2_sptr))
	{
		env_ptr->ThrowError ("avstp_average: input clips must have the same format.");
	}
}



::PVideoFrame __stdcall	AvstpAverage::GetFrame (int n, ::IScriptEnvironment *env_ptr)
{
	::PVideoFrame	fsr1_sptr = _src1_sptr->GetFrame (n, env_ptr);
	::PVideoFrame	fsr2_sptr = _src2_sptr->GetFrame (n, env_ptr);
	::PVideoFrame	fdst_sptr = env_ptr->NewVideoFrame (vi);

	// Sets one slicer per plane, because planes may have different
	// heights (YV12). We could have done it differently and handle
	// the chroma subsampling from the plane processing function,
	// but it would have been less trivial to deal with.
	PlaneProc		pproc_arr [3];
	for (int p = 0; p < _nbr_planes; ++p)
	{
		static const int	plane_id_arr [3] = { PLANAR_Y, PLANAR_U, PLANAR_V };
		const int		plane_id = plane_id_arr [p];
		PlaneProc &		pproc    = pproc_arr [p];

		// Collects the source and destination plane information
		pproc._tdg._this_ptr   = this;
		pproc._tdg._dst_ptr    = fdst_sptr->GetWritePtr (plane_id);
		pproc._tdg._sr1_ptr    = fsr1_sptr->GetReadPtr (plane_id);
		pproc._tdg._sr2_ptr    = fsr2_sptr->GetReadPtr (plane_id);
		pproc._tdg._stride_dst = fdst_sptr->GetPitch (plane_id);
		pproc._tdg._stride_sr1 = fsr1_sptr->GetPitch (plane_id);
		pproc._tdg._stride_sr2 = fsr2_sptr->GetPitch (plane_id);
		pproc._tdg._w          =   (vi.IsPlanar ())
		                         ? fdst_sptr->GetRowSize (plane_id)
		                         : vi.RowSize ();
		const int height       = fdst_sptr->GetHeight (plane_id);

		// Setup and run the tasks. We need to provide the slicer with
		// the plane height so it can generate coherent top and bottom
		// row indexes for each slice.
		pproc._slicer.start (height, pproc._tdg, &AvstpAverage::process_subplane);
	}

	// Waits for all the tasks to terminate
	for (int p = 0; p < _nbr_planes; ++p)
	{
		pproc_arr [p]._slicer.wait ();
	}

	return (fdst_sptr);
}



void	AvstpAverage::process_subplane (Slicer::TaskData &td)
{
	assert (&td != 0);

	const TaskDataGlobal &	tdg = *(td._glob_data_ptr);

	// Sets our pointers to the slice top location
	::BYTE *			dst_ptr = tdg._dst_ptr + td._y_beg * tdg._stride_dst;
	const ::BYTE *	sr1_ptr = tdg._sr1_ptr + td._y_beg * tdg._stride_sr1;
	const ::BYTE *	sr2_ptr = tdg._sr2_ptr + td._y_beg * tdg._stride_sr2;

	// Slice processing
	for (int y = td._y_beg; y < td._y_end; ++y)
	{
		for (int x = 0; x < tdg._w; ++x)
		{
			dst_ptr [x] = ::BYTE ((sr1_ptr [x] + sr2_ptr [x] + 1) >> 1);
		}

		dst_ptr += tdg._stride_dst;
		sr1_ptr += tdg._stride_sr1;
		sr2_ptr += tdg._stride_sr2;
	}
}



bool	AvstpAverage::is_same_clip_format (const ::PClip &test_clip_sptr) const
{
	assert (&test_clip_sptr != 0);
	assert (test_clip_sptr != 0);

	const ::VideoInfo &	vi_tst = test_clip_sptr->GetVideoInfo ();

	return (   vi_tst.height     == vi.height
	        && vi_tst.width      == vi.width
		     && vi_tst.num_frames == vi.num_frames
		     && vi_tst.IsSameColorspace (vi));
}



/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



::AVSValue __cdecl	create_avstp_example (::AVSValue args, void *user_data_ptr, ::IScriptEnvironment *env_ptr)
{
	return (new AvstpAverage (
		env_ptr,
		args [0].AsClip (),	// src1
		args [1].AsClip ()	// src2
	));  
}



// The following function is the function that actually registers the filter in AviSynth
// It is called automatically, when the plugin is loaded to see which functions this filter contains.
extern "C" __declspec (dllexport) const char * __stdcall	AvisynthPluginInit2 (::IScriptEnvironment *env_ptr)
{
	// The AddFunction has the following paramters:
	// AddFunction(Filtername , Arguments, Function to call,0);

	// Arguments is a string that defines the types and optional names of the arguments for your filter.
	// c - Video Clip
	// i - Integer number
	// f - Float number
	// s - String
	// b - boolean

	// The word inside the [ ] lets you used named parameters in your script
	// e.g last=SimpleSample(last,windowclip,size=100).
	// but last=SimpleSample(last,windowclip, 100) will also work 
	env_ptr->AddFunction (
		"avstp_average",
		"cc",
		create_avstp_example,
		0
	);

	// A freeform name of the plugin.
	return ("AVSTP example");
}
