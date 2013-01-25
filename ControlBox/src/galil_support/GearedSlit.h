//=============================================================================
// Slit.h
//=============================================================================
// abstraction.......Galil Slit abstraction class
// class.............Slit
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _GEARED_SLIT_H_
#define _GEARED_SLIT_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "Axis.h"
#include "AbstractSlit.h"
//=============================================================================
// CONSTs
//=============================================================================

namespace galil
{


//------------------------------------------------------------------------------
//- CLASS: Slit
//------------------------------------------------------------------------------
class GearedSlit : public AbstractSlit
{
public:
  //- ctor
  GearedSlit ();

  //- dtor
  virtual ~GearedSlit ();

  //- gap mutator -----------------------------
	//- AbstractSlit pure virtual method, must be overloaded
  void gap (double _g)
    throw (Tango::DevFailed);

  //- gap accessor ----------------------------
	//- AbstractSlit implementation 

  //- position mutator ------------------------
	//- AbstractSlit pure virtual method, must be overloaded
  void position (double p)
    throw (Tango::DevFailed);

  //- position accessor -----------------------
	//- AbstractSlit implementation 

  //- master axis position mutator ------------
	//- AbstractSlit pure virtual method, must be overloaded
  void master_position (double p)
    throw (Tango::DevFailed);

  //- master axis position accessor -----------
	//- AbstractSlit implementation 

  //- slave axis position mutator -------------
	//- AbstractSlit pure virtual method, must be overloaded
  void slave_position (double p)
    throw (Tango::DevFailed);

  //- slave axis position accessor ------------
	//- AbstractSlit implementation 

  //- stop both the master & slave axis -------
	//- AbstractSlit pure virtual method, must be overloaded
  void stop ()
    throw (Tango::DevFailed);

  //- disables gearing on both axis -----------
	//- AbstractSlit implementation 

  //- slit (composite of Axis) state accessor -
	//- AbstractSlit pure virtual method, must be overloaded
  AxisState state ()
		throw (Tango::DevFailed);

  //- slit status accessor --------------------
	//- AbstractSlit pure virtual method, must be overloaded
   void status (SlitStatus & s)
    throw (Tango::DevFailed);

private:

  //- unimplemented/unsupported members
  //-----------------------------------
  GearedSlit (const GearedSlit &);
  GearedSlit & operator= (const GearedSlit &);

protected: 
	//---------------------------------------------------------
  // GearedSlit::command_allowed
  //---------------------------------------------------------
  // Given the current status of the GearedSlit, this 
  // method checks whether or not the specified command is 
  // allowed. In case the command is refused and the <why> is 
  // not NULL, the reason is returned in the provided string.
  //---------------------------------------------------------
  bool command_allowed (AbstractSlitCommandIdentifier ascid, std::string * why = 0)
    throw (Tango::DevFailed);

  //---------------------------------------------------------
	// GearedSlit::why_cmd_refused 
  //---------------------------------------------------------
	int why_cmd_refused (AbstractSlitCommandIdentifier _cid) const;

  //---------------------------------------------------------
	// GearedSlit::eval_criteria
  //---------------------------------------------------------
	bool eval_criteria (int cid) const;

};

} //-  namespace galil

#endif //-- _SLIT_H_
