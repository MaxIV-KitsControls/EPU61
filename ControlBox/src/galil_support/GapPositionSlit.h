//=============================================================================
// Slit.h
//=============================================================================
// abstraction.......Galil Slit abstraction class
// class.............Slit
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _GAP_POSITION_SLIT_H_
#define _GAP_POSITION_SLIT_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "Axis.h"
#include "AbstractSlit.h"

//=============================================================================
// CONSTs
//=============================================================================
// ============================================================================
// SHORTCUTS TO EACH <CMD REQUEST VALIDATION> CRITERIA COMPONENTS
// ============================================================================
#define GAP_NOT_OFF                   (this->m_master_axis->state() != galil::AXIS_OFF)
//-----------------------------------------------------------------------------
#define POSITION_NOT_OFF              (this->m_slave_axis->state() != galil::AXIS_OFF)
//-----------------------------------------------------------------------------
#define GAP_NOT_FAULT                 (this->m_master_axis->state() != galil::AXIS_ERROR)
//-----------------------------------------------------------------------------
#define POSITION_NOT_FAULT            (this->m_slave_axis->state() != galil::AXIS_ERROR)
//-----------------------------------------------------------------------------
#define GAP_NOT_MOVING                (this->m_master_axis->state() != galil::AXIS_MOVING)
//-----------------------------------------------------------------------------
#define POSITION_NOT_MOVING           (this->m_slave_axis->state() != galil::AXIS_MOVING)
//-----------------------------------------------------------------------------
#define GAP_NO_FORW_LSW_PRESENT       (! this->m_master_axis->forward_limit_switch_detected())
//-----------------------------------------------------------------------------
#define GAP_NO_BACK_LSW_PRESENT       (! this->m_master_axis->backward_limit_switch_detected()) 
//-----------------------------------------------------------------------------
#define POSITION_NO_FORW_LSW_PRESENT  (! this->m_slave_axis->forward_limit_switch_detected())
//-----------------------------------------------------------------------------
#define POSITION_NO_BACK_LSW_PRESENT  (! this->m_slave_axis->backward_limit_switch_detected()) 
//-----------------------------------------------------------------------------
#define INDEP_MODE                    (! this->m_slave_axis->gearing_enabled())



namespace galil
{


//------------------------------------------------------------------------------
//- CLASS: Slit
//------------------------------------------------------------------------------
class GapPositionSlit : public AbstractSlit
{
public:
  //- ctor
  GapPositionSlit ();

  //- dtor
  virtual ~GapPositionSlit ();

  //- gap mutator -----------------------------
	//- AbstractSlit pure virtual method, must be overloaded
  void gap (double _g)
    throw (Tango::DevFailed);

  //- gap accessor ----------------------------
	//- must be overloaded : accesses to gap motor position
  double gap (void);

  //- position mutator ------------------------
	//- AbstractSlit pure virtual method, must be overloaded
  void position (double p)
    throw (Tango::DevFailed);

  //- position accessor -----------------------
	//- AbstractSlit implementation 
	//- must be overloaded : accesses to position motor position
  double position (void);

  //- master axis position mutator ------------
	//- AbstractSlit pure virtual method, must be overloaded
  void master_position (double p)
    throw (Tango::DevFailed);

  //- master axis position accessor -----------
	//- must be overloaded : accesses 2 motors and compute position
  double master_position (void);

  //- slave axis position mutator -------------
	//- AbstractSlit pure virtual method, must be overloaded
  void slave_position (double p)
    throw (Tango::DevFailed);

  //- slave axis position accessor ------------
	//- AbstractSlit implementation 
	//- must be overloaded : accesses 2 motors and compute position
  double slave_position (void);

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
  GapPositionSlit (const GapPositionSlit &);
  GapPositionSlit & operator= (const GapPositionSlit &);

protected: 
	//---------------------------------------------------------
  // GapPositionSlit::command_allowed
  //---------------------------------------------------------
  // Given the current status of the GapPositionSlit, this 
  // method checks whether or not the specified command is 
  // allowed. In case the command is refused and the <why> is 
  // not NULL, the reason is returned in the provided string.
  //---------------------------------------------------------
  bool command_allowed (AbstractSlitCommandIdentifier ascid, std::string * why = 0)
    throw (Tango::DevFailed);

  //---------------------------------------------------------
	// GapPositionSlit::why_cmd_refused 
  //---------------------------------------------------------
	int why_cmd_refused (AbstractSlitCommandIdentifier _cid) const;

  //---------------------------------------------------------
	// GapPositionSlit::eval_criteria
  //---------------------------------------------------------
	bool eval_criteria (int cid) const;

};

} //-  namespace galil

#endif //- _GAP_POSITION_SLIT_H_
