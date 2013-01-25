#!/usr/bin/python

import PyTango
import sys

class FakeCoilSupply(PyTango.Device_3Impl):
    def __init__(self,cl,name):
        PyTango.Device_3Impl.__init__(self,cl,name)
        self.debug_stream('In FakeCoilSupply __init__')
        FakeCoilSupply.init_device(self)

    def init_device(self):
        self.debug_stream('In Python init_device method')
        self.set_state(PyTango.DevState.ON)
        self.attr_Coil1 = 0
        self.attr_Coil2 = 0
        self.attr_Coil3 = 0
        self.attr_Coil4 = 0
        
    def delete_device(self):
        self.debug_stream('[delete_device] for device %s ' % self.get_name())

    def read_attr_hardware(self, data):
        self.debug_stream('In read_attr_hardware')

    def read_Coil1(self, the_att):
        the_att.set_value(self.attr_Coil1)

    def write_Coil1(self, the_att):
        data = the_att.get_write_value()
        self.attr_Coil1 = data

    def read_Coil2(self, the_att):
        the_att.set_value(self.attr_Coil2)

    def write_Coil2(self, the_att):
        data = the_att.get_write_value()
        self.attr_Coil2 = data
        
    def read_Coil3(self, the_att):
        the_att.set_value(self.attr_Coil3)

    def write_Coil3(self, the_att):
        data = the_att.get_write_value()
        self.attr_Coil3 = data

    def read_Coil4(self, the_att):
        the_att.set_value(self.attr_Coil4)

    def write_Coil4(self, the_att):
        data = the_att.get_write_value()
        self.attr_Coil4 = data


#------------------------------------------------------------------
# CLASS
#------------------------------------------------------------------
class FakeCoilSupplyClass(PyTango.DeviceClass):

    def __init__(self, name):
        PyTango.DeviceClass.__init__(self, name)
        self.set_type("FakeCoilSupply")

    cmd_list = { 
    }

    attr_list = { 'Coil1' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  { 'min alarm' : -10000 , 'max alarm' : 10000 } ],
				  'Coil2' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  { 'min alarm' : -10000 , 'max alarm' : 10000 } ],
				  'Coil3' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  { 'min alarm' : -10000 , 'max alarm' : 10000 } ],
				  'Coil4' : [ [ PyTango.ArgType.DevLong ,
                                    PyTango.AttrDataFormat.SCALAR ,
                                    PyTango.AttrWriteType.READ_WRITE],
                                  { 'min alarm' : -10000 , 'max alarm' : 10000 } ]
    }


if __name__ == '__main__':
    try:
        util = PyTango.Util(sys.argv)

        util.add_class(FakeCoilSupplyClass, FakeCoilSupply, 'FakeCoilSupply')

        U = PyTango.Util.instance()
        U.server_init()
        U.server_run()
    except PyTango.DevFailed,e:
        print '-------> Received a DevFailed exception:',e
    except Exception,e:
        print '-------> An unforeseen exception occured....',e

