#!/usr/bin/python

import PyTango
import sys

class SafetyPLC(PyTango.Device_3Impl):
    def __init__(self,cl,name):
        PyTango.Device_3Impl.__init__(self,cl,name)
        self.debug_stream('In SafetyPLC __init__')
        SafetyPLC.init_device(self)

    def init_device(self):
        self.debug_stream('In Python init_device method')
        self.set_state(PyTango.DevState.ON)
        
        self.attr_emergencyStop       = 0
        self.attr_tiltLowerGirder     = 0
        self.attr_tiltUpperGirder     = 0
        self.attr_z1BackwardKillSwitch   = 1
        self.attr_z1ForwardKillSwitch    = 1
        self.attr_z2BackwardKillSwitch   = 1
        self.attr_z2ForwardKillSwitch    = 1
        self.attr_z3BackwardKillSwitch   = 1
        self.attr_z3ForwardKillSwitch    = 1
        self.attr_z4BackwardKillSwitch   = 1
        self.attr_z4ForwardKillSwitch    = 1
        self.attr_x1BackwardKillSwitch   = 1
        self.attr_x1ForwardKillSwitch    = 1
        self.attr_x2BackwardKillSwitch   = 1
        self.attr_x2ForwardKillSwitch    = 1
        self.attr_x3BackwardKillSwitch   = 1
        self.attr_x3ForwardKillSwitch    = 1
        self.attr_x4BackwardKillSwitch   = 1
        self.attr_x4ForwardKillSwitch    = 1
        self.attr_interlockOverride      = 0
        self.attr_Current1 = 0
        self.attr_Current2 = 0
        self.attr_Current3 = 0
        self.attr_Current4 = 0
        
    def delete_device(self):
        self.debug_stream('[delete_device] for device %s ' % self.get_name())

    def read_attr_hardware(self, data):
        self.debug_stream('In read_attr_hardware')

    def read_emergencyStop(self, the_att):
        the_att.set_value(self.attr_emergencyStop)
    def write_emergencyStop(self, the_att):
        data = the_att.get_write_value()
        self.attr_emergencyStop = data
    
    def read_tiltLowerGirder(self, the_att):
        the_att.set_value(self.attr_tiltLowerGirder)
    def write_tiltLowerGirder(self, the_att):
        data = the_att.get_write_value()
        self.attr_tiltLowerGirder = data
        
    def read_tiltUpperGirder(self, the_att):
        the_att.set_value(self.attr_tiltUpperGirder)
    def write_tiltUpperGirder(self, the_att):
        data = the_att.get_write_value()
        self.attr_tiltUpperGirder = data

    def read_z1BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z1BackwardKillSwitch)
    def write_z1BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z1BackwardKillSwitch = data

    def read_z1ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z1ForwardKillSwitch)
    def write_z1ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z1ForwardKillSwitch = data

    def read_z2BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z2BackwardKillSwitch)
    def write_z2BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z2BackwardKillSwitch = data

    def read_z2ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z2ForwardKillSwitch)
    def write_z2ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z2ForwardKillSwitch = data

    def read_z3BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z3BackwardKillSwitch)
    def write_z3BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z3BackwardKillSwitch = data

    def read_z3ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z3ForwardKillSwitch)
    def write_z3ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z3ForwardKillSwitch = data

    def read_z4ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z4ForwardKillSwitch)
    def write_z4ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z4ForwardKillSwitch = data

    def read_z4BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_z4BackwardKillSwitch)
    def write_z4BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_z4BackwardKillSwitch = data

    def read_x1BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x1BackwardKillSwitch)
    def write_x1BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x1BackwardKillSwitch = data

    def read_x1ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x1ForwardKillSwitch)
    def write_x1ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x1ForwardKillSwitch = data

    def read_x2ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x2ForwardKillSwitch)
    def write_x2ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x2ForwardKillSwitch = data
        
    def read_x2BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x2BackwardKillSwitch)
    def write_x2BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x2BackwardKillSwitch = data

    def read_x3BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x3BackwardKillSwitch)
    def write_x3BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x3BackwardKillSwitch = data

    def read_x3ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x3ForwardKillSwitch)
    def write_x3ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x3ForwardKillSwitch = data

    def read_x4ForwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x4ForwardKillSwitch)
    def write_x4ForwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x4ForwardKillSwitch = data

    def read_x4BackwardKillSwitch(self, the_att):
        the_att.set_value(self.attr_x4BackwardKillSwitch)
    def write_x4BackwardKillSwitch(self, the_att):
        data = the_att.get_write_value()
        self.attr_x4BackwardKillSwitch = data

    def read_interlockOverride(self, the_att):
        the_att.set_value(self.attr_interlockOverride)
    def write_interlockOverride(self, the_att):
        data = the_att.get_write_value()
        self.attr_interlockOverride = data
        
    def read_Current1(self, the_att):
        the_att.set_value(self.attr_Current1)
    def write_Current1(self, the_att):
        data = the_att.get_write_value()
        self.attr_Current1 = data

    def read_Current2(self, the_att):
        the_att.set_value(self.attr_Current2)
    def write_Current2(self, the_att):
        data = the_att.get_write_value()
        self.attr_Current2 = data

    def read_Current3(self, the_att):
        the_att.set_value(self.attr_Current3)
    def write_Current3(self, the_att):
        data = the_att.get_write_value()
        self.attr_Current3 = data

    def read_Current4(self, the_att):
        the_att.set_value(self.attr_Current4)
    def write_Current4(self, the_att):
        data = the_att.get_write_value()
        self.attr_Current4 = data


#------------------------------------------------------------------
# CLASS
#------------------------------------------------------------------
class SafetyPLCClass(PyTango.DeviceClass):

    def __init__(self, name):
        PyTango.DeviceClass.__init__(self, name)
        self.set_type("SafetyPLC")

    cmd_list = { 
    }

    attr_list = { 
    
                    'emergencyStop' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'tiltLowerGirder' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'tiltUpperGirder' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z1BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z1ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z2BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z2ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z3BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z3ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z4BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'z4ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x1BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x1ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x2BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x2ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x3BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x3ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x4BackwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'x4ForwardKillSwitch' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'interlockOverride' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'Current1' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'Current2' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'Current3' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ],
                    'Current4' : [ [ PyTango.ArgType.DevDouble ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  {  } ]
    }


if __name__ == '__main__':
    try:
        util = PyTango.Util(sys.argv)

        util.add_class(SafetyPLCClass, SafetyPLC, 'SafetyPLC')

        U = PyTango.Util.instance()
        U.server_init()
        U.server_run()
    except PyTango.DevFailed,e:
        print '-------> Received a DevFailed exception:',e
    except Exception,e:
        print '-------> An unforeseen exception occured....',e
