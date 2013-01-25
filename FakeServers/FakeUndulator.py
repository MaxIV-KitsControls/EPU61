#!/usr/bin/python

import PyTango
import sys

class FakeUndulator(PyTango.Device_3Impl):
    def __init__(self,cl,name):
        PyTango.Device_3Impl.__init__(self,cl,name)
        self.debug_stream('In FakeUndulator __init__')
        FakeUndulator.init_device(self)

    def init_device(self):
        self.debug_stream('In Python init_device method')
        self.set_state(PyTango.DevState.STANDBY)
        self.attr_PhaseMoving = False
        self.attr_GapMoving = False
        self.attr_EngineeringLock = False
        self.attr_Interlock = False
        self.attr_StopAll = False
        
        self.attr_PhaseMode                  = 0
        
        self.attr_EngineeringGapAcceleration = 0
        self.attr_EngineeringGapSpeed        = 0
        self.attr_EngineeringPhaseAcceleration = 0
        self.attr_EngineeringPhaseSpeed        = 0
        self.attr_GapAcceleration            = 0
        self.attr_GapSpeed                   = 0
        self.attr_Phase                      = 0
        self.attr_PhaseAcceleration          = 0
        self.attr_PhaseSpeed                 = 0
        self.attr_Taper                      = 0
        self.attr_TaperAcceleration          = 0
        self.attr_TaperSpeed                 = 0
        self.attr_Offset                     = 0
        self.attr_Gap                        = 0
        self.attr_AxesFlags                  = 0
        
    def delete_device(self):
        self.debug_stream('[delete_device] for device %s ' % self.get_name())

    def read_attr_hardware(self, data):
        self.debug_stream('In read_attr_hardware')

    def read_PhaseMoving(self, the_att):
        the_att.set_value(self.attr_PhaseMoving)
    def write_PhaseMoving(self, the_att):
        data = the_att.get_write_value()
        self.attr_PhaseMoving = data
  
    def read_GapMoving(self, the_att):
        the_att.set_value(self.attr_GapMoving)
    def write_GapMoving(self, the_att):
        data = the_att.get_write_value()
        self.attr_GapMoving = data
  
    def read_EngineeringLock(self, the_att):
        the_att.set_value(self.attr_EngineeringLock)
    def write_EngineeringLock(self, the_att):
        data = the_att.get_write_value()
        self.attr_EngineeringLock = data
  
    def read_Interlock(self, the_att):
        the_att.set_value(self.attr_Interlock)
    def write_Interlock(self, the_att):
        data = the_att.get_write_value()
        self.attr_Interlock = data
  
    def read_StopAll(self, the_att):
        the_att.set_value(self.attr_StopAll)
    def write_StopAll(self, the_att):
        data = the_att.get_write_value()
        self.attr_StopAll = data
  
    def read_EngineeringGapAcceleration(self, the_att):
        the_att.set_value(self.attr_EngineeringGapAcceleration)
    def write_EngineeringGapAcceleration(self, the_att):
        data = the_att.get_write_value()
        self.attr_EngineeringGapAcceleration = data
  
    def read_EngineeringGapSpeed(self, the_att):
        the_att.set_value(self.attr_EngineeringGapSpeed)
    def write_EngineeringGapSpeed(self, the_att):
        data = the_att.get_write_value()
        self.attr_EngineeringGapSpeed = data

    def read_EngineeringPhaseAcceleration(self, the_att):
        the_att.set_value(self.attr_EngineeringPhaseAcceleration)
    def write_EngineeringPhaseAcceleration(self, the_att):
        data = the_att.get_write_value()
        self.attr_EngineeringPhaseAcceleration = data
  
    def read_EngineeringPhaseSpeed(self, the_att):
        the_att.set_value(self.attr_EngineeringPhaseSpeed)
    def write_EngineeringPhaseSpeed(self, the_att):
        data = the_att.get_write_value()
        self.attr_EngineeringPhaseSpeed = data
  
    def read_GapAcceleration(self, the_att):
        the_att.set_value(self.attr_GapAcceleration)
    def write_GapAcceleration(self, the_att):
        data = the_att.get_write_value()
        self.attr_GapAcceleration = data
  
    def read_GapSpeed(self, the_att):
        the_att.set_value(self.attr_GapSpeed)
    def write_GapSpeed(self, the_att):
        data = the_att.get_write_value()
        self.attr_GapSpeed = data
  
    def read_Phase(self, the_att):
        the_att.set_value(self.attr_Phase)
    def write_Phase(self, the_att):
        data = the_att.get_write_value()
        self.attr_Phase = data
  
    def read_PhaseAcceleration(self, the_att):
        the_att.set_value(self.attr_PhaseAcceleration)
    def write_PhaseAcceleration(self, the_att):
        data = the_att.get_write_value()
        self.attr_PhaseAcceleration = data
  
    def read_PhaseSpeed(self, the_att):
        the_att.set_value(self.attr_PhaseSpeed)
    def write_PhaseSpeed(self, the_att):
        data = the_att.get_write_value()
        self.attr_PhaseSpeed = data
  
    def read_Taper(self, the_att):
        the_att.set_value(self.attr_Taper)
    def write_Taper(self, the_att):
        data = the_att.get_write_value()
        self.attr_Taper = data
  
    def read_TaperAcceleration(self, the_att):
        the_att.set_value(self.attr_TaperAcceleration)
    def write_TaperAcceleration(self, the_att):
        data = the_att.get_write_value()
        self.attr_TaperAcceleration = data
  
    def read_TaperSpeed(self, the_att):
        the_att.set_value(self.attr_TaperSpeed)
    def write_TaperSpeed(self, the_att):
        data = the_att.get_write_value()
        self.attr_TaperSpeed = data

    def read_AxesFlags(self, the_att):
        the_att.set_value(self.attr_AxesFlags)
    def write_AxesFlags(self, the_att):
        data = the_att.get_write_value()
        self.attr_AxesFlags = data
                   
    def read_PhaseMode(self, the_att):
        the_att.set_value(self.attr_PhaseMode)

    def write_PhaseMode(self, the_att):
        data = the_att.get_write_value()
        self.attr_PhaseMode = data
        
    def read_Gap(self, the_att):
        the_att.set_value(self.attr_Gap)

    def write_Gap(self, the_att):
        data = the_att.get_write_value()
        self.attr_Gap = data
        
    def read_Offset(self, the_att):
        the_att.set_value(self.attr_Offset)

    def write_Offset(self, the_att):
        data = the_att.get_write_value()
        self.attr_Offset = data
  
    def toggleStopAll(self, param):
        self.attr_StopAll=param

class FakeAxis(PyTango.Device_3Impl):
    def __init__(self,cl,name):
        PyTango.Device_3Impl.__init__(self,cl,name)
        self.debug_stream('In FakeAxis __init__')
        FakeAxis.init_device(self)

    def init_device(self):
        self.debug_stream('In Python init_device method')
        self.set_state(PyTango.DevState.OFF)
        self.attr_backwardLimitSwitch = False
        self.attr_forwardLimitSwitch = False
        self.attr_position = 0
        self.attr_offset = 0
        
    def delete_device(self):
        self.debug_stream('[delete_device] for device %s ' % self.get_name())

    def read_attr_hardware(self, data):
        self.debug_stream('In read_attr_hardware')

    def read_backwardLimitSwitch(self, the_att):
        the_att.set_value(self.attr_backwardLimitSwitch)

    def write_backwardLimitSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_backwardLimitSwitch = data
        
    def read_forwardLimitSwitch(self, the_att):
        the_att.set_value(self.attr_forwardLimitSwitch)

    def write_forwardLimitSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_forwardLimitSwitch = data
        
    def read_position(self, the_att):
        the_att.set_value(self.attr_position)

    def write_position(self, the_att):
        data = the_att.get_write_value()
        self.attr_position = data
  
    def read_offset(self, the_att):
        the_att.set_value(self.attr_offset)

    def write_offset(self, the_att):
        data = the_att.get_write_value()
        self.attr_offset = data
        
    def Off(self):
        self.set_state(PyTango.DevState.OFF)
    
    def On(self):
        self.set_state(PyTango.DevState.STANDBY)
  

class FakeGeared(PyTango.Device_3Impl):
    def __init__(self,cl,name):
        PyTango.Device_3Impl.__init__(self,cl,name)
        self.debug_stream('In FakeGeared __init__')
        FakeGeared.init_device(self)

    def init_device(self):
        self.debug_stream('In Python init_device method')
        self.set_state(PyTango.DevState.STANDBY)
        self.attr_isGeared = False
        self.attr_masterPosition = 0
        self.attr_slavePosition = 0
        
    def delete_device(self):
        self.debug_stream('[delete_device] for device %s ' % self.get_name())

    def read_attr_hardware(self, data):
        self.debug_stream('In read_attr_hardware')

    def read_isGeared(self, the_att):
        the_att.set_value(self.attr_isGeared)

    def write_isGeared(self, the_att):
        data = the_att.get_write_value()
        self.attr_isGeared = data

    def read_masterPosition(self, the_att):
        the_att.set_value(self.attr_masterPosition)

    def write_masterPosition(self, the_att):
        data = the_att.get_write_value()
        self.attr_masterPosition = data
        
    def read_slavePosition(self, the_att):
        the_att.set_value(self.attr_slavePosition)

    def write_slavePosition(self, the_att):
        data = the_att.get_write_value()
        self.attr_slavePosition = data

    def SetGearedMode(self):
        self.attr_isGeared = True
    
    def SetIndependantMode(self):
        self.attr_isGeared = False

#------------------------------------------------------------------
# CLASS
#------------------------------------------------------------------
class FakeUndulatorClass(PyTango.DeviceClass):

    def __init__(self, name):
        PyTango.DeviceClass.__init__(self, name)
        self.set_type("FakeUndulator")

    cmd_list = { 'toggleStopAll' : [ [PyTango.ArgType.DevBoolean, "newstate"], [PyTango.ArgType.DevVoid, "void"] ]
    }

    attr_list = { 'GapMoving' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'PhaseMoving' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],                 
                  'EngineeringLock' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],   
                  'Interlock' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],  
                  'StopAll' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],  
  
                  'PhaseMode' : [ [ PyTango.ArgType.DevLong,    
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
  
                  'Gap' : [ [ PyTango.ArgType.DevDouble,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                                    
                  'Offset' : [ [ PyTango.ArgType.DevDouble,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                                    
                  'EngineeringGapAcceleration' : [ [ PyTango.ArgType.DevDouble,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'EngineeringGapSpeed' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'EngineeringPhaseAcceleration' : [ [ PyTango.ArgType.DevDouble,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'EngineeringPhaseSpeed' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],                                    
                  'GapAcceleration' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'GapSpeed' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'Phase' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'PhaseAcceleration' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'PhaseSpeed' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'Taper' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'TaperAcceleration' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'TaperSpeed' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'AxesFlags' : [ [ PyTango.ArgType.DevULong,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ]
                }
                
                
#------------------------------------------------------------------
# CLASS
#------------------------------------------------------------------
class FakeAxisClass(PyTango.DeviceClass):

    def __init__(self, name):
        PyTango.DeviceClass.__init__(self, name)
        self.set_type("FakeAxis")

    cmd_list = {
        'On' : [ [PyTango.ArgType.DevVoid, ""], [PyTango.ArgType.DevVoid, ""] ],
        'Off' : [ [PyTango.ArgType.DevVoid, ""], [PyTango.ArgType.DevVoid, ""] ]
    }

    attr_list = { 'backwardLimitSwitch' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'forwardLimitSwitch' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],                 
                  'position' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'offset' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ]
                }
                

#------------------------------------------------------------------
# CLASS
#------------------------------------------------------------------
class FakeGearedClass(PyTango.DeviceClass):

    def __init__(self, name):
        PyTango.DeviceClass.__init__(self, name)
        self.set_type("FakeGeared")

    cmd_list = { 
        'SetGearedMode' : [ [PyTango.ArgType.DevVoid, ""], [PyTango.ArgType.DevVoid, ""] ],
        'SetIndependantMode' : [ [PyTango.ArgType.DevVoid, ""], [PyTango.ArgType.DevVoid, ""] ]
    }

    attr_list = { 'isGeared' : [ [ PyTango.ArgType.DevBoolean ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'masterPosition' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ],
                  'slavePosition' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE], {} ]
                }


if __name__ == '__main__':
    try:
        util = PyTango.Util(sys.argv)

        util.add_class(FakeUndulatorClass, FakeUndulator, 'FakeUndulator')
        util.add_class(FakeGearedClass, FakeGeared, 'FakeGeared')
        util.add_class(FakeAxisClass, FakeAxis, 'FakeAxis')
        
      
        U = PyTango.Util.instance()
        U.server_init()
        U.server_run()
    except PyTango.DevFailed,e:
        print '-------> Received a DevFailed exception:',e
    except Exception,e:
        print '-------> An unforeseen exception occured....',e

