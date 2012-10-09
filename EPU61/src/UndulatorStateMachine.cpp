/*----- PROTECTED REGION ID(UndulatorStateMachine.cpp) ENABLED START -----*/
static const char *RcsId = "$Id:  $";
//=============================================================================
//
// file :        UndulatorStateMachine.cpp
//
// description : C++ source for the �name� and its alowed
//               methods for commands and attributes
//
// project :     .
//
// $Author:  $
//
// $Revision:  $
// $Date:  $
//
// SVN only:
// $HeadURL:  $
//
// CVS only:
// $Source:  $
// $Log:  $
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================



#include <Undulator.h>
#include <UndulatorClass.h>

/*----- PROTECTED REGION END -----*/


/*
 * Undulator states description:
 *
 * STANDBY :	Motors are ready to move. 
 * FAULT :	Error detected during operation.  
 * MOVING :	Motors are moving. Please use the GapMoving and PhaseMoving boolean attributes
for distinguishing between movement types. 
 * DISABLE :	Interlock condition or StopAll status have disabled the device.
 
 * ALARM :	Something is preventing axes to go into standby. 
For more details check status message. 
 */

namespace Undulator_ns
{

//=================================================
//		Attributes Allowed Methods
//=================================================

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_GapReadbackState_allowed()
 *	Description : Execution allowed for GapReadback attribute.
 */
//--------------------------------------------------------

bool Undulator::is_GapReadback_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for GapReadback attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_GapReadbackStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_GapReadbackStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_GapSetpointState_allowed()
 *	Description : Execution allowed for GapSetpoint attribute.
 */
//--------------------------------------------------------

bool Undulator::is_GapSetpoint_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for GapSetpoint attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::GapSetpointStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::GapSetpointStateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for GapSetpoint attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_GapSetpointStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_GapSetpointStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_EngineeringLockState_allowed()
 *	Description : Execution allowed for EngineeringLock attribute.
 */
//--------------------------------------------------------

bool Undulator::is_EngineeringLock_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for EngineeringLock attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::EngineeringLockStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::EngineeringLockStateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for EngineeringLock attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_EngineeringLockStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_EngineeringLockStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_GapAccelerationState_allowed()
 *	Description : Execution allowed for GapAcceleration attribute.
 */
//--------------------------------------------------------

bool Undulator::is_GapAcceleration_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::GapAccelerationStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::GapAccelerationStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for GapAcceleration attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_GapAccelerationStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_GapAccelerationStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_GapSpeedState_allowed()
 *	Description : Execution allowed for GapSpeed attribute.
 */
//--------------------------------------------------------

bool Undulator::is_GapSpeed_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::GapSpeedStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::GapSpeedStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for GapSpeed attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_GapSpeedStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_GapSpeedStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_OffsetReadbackState_allowed()
 *	Description : Execution allowed for OffsetReadback attribute.
 */
//--------------------------------------------------------

bool Undulator::is_OffsetReadback_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for OffsetReadback attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_OffsetReadbackStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_OffsetReadbackStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_OffsetSetpointState_allowed()
 *	Description : Execution allowed for OffsetSetpoint attribute.
 */
//--------------------------------------------------------

bool Undulator::is_OffsetSetpoint_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::OffsetSetpointStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::OffsetSetpointStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for OffsetSetpoint attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_OffsetSetpointStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_OffsetSetpointStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_PhaseAccelerationState_allowed()
 *	Description : Execution allowed for PhaseAcceleration attribute.
 */
//--------------------------------------------------------

bool Undulator::is_PhaseAcceleration_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::PhaseAccelerationStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::PhaseAccelerationStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for PhaseAcceleration attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_PhaseAccelerationStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_PhaseAccelerationStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_PhaseModeReadbackState_allowed()
 *	Description : Execution allowed for PhaseModeReadback attribute.
 */
//--------------------------------------------------------

bool Undulator::is_PhaseModeReadback_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for PhaseModeReadback attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_PhaseModeReadbackStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_PhaseModeReadbackStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_PhaseModeSetpointState_allowed()
 *	Description : Execution allowed for PhaseModeSetpoint attribute.
 */
//--------------------------------------------------------

bool Undulator::is_PhaseModeSetpoint_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::PhaseModeSetpointStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::PhaseModeSetpointStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for PhaseModeSetpoint attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_PhaseModeSetpointStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_PhaseModeSetpointStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_PhaseReadbackState_allowed()
 *	Description : Execution allowed for PhaseReadback attribute.
 */
//--------------------------------------------------------

bool Undulator::is_PhaseReadback_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for PhaseReadback attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_PhaseReadbackStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_PhaseReadbackStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_PhaseSetpointState_allowed()
 *	Description : Execution allowed for PhaseSetpoint attribute.
 */
//--------------------------------------------------------

bool Undulator::is_PhaseSetpoint_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for PhaseSetpoint attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::PhaseSetpointStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::PhaseSetpointStateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for PhaseSetpoint attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_PhaseSetpointStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_PhaseSetpointStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_PhaseSpeedState_allowed()
 *	Description : Execution allowed for PhaseSpeed attribute.
 */
//--------------------------------------------------------

bool Undulator::is_PhaseSpeed_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::PhaseSpeedStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::PhaseSpeedStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for PhaseSpeed attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_PhaseSpeedStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_PhaseSpeedStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_TaperAccelerationState_allowed()
 *	Description : Execution allowed for TaperAcceleration attribute.
 */
//--------------------------------------------------------

bool Undulator::is_TaperAcceleration_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::TaperAccelerationStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::TaperAccelerationStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for TaperAcceleration attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_TaperAccelerationStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_TaperAccelerationStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_TaperReadbackState_allowed()
 *	Description : Execution allowed for TaperReadback attribute.
 */
//--------------------------------------------------------

bool Undulator::is_TaperReadback_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for TaperReadback attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_TaperReadbackStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_TaperReadbackStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_TaperSetpointState_allowed()
 *	Description : Execution allowed for TaperSetpoint attribute.
 */
//--------------------------------------------------------

bool Undulator::is_TaperSetpoint_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::TaperSetpointStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::TaperSetpointStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for TaperSetpoint attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_TaperSetpointStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_TaperSetpointStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_TaperSpeedState_allowed()
 *	Description : Execution allowed for TaperSpeed attribute.
 */
//--------------------------------------------------------

bool Undulator::is_TaperSpeed_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT)
		{
		
	/*----- PROTECTED REGION ID(Undulator::TaperSpeedStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::TaperSpeedStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for TaperSpeed attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_TaperSpeedStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_TaperSpeedStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_LimitsDisabledState_allowed()
 *	Description : Execution allowed for LimitsDisabled attribute.
 */
//--------------------------------------------------------

bool Undulator::is_LimitsDisabled_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for LimitsDisabled attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_LimitsDisabledStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_LimitsDisabledStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_StopAllState_allowed()
 *	Description : Execution allowed for StopAll attribute.
 */
//--------------------------------------------------------

bool Undulator::is_StopAll_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for StopAll attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_StopAllStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_StopAllStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_GapMovingState_allowed()
 *	Description : Execution allowed for GapMoving attribute.
 */
//--------------------------------------------------------

bool Undulator::is_GapMoving_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for GapMoving attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_GapMovingStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_GapMovingStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_PhaseMovingState_allowed()
 *	Description : Execution allowed for PhaseMoving attribute.
 */
//--------------------------------------------------------

bool Undulator::is_PhaseMoving_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for PhaseMoving attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_PhaseMovingStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_PhaseMovingStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_EngineeringGapSpeedState_allowed()
 *	Description : Execution allowed for EngineeringGapSpeed attribute.
 */
//--------------------------------------------------------

bool Undulator::is_EngineeringGapSpeed_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT	|| 
			get_state() == Tango::MOVING	|| 
			get_state() == Tango::DISABLE	|| 
			get_state() == Tango::ALARM)
		{
		
	/*----- PROTECTED REGION ID(Undulator::EngineeringGapSpeedStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::EngineeringGapSpeedStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for EngineeringGapSpeed attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_EngineeringGapSpeedStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_EngineeringGapSpeedStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_EngineeringPhaseSpeedState_allowed()
 *	Description : Execution allowed for EngineeringPhaseSpeed attribute.
 */
//--------------------------------------------------------

bool Undulator::is_EngineeringPhaseSpeed_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT	|| 
			get_state() == Tango::MOVING	|| 
			get_state() == Tango::DISABLE	|| 
			get_state() == Tango::ALARM)
		{
		
	/*----- PROTECTED REGION ID(Undulator::EngineeringPhaseSpeedStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::EngineeringPhaseSpeedStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for EngineeringPhaseSpeed attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_EngineeringPhaseSpeedStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_EngineeringPhaseSpeedStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_EngineeringGapAccelerationState_allowed()
 *	Description : Execution allowed for EngineeringGapAcceleration attribute.
 */
//--------------------------------------------------------

bool Undulator::is_EngineeringGapAcceleration_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT	|| 
			get_state() == Tango::MOVING	|| 
			get_state() == Tango::DISABLE	|| 
			get_state() == Tango::ALARM)
		{
		
	/*----- PROTECTED REGION ID(Undulator::EngineeringGapAccelerationStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::EngineeringGapAccelerationStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for EngineeringGapAcceleration attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_EngineeringGapAccelerationStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_EngineeringGapAccelerationStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_EngineeringPhaseAccelerationState_allowed()
 *	Description : Execution allowed for EngineeringPhaseAcceleration attribute.
 */
//--------------------------------------------------------

bool Undulator::is_EngineeringPhaseAcceleration_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
		if (	//	Compare device state with not allowed states for WRITE 
			get_state() == Tango::FAULT	|| 
			get_state() == Tango::MOVING	|| 
			get_state() == Tango::DISABLE	|| 
			get_state() == Tango::ALARM)
		{
		
	/*----- PROTECTED REGION ID(Undulator::EngineeringPhaseAccelerationStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::EngineeringPhaseAccelerationStateAllowed_WRITE

			return false;
		}
		return true;
	}
	else
		//	Not any excluded states for EngineeringPhaseAcceleration attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_EngineeringPhaseAccelerationStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_EngineeringPhaseAccelerationStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_InterlockState_allowed()
 *	Description : Execution allowed for Interlock attribute.
 */
//--------------------------------------------------------

bool Undulator::is_Interlock_allowed(Tango::AttReqType type)
{
		//	Not any excluded states for Interlock attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_InterlockStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_InterlockStateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_Coil1State_allowed()
 *	Description : Execution allowed for Coil1 attribute.
 */
//--------------------------------------------------------

bool Undulator::is_Coil1_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for Coil1 attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::Coil1StateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::Coil1StateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for Coil1 attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_Coil1StateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_Coil1StateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_Coil2State_allowed()
 *	Description : Execution allowed for Coil2 attribute.
 */
//--------------------------------------------------------

bool Undulator::is_Coil2_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for Coil2 attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::Coil2StateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::Coil2StateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for Coil2 attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_Coil2StateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_Coil2StateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_Coil3State_allowed()
 *	Description : Execution allowed for Coil3 attribute.
 */
//--------------------------------------------------------

bool Undulator::is_Coil3_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for Coil3 attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::Coil3StateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::Coil3StateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for Coil3 attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_Coil3StateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_Coil3StateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_Coil4State_allowed()
 *	Description : Execution allowed for Coil4 attribute.
 */
//--------------------------------------------------------

bool Undulator::is_Coil4_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for Coil4 attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::Coil4StateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::Coil4StateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for Coil4 attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_Coil4StateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_Coil4StateAllowed_READ

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_CorrectionEnabledState_allowed()
 *	Description : Execution allowed for CorrectionEnabled attribute.
 */
//--------------------------------------------------------

bool Undulator::is_CorrectionEnabled_allowed(Tango::AttReqType type)
{
	//	Check if access type.
	if ( type!=Tango::READ_REQ )
	{
			//	Not any excluded states for CorrectionEnabled attribute in WRITE access.
		
	/*----- PROTECTED REGION ID(Undulator::CorrectionEnabledStateAllowed_WRITE) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::CorrectionEnabledStateAllowed_WRITE

		return true;
	}
	else
		//	Not any excluded states for CorrectionEnabled attribute in READ access.
	
	/*----- PROTECTED REGION ID(Undulator::read_CorrectionEnabledStateAllowed_READ) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::read_CorrectionEnabledStateAllowed_READ

	return true;
}


	/*----- PROTECTED REGION ID(Undulator::are_dynamic_attributes_allowed) ENABLED START -----*/

	//	Add your code to check if dynamic attributes are alowed

	/*----- PROTECTED REGION END -----*/	//	Undulator::are_dynamic_attributes_allowed


//=================================================
//		Commands Allowed Methods
//=================================================


//--------------------------------------------------------
/**
 *	Method      : Undulator::is_StopGapState_allowed()
 *	Description : Execution allowed for StopGap command.
 */
//--------------------------------------------------------

bool Undulator::is_StopGap_allowed(const CORBA::Any &any)
{
	if (	//	Compare device state with not allowed states for command 
		get_state() == Tango::FAULT	|| 
		get_state() == Tango::DISABLE)
	{

	/*----- PROTECTED REGION ID(Undulator::StopGapStateAllowed) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::StopGapStateAllowed

		return false;
	}
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_ToggleStopAllState_allowed()
 *	Description : Execution allowed for ToggleStopAll command.
 */
//--------------------------------------------------------

bool Undulator::is_ToggleStopAll_allowed(const CORBA::Any &any)
{
	if (	//	Compare device state with not allowed states for command 
		get_state() == Tango::FAULT)
	{

	/*----- PROTECTED REGION ID(Undulator::ToggleStopAllStateAllowed) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::ToggleStopAllStateAllowed

		return false;
	}
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_DisableLimitsState_allowed()
 *	Description : Execution allowed for DisableLimits command.
 */
//--------------------------------------------------------

bool Undulator::is_DisableLimits_allowed(const CORBA::Any &any)
{
	if (	//	Compare device state with not allowed states for command 
		get_state() == Tango::FAULT	|| 
		get_state() == Tango::MOVING)
	{

	/*----- PROTECTED REGION ID(Undulator::DisableLimitsStateAllowed) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::DisableLimitsStateAllowed

		return false;
	}
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_StopPhaseState_allowed()
 *	Description : Execution allowed for StopPhase command.
 */
//--------------------------------------------------------

bool Undulator::is_StopPhase_allowed(const CORBA::Any &any)
{
	if (	//	Compare device state with not allowed states for command 
		get_state() == Tango::FAULT	|| 
		get_state() == Tango::DISABLE)
	{

	/*----- PROTECTED REGION ID(Undulator::StopPhaseStateAllowed) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::StopPhaseStateAllowed

		return false;
	}
	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_CalibrateGapState_allowed()
 *	Description : Execution allowed for CalibrateGap command.
 */
//--------------------------------------------------------

bool Undulator::is_CalibrateGap_allowed(const CORBA::Any &any)
{
	//	Not any excluded states for CalibrateGap command.

	/*----- PROTECTED REGION ID(Undulator::CalibrateGapStateAllowed) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::CalibrateGapStateAllowed

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_CalibratePhaseState_allowed()
 *	Description : Execution allowed for CalibratePhase command.
 */
//--------------------------------------------------------

bool Undulator::is_CalibratePhase_allowed(const CORBA::Any &any)
{
	//	Not any excluded states for CalibratePhase command.

	/*----- PROTECTED REGION ID(Undulator::CalibratePhaseStateAllowed) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::CalibratePhaseStateAllowed

	return true;
}

//--------------------------------------------------------
/**
 *	Method      : Undulator::is_LoadCorrectionDataState_allowed()
 *	Description : Execution allowed for LoadCorrectionData command.
 */
//--------------------------------------------------------

bool Undulator::is_LoadCorrectionData_allowed(const CORBA::Any &any)
{
	//	Not any excluded states for LoadCorrectionData command.

	/*----- PROTECTED REGION ID(Undulator::LoadCorrectionDataStateAllowed) ENABLED START -----*/

	/*----- PROTECTED REGION END -----*/	//	Undulator::LoadCorrectionDataStateAllowed

	return true;
}


	/*----- PROTECTED REGION ID(Undulator::are_dynamic_commands_allowed) ENABLED START -----*/

	//	Add your code to check if dynamic commands are alowed

	/*----- PROTECTED REGION END -----*/	//	Undulator::are_dynamic_commands_allowed

}	// namespace Undulator_ns