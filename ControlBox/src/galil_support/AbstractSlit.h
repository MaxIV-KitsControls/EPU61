//=============================================================================
// AbstractSlit.h
//=============================================================================
// abstraction.......Galil AbstractSlit abstraction class
// class.............AbstractSlit
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _ABSTRACT_SLIT_H_
#define _ABSTRACT_SLIT_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "Axis.h"

//=============================================================================
// CONSTs
//=============================================================================

// ============================================================================
// SHORTCUTS TO EACH <CMD REQUEST VALIDATION> CRITERIA COMPONENTS
// ============================================================================
#define MASTER_NOT_OFF             (this->m_master_axis->state() != galil::AXIS_OFF)
//-----------------------------------------------------------------------------
#define SLAVE_NOT_OFF              (this->m_slave_axis->state() != galil::AXIS_OFF)
//-----------------------------------------------------------------------------
#define MASTER_NOT_FAULT           (this->m_master_axis->state() != galil::AXIS_ERROR)
//-----------------------------------------------------------------------------
#define SLAVE_NOT_FAULT            (this->m_slave_axis->state() != galil::AXIS_ERROR)
//-----------------------------------------------------------------------------
#define MASTER_NOT_MOVING          (this->m_master_axis->state() != galil::AXIS_MOVING)
//-----------------------------------------------------------------------------
#define INDEP_MODE                 (! this->m_slave_axis->gearing_enabled())
//-----------------------------------------------------------------------------
#define SLAVE_NOT_MOVING           (! INDEP_MODE || this->m_slave_axis->state() != galil::AXIS_MOVING)
//-----------------------------------------------------------------------------
#define SLAVE_NO_FORW_LSW_PRESENT  (! this->m_slave_axis->forward_limit_switch_detected())
//-----------------------------------------------------------------------------
#define SLAVE_NO_BACK_LSW_PRESENT  (! this->m_slave_axis->backward_limit_switch_detected()) 
//-----------------------------------------------------------------------------
#define SLIT_NOT_MOVING            (MASTER_NOT_MOVING)
//-----------------------------------------------------------------------------
#define SLIT_NOT_MOVING_OR_INDEP   ( SLIT_NOT_MOVING || INDEP_MODE )



namespace galil
{

//------------------------------------------------------------------------------
//- TYPEDEF: AbstractSlitCommandIdentifier
//------------------------------------------------------------------------------
typedef enum
{
  SLIT_GAP_MOVE,
  SLIT_POS_MOVE,
  SLIT_INDEP_MASTER_MOVE,
  SLIT_INDEP_SLAVE_MOVE,
  //-------------
  SLIT_MAX_COMMAND
} AbstractSlitCommandIdentifier; 



//------------------------------------------------------------------------------
//- TYPEDEF: AbstractSlitStatus - a dedicated type for AbstractSlit status
//------------------------------------------------------------------------------
//- a dedicated type for Axis status
//-----------------------------------------
typedef struct SlitStatus
{
  //- members -------------------
  //- master axis status
  AxisStatus master_status;
  //- slave axis status
  AxisStatus slave_status;
  //- slit composite status
  std::string slit_status;
  //- ctor ----------------------
  SlitStatus ();
  //- operator= -----------------
  void operator= (const SlitStatus& src);
} SlitStatus;

//------------------------------------------------------------------------------
//- CLASS: AbstractSlit
//------------------------------------------------------------------------------
class AbstractSlit : public yat4tango::TangoLogAdapter
{
public:
  //- a dedicated type for AbstractSlit configuration
  //-----------------------------------------
  typedef struct Config
  {
    //- members --------------------
      //- axis ID
      AxisIdentifier master;
      //- axis ID
      AxisIdentifier slave;
      //- convention for motion direction 
      bool invert_pos_dir;
      //- host device
      Tango::DeviceImpl * host_device;
    //- ctor -----------------------
    Config ();
    //- operator= ------------------
    void operator= (const Config& src);
  } Config;

  //- ctor
  AbstractSlit ();

  //- dtor
  virtual ~AbstractSlit ();

  //- AbstractSlit (re)configuration ----------
  void configure (const Config& cfg)
    throw (Tango::DevFailed);

  //- gap mutator -----------------------------
  //- pure virtual - must be overloaded
  virtual void gap (double _g)
    throw (Tango::DevFailed) = 0;

  //- gap accessor ----------------------------
  virtual double gap ();

  //- position mutator ------------------------
  //- pure virtual - must be overloaded
  virtual void position (double p)
    throw (Tango::DevFailed) = 0;

  //- position accessor -----------------------
  virtual double position ();

  //- master axis position mutator ------------
  //- pure virtual - must be overloaded
  virtual void master_position (double p)
    throw (Tango::DevFailed) = 0;

  //- master axis position accessor -----------
  virtual double master_position ();

  //- slave axis position mutator -------------
  //- pure virtual - must be overloaded
  virtual void slave_position (double p)
    throw (Tango::DevFailed) = 0;

  //- slave axis position accessor ------------
  virtual double slave_position ();

  //- stop both the master & slave axis -------
  //- pure virtual - must be overloaded
  virtual void stop ()
    throw (Tango::DevFailed) = 0;

  //- turn Off both the master & slave axis -------
  virtual void off ()
    throw (Tango::DevFailed);

  //- turn On both the master & slave axis -------
  virtual void on ()
    throw (Tango::DevFailed);

  //- disables gearing on both axis -----------
  void disable_gearing ()
    throw (Tango::DevFailed);

  //- AbstractSlit (composite of Axis) state accessor -
  virtual AxisState state () = 0;

  //- AbstractSlit status accessor --------------------
  virtual void status (SlitStatus & s)
    throw (Tango::DevFailed) = 0;

protected :
  //---------------------------------------------------------
  // AbstractSlit::command_allowed
  //---------------------------------------------------------
  // Given the current status of the AbstractSlit, this 
  // method checks whether or not the specified command is 
  // allowed. In case the command is refused and the <why> is 
  // not NULL, the reason is returned in the provided string.
  //---------------------------------------------------------
  virtual   bool command_allowed (AbstractSlitCommandIdentifier ascid, std::string * why = 0)
    throw (Tango::DevFailed) = 0;

  //---------------------------------------------------------
	// AbstractSlit::why_cmd_refused 
  //---------------------------------------------------------
  virtual int why_cmd_refused (AbstractSlitCommandIdentifier _cid) const = 0;

  //---------------------------------------------------------
	// GearedSlit::eval_criteria
  //---------------------------------------------------------
  virtual bool eval_criteria (int cid) const = 0;

  //- configuration
  AbstractSlit::Config m_cfg;

  //- the associated master axis
  galil::Axis * m_master_axis;

  //- the associated slave axis
  galil::Axis * m_slave_axis;

  //- unimplemented/unsupported members
  //-----------------------------------
  AbstractSlit (const AbstractSlit &);
  AbstractSlit & operator= (const AbstractSlit &);
};

} //-  namespace galil

#endif //-- _ABSTRACT_SLIT_H_
