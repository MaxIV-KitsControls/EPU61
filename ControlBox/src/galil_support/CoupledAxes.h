//=============================================================================
// CoupledAxes.h
//=============================================================================
// abstraction.......Galil CoupledAxes abstraction class
// use...............for special geared axes that cannot be handled by CoupledAxes
//                   this class drives only the master axis
// class.............CoupledAxes
// original author...N.Leclercq - J.Coquet - SOLEIL
//=============================================================================

#ifndef _COUPLED_AXES_H_
#define _COUPLED_AXES_H_

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
//- TYPEDEF: CoupledAxesCommandIdentifier
//------------------------------------------------------------------------------
typedef enum
{
  COUPLED_DEVICE_MIN_COMMAND = -1,
  //-------------
  COUPLED_DEVICE_FORWARD_CMD,
  COUPLED_DEVICE_BACKWARD_CMD,
  COUPLED_DEVICE_ABS_POS_CMD,
  //-------------
  COUPLED_DEVICE_MAX_COMMAND
} CoupledAxesCommandIdentifier; 

//------------------------------------------------------------------------------
//- TYPEDEF: CoupledAxesStatus - a dedicated type for CoupledAxes status
//------------------------------------------------------------------------------
typedef struct CoupledAxesStatus
{
  //- members -------------------
  //- master axis status : a dedicated type for Axis status
  AxisStatus master_status;
  //- CoupledAxes composite status
  std::string coupled_axes_status;
  //- ctor ----------------------
  CoupledAxesStatus ();
  //- operator= -----------------
  void operator= (const CoupledAxesStatus& src);
} CoupledAxesStatus;


//------------------------------------------------------------------------------
//- CLASS: CoupledAxes
//------------------------------------------------------------------------------
class CoupledAxes : public yat4tango::TangoLogAdapter
{
public:
  //- a dedicated type for CoupledAxes configuration
  //-----------------------------------------
  typedef struct Config
  {
    //- members --------------------
      //- axis ID
      AxisIdentifier master;
      //- host device
      Tango::DeviceImpl * host_device;
    //- ctor -----------------------
    Config ();
    //- operator= ------------------
    void operator= (const Config& src);
  } Config;

  //- ctor
  CoupledAxes ();

  //- dtor
  virtual ~CoupledAxes ();

  //- CoupledAxes (re)configuration -----------
  void configure (const Config& cfg)
    throw (Tango::DevFailed);

  //- position mutator ------------------------
  void position (double p)
    throw (Tango::DevFailed);

  //- position accessor -----------------------
  double position ();

  //- Forward cmd. request --------------------
  void forward ()
    throw (Tango::DevFailed); 

  //- Backward cmd. request -------------------
  void backward ()
    throw (Tango::DevFailed); 

  //- Stop cmd. request -----------------------
  void stop ()
    throw (Tango::DevFailed); 

  //- CoupledAxes (composite of Axis) state accessor -
  AxisState state ();

  //- CoupledAxes status accessor --------------------
  void status (CoupledAxesStatus & s)
    throw (Tango::DevFailed);

  //- CoupledAxes forward limit switch accessor --------------------
  bool forward_limit_switch_detected ()
    throw (Tango::DevFailed)
	{
		return this->m_master_axis->forward_limit_switch_detected();
	};

	//- CoupledAxes backward limit switch accessor --------------------
  bool backward_limit_switch_detected ()
    throw (Tango::DevFailed)
	{
		return this->m_master_axis->backward_limit_switch_detected();
	};

private:
  //---------------------------------------------------------
  // CoupledAxes::command_allowed
  //---------------------------------------------------------
  // Given the current status of the CoupledAxes, this 
  // method checks whether or not the specified command is 
  // allowed. In case the command is refused and the <why> is 
  // not NULL, the reason is returned in the provided string.
  //---------------------------------------------------------
  bool command_allowed (CoupledAxesCommandIdentifier cacid, std::string * why = 0)
    throw (Tango::DevFailed);

  //---------------------------------------------------------
	// CoupledAxes::why_cmd_refused 
  //---------------------------------------------------------
	int why_cmd_refused (CoupledAxesCommandIdentifier cacid) const;

  //---------------------------------------------------------
	// CoupledAxes::eval_criteria
  //---------------------------------------------------------
	bool eval_criteria (int cid) const;

  //- configuration
  CoupledAxes::Config m_cfg;

  //- the associated master axis
  galil::Axis * m_master_axis;

};

} //-  namespace galil

#endif //_COUPLED_AXES_H_
