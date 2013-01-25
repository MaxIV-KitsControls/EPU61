//=============================================================================
// GearedAxes.h
//=============================================================================
// abstraction.......Galil GearedAxes abstraction class
// class.............GearedAxes
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _GEARED_AXES_H_
#define _GEARED_AXES_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "Axis.h"

//=============================================================================
// CONSTs
//=============================================================================

namespace galil
{

//------------------------------------------------------------------------------
//- TYPEDEF: GearedAxesCommandIdentifier
//------------------------------------------------------------------------------
typedef enum
{
  GEARED_AXES_MIN_COMMAND = -1,
  //-------------
  GEARED_AXES_GEARED_MOVE,
  GEARED_AXES_INDEP_MASTER_MOVE,
  GEARED_AXES_INDEP_SLAVE_MOVE,
  GEARED_AXES_ENABLE_GEARING,
  GEARED_AXES_DISABLE_GEARING,
  //-------------
  GEARED_AXES_MAX_COMMAND
} GearedAxesCommandIdentifier; 


//------------------------------------------------------------------------------
//- TYPEDEF: GearedAxesStatus - a dedicated type for GearedAxes status
//------------------------------------------------------------------------------
//- a dedicated type for Axis status
//-----------------------------------------
typedef struct GearedAxesStatus
{
  //- members -------------------
  //- master axis status
  AxisStatus master_status;
  //- slave axis status
  AxisStatus slave_status;
  //- slit composite status
  std::string slit_status;
  //- ctor ----------------------
  GearedAxesStatus ();
  //- operator= -----------------
  void operator= (const GearedAxesStatus& src);
} GearedAxesStatus;

//------------------------------------------------------------------------------
//- CLASS: GearedAxes
//------------------------------------------------------------------------------
class GearedAxes : public yat4tango::TangoLogAdapter
{
public:
  //- a dedicated type for GearedAxes configuration
  //-----------------------------------------
  typedef struct Config
  {
    //- members --------------------
      //- axis ID
      AxisIdentifier master;
      //- axis ID
      AxisIdentifier slave;
      //- convention for motion direction 
      double user_ratio;
      //- host device
      Tango::DeviceImpl * host_device;
    //- ctor -----------------------
    Config ();
    //- operator= ------------------
    void operator= (const Config& src);
  } Config;

  //- ctor
  GearedAxes ();

  //- dtor
  virtual ~GearedAxes ();

  //- GearedAxes (re)configuration ------------
  void configure (const Config& cfg)
    throw (Tango::DevFailed);

  //- master axis position mutator ------------
  void master_position (double p)
    throw (Tango::DevFailed);

  //- master axis position accessor -----------
  double master_position ();

  //- slave axis position mutator -------------
  void slave_position (double p)
    throw (Tango::DevFailed);

  //- slave axis position accessor ------------
  double slave_position ();

  //- stop both the master & slave axis -------
  void stop ()
    throw (Tango::DevFailed);

  //- disables gearing on both axis -----------
  void disable_gearing ()
    throw (Tango::DevFailed);

  //- enables gearing -------------------------
  void enable_gearing ()
    throw (Tango::DevFailed);

  //- enables gearing -------------------------
  bool is_gearing_enabled ()
	  throw (Tango::DevFailed)
  {
	  return this->m_slave_axis->gearing_enabled();
  };
  //- slit (composite of Axis) state accessor -
  AxisState state ();

  //- slit status accessor --------------------
  void status (GearedAxesStatus & s)
    throw (Tango::DevFailed);

private:
  //---------------------------------------------------------
  // GearedAxes::command_allowed
  //---------------------------------------------------------
  // Given the current status of the slit, this 
  // method checks whether or not the specified command is 
  // allowed. In case the command is refused and the <why> is 
  // not NULL, the reason is returned in the provided string.
  //---------------------------------------------------------
  bool command_allowed (GearedAxesCommandIdentifier scid, std::string * why = 0)
    throw (Tango::DevFailed);

  //---------------------------------------------------------
	// GearedAxes::why_cmd_refused 
  //---------------------------------------------------------
	int why_cmd_refused (GearedAxesCommandIdentifier _cid) const;

  //---------------------------------------------------------
	// GearedAxes::eval_criteria
  //---------------------------------------------------------
	bool eval_criteria (int cid) const;

  //- configuration
  GearedAxes::Config m_cfg;

  //- the associated master axis
  galil::Axis * m_master_axis;

  //- the associated slave axis
  galil::Axis * m_slave_axis;

  //- unimplemented/unsupported members
  //-----------------------------------
  GearedAxes (const GearedAxes &);
  GearedAxes & operator= (const GearedAxes &);
};

} //-  namespace galil

#endif //-- _GEARED_AXES_H_
