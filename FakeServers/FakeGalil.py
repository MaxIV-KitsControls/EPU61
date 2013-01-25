#!/usr/bin/python

import socket
import sys
import re
import PyTango
from threading import Thread

class DevSim(object):
    cmds = [ 'XQ#GAP,7', 'XQ#PHASE,7', 'HX 7', 'ST S' ]
    devs = {}
    
    def __init__(self, name):
        self.encoders = { 'TDA' : 0.0,
                          'TDB' : 0.0,
                          'TDC' : 0.0,
                          'TDD' : 0.0 }

        self.state = {
          'coilG' : 100000.0,
          'coilC' : 0.0,
          'coilT' : 0.0,
          
          'coilPM' : 0.0,
          'coilPO' : 0.0,
          
          'StopGap' : 0.0,
          'StopPh' : 0.0,
          
          'GapMv'  : 0.0,
          '@OUT[5]' : 0.0,
          'MG @OUT[5]' : 1.0,

          'IlockAct' : 0.0,
          'StopRq'   : 0.0,
          
          'AxesFlgs' : 0.0,
          
          'Offsets[0]' : 0.0,
          'Offsets[1]' : 0.0,
          'Offsets[2]' : 0.0,
          'Offsets[3]' : 0.0,
          
          'UMOfsts[0]' : 0.0,
          'UMOfsts[1]' : 0.0,
          'UMOfsts[2]' : 0.0,
          'UMOfsts[3]' : 0.0,
          
          'PhDest[0]' : 0.0,
          'PhDest[1]' : 0.0,
          'PhDest[2]' : 0.0,
          'PhDest[3]' : 0.0,
          
          'UMOfsts[0]' : 0.0,
          'UMOfsts[1]' : 0.0,
          'UMOfsts[2]' : 0.0,
          'UMOfsts[3]' : 0.0,
          
          'OffInit' : 0.0,
          
          'PhMdRq' : 0.0,
          'PhSpd' : 0.0,
          'PhAcc' : 0.0,
          'PhCmd' : 0.0,
          
          'GapOfst' : 0.0,
          'Gap' : 0.0,
          'Taper' : 0.0,
          'TprSpd': 0.0,
          'GapSpd': 0.0,
          'GapAcc': 0.0,
          'TprAcc': 0.0          
        }
    
        DevSim.devs[name]=self

    def procRequest(self, data):
        response = ''
        data=data.replace('\n',';')
        data=data.replace('\r',';')


        for token in re.split(';', data):
           
            if token=='qt':
                # This will quit the loop
                response='qt'
                return response
            elif token=='':
                # Skip empty
                pass
            elif token in DevSim.cmds:
                # Check if it is a commnad
                # Indicate that we have processed this command
                response+=':'
            elif token in self.encoders:
                # Check if it is encoder readout req.
                response+=str(self.encoders[token]) + '\r\n:'
            else:
                eqPos=token.find('=')
                
                if (eqPos==-1):
                    if token=='MG @OUT[5]':
                        response+=str(self.state[token]) + '\r\n:'
                    else:
                        # indiacate invalid command
                        response+='?'
                else:
                    beforeEq=token[:eqPos]
                    afterEq=token[eqPos+1:]
                    
                    if beforeEq in self.state:
                        if afterEq=='?':
                            # User queries for value             
                            response+=str(self.state[beforeEq]) + '\r\n:'
                        else:
                            # User sets value. Cast to appropriate type
                            self.state[beforeEq] = type(self.state[beforeEq])(afterEq)
                            response+=':'
                    else:
                        # indiacate invalid command
                        response+='?'
                                
        return response

addr = 'localhost'
port = 6000
tcpThreadRun = True

def listenThreadFunc():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    global addr
    global port
    global tcpThreadRun
    
    dev = DevSim('TCP')
    
    sa = (addr, port)
    print 'starting up on %s port %s' % sa

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(sa)

    s.listen(1)

    tcpThreadRun = True
    while tcpThreadRun:
        conn, ca = s.accept()
        try:
              while tcpThreadRun:
                  data = conn.recv(512)
                  
                  if data:
                      response = dev.procRequest(data)
                      
                      if response == 'qt':
                          tcpThreadRun = False
                          break
                      else:
                          conn.sendall(response)
                  else:
                      break
        finally:
            conn.close()

tcpThread = Thread(target=listenThreadFunc, args=())

def startTcp():
    global tcpThread
    tcpThread.start()

def stopTcp():
    global tcpThread
    global tcpThreadRun
    tcpThreadRun = False
    tcpThread.join()
    

class FakeControlBox(PyTango.Device_3Impl):
    def __init__(self,cl,name):
        PyTango.Device_3Impl.__init__(self,cl,name)
        self.debug_stream('In FakeControlBox __init__')
        FakeControlBox.init_device(self)
        self.dev = DevSim(name)


    def init_device(self):
        self.debug_stream('In Python init_device method')
        self.set_state(PyTango.DevState.STANDBY)
    
    def ExecLowLevelCmd(self, param):
        return self.dev.procRequest(param)

class FakeControlBoxClass(PyTango.DeviceClass):

    def __init__(self, name):
        PyTango.DeviceClass.__init__(self, name)
        self.set_type("FakeControlBox")

    cmd_list = {
        'ExecLowLevelCmd' : [ [PyTango.ArgType.DevString, ""], [PyTango.ArgType.DevString, ""] ]
    }

    attr_list = {
                }
 
def dsThreadFunc():
    try:
        util = PyTango.Util(['FakeGalil', 'sim'])

        util.add_class(FakeControlBoxClass, FakeControlBox, 'FakeControlBox')
              
        U = PyTango.Util.instance()
        U.server_init()
        U.server_run()
    except PyTango.DevFailed,e:
        print '-------> Received a DevFailed exception:',e
    except Exception,e:
        print '-------> An unforeseen exception occured....',e


dsThread = Thread(target=dsThreadFunc, args=())

def startDS():
    global dsThread
    dsThread.start()
    
def stopDS():
    global dsThread
    dsThread.join()


