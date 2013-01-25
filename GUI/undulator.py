#!/usr/bin/python

import PyTango
import taurus
import time
import math
import sys
import traceback
from PyQt4 import QtGui,QtCore
from PyQt4.QtCore import QObject, pyqtSignal

# Listenes for phase-mode changes and updates ui appropriatly
# GUI is updated via signal, not to change QT Widgets from non-main (listener) thread 
def phaseModeListener(src, evt_type, attr_val):
    if isinstance(src,taurus.core.tango.tangoattribute.TangoAttribute) and evt_type==PyTango.EventType.CHANGE_EVENT:
        #print 'phaseModeListener ' + str(evt_type)
        pmNum = attr_val.value
        
        if pmNum == 0:
            pmName = "Phase A"          
        elif pmNum == 1:
            pmName = "Phase B"          
        elif pmNum == 2:
            pmName = "Phase C"           
        elif pmNum == 3:
            pmName = "Phase D"
        else:
            pmName = "WRONG PHASE"    
        
        Main.updatePhaseMode.emit(pmName)
        

# Updates gap attribute limits on UI based on changes
# GUI is updated via signal, not to change QT Widgets from non-main (listener) thread 
def gapConfigListener(src, evt_type, attr_val):
    #print 'gapConfigListener'
    if evt_type==PyTango.EventType.QUALITY_EVENT:
        Main.updateGapLimits.emit(float(src.getMinValue()),float(src.getMaxValue()))
        
# Updates phase attribute limits on UI based on changes
# GUI is updated via signal, not to change QT Widgets from non-main (listener) thread 
def phaseConfigListener(src, evt_type, attr_val):
    #print 'phaseConfigListener'
    if evt_type==PyTango.EventType.QUALITY_EVENT:
        Main.updatePhaseLimits.emit(float(src.getMaxValue()))
        
# Monitors stopAll attribute status on the undulator device and appropriately updates GUI
# GUI is updated via signal, not to change QT Widgets from non-main (listener) thread 
def stopAllListener(src, evt_type, attr_val):
    #print 'stopAllListener'
    # sometimes this comes as TangoConfiuration type on initialization and causes errors
    # hence the check
    if isinstance(src,taurus.core.tango.tangoattribute.TangoAttribute) and evt_type==PyTango.EventType.CHANGE_EVENT:
        Main.updateStopBtn.emit(attr_val.value)

# Updates LEDs for axis flags
def axesFlagsListener(src, evt_type, attr_val):
    if isinstance(src,taurus.core.tango.tangoattribute.TangoAttribute) and evt_type==PyTango.EventType.CHANGE_EVENT:
        #print 'axesFlagsListener'
        Main.updateAxesFlags.emit(attr_val.value)

# Set max gap configuration     
def setMaxGap(value):
    gapConfig = taurus.Attribute('$undulator/Gap').getConfig()
    
    if gapConfig.getMaxValue() != value:
        gapConfig.setLimits(gapConfig.getMinValue(),value)    
        
# Set min gap configuration
def setMinGap(value):
    gapConfig = taurus.Attribute('$undulator/Gap').getConfig()
    
    if gapConfig.getMinValue()  != value:
        gapConfig.setLimits(value, gapConfig.getMaxValue())    

# Set max phase configuration   
def setMaxPhase(value):
    phaseConfig = taurus.Attribute('$undulator/Phase').getConfig()
    
    if (phaseConfig.getMaxValue() != value) or (phaseConfig.getMinValue() != -value) :
        phaseConfig.setLimits(-value,value)
        
# Sets phase mode
def updatePhaseMode(phaseMode):
    try:
        #Write phase number to device server        
        taurus.Attribute('$undulator/PhaseMode').write(phaseMode)
    except:
        pass
        
# Enables\Disables gearing on lower girder
# geared = 1 Enable gearing
# geared != 1 Disable gearing
def gearUp(geared):
    try:
        if geared == 1:
            #Enable gearing
            taurus.Device('$gearUp').SetGearedMode()
        else:
            #Disable gearing
            taurus.Device('$gearUp').SetIndependantMode()
    except: 
        pass

# Enables\Disables gearing on lower girder
# geared = 1 Enable gearing
# geared != 1 Disable gearing
def gearDown(geared):
    try:
        if geared == 1:
            #Enable gearing
            taurus.Device('$gearLow').SetGearedMode()
        else:
            #Disable gearing
            taurus.Device('$gearLow').SetIndependantMode()
    except: 
        print "Lower gearing problem. Can not set gearing to: " + str(geared)
         
        
# Toggles stop all state via button
def handleStopAllBtn():
    und = taurus.Device('$undulator')
    
    if und.StopAll:
        und.toggleStopAll(False)
    else:
        und.toggleStopAll(True)

# Temporary -> Enables motors
def motorsOn():
    try:
        taurus.Device('$gapz1').On()
        taurus.Device('$gapz2').On()
        taurus.Device('$gapz3').On()
        taurus.Device('$gapz4').On()
        taurus.Device('$phasex1').On()
        taurus.Device('$phasex2').On()
        taurus.Device('$phasex3').On()
        taurus.Device('$phasex4').On()
    except: 
        print "Error sending motorOn commands."

# Temporary -> Disable motors
def motorsOff():
    try:
        taurus.Device('$gapz1').Off()
        taurus.Device('$gapz2').Off()
        taurus.Device('$gapz3').Off()
        taurus.Device('$gapz4').Off()
        taurus.Device('$phasex1').Off()
        taurus.Device('$phasex2').Off()
        taurus.Device('$phasex3').Off()
        taurus.Device('$phasex4').Off()
    except: 
        print "Error sending motorOn commands."

def resetDrive(drvNo):
    try:
        taurus.Device('$undulator').ResetDrive(drvNo)
    except:
        print "Error reseting the drive."

# Engineering movement
# link - Tango device string to axis
# refPos - Desired movement
# mPos - Current motor position
# suffix - Suffix text in motor position (to remove)    
def goCommand(link,refPos, mPos, suffix):
    # Extract value for current motor position. 
    mPos = float(str(mPos).replace(str(suffix),''))     
    try:                
        #Calculate new position     
        pos = mPos + float(refPos)      
        #Send motor to new position
        
        taurus.Attribute(link).write(pos)
    except:
        print "Problem moving " + str(link)
        

# Sends calibrate gap command with center and gap parameters.       
def gapToRef(gap, center):
    try:
        # Set up array
        params=array('d',[float(gap),float(center)])  
        # Send command with array as parameter  
        taurus.Device('$undulator').CalibrateGap(params)  
    except RuntimeError as e:
        print "Problem with gap to reference command." + e
        

def connections(ui):
    # Connect widget signals used
    QtCore.QObject.connect(ui.cmbGearUpper, QtCore.SIGNAL("currentIndexChanged(int)"), gearUp)        
    QtCore.QObject.connect(ui.cmbGearLower, QtCore.SIGNAL("currentIndexChanged(int)"), gearDown)    
    QtCore.QObject.connect(ui.btnGapRef, QtCore.SIGNAL("clicked()"), lambda: gapToRef(ui.vspGapRef.text(), ui.vspCenterRef.text()))
    QtCore.QObject.connect(ui.cmbPhase, QtCore.SIGNAL("currentIndexChanged(int)"), updatePhaseMode)
    QtCore.QObject.connect(ui.btnStopAll, QtCore.SIGNAL("clicked()"), handleStopAllBtn)
    QtCore.QObject.connect(ui.btnGoA, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosA.getModel(), ui.refPosA.value(), ui.mPosA.text(), ui.mPosA.getSuffixText()))
    QtCore.QObject.connect(ui.btnGoB, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosB.getModel(), ui.refPosB.value(), ui.mPosB.text(), ui.mPosB.getSuffixText()))
    QtCore.QObject.connect(ui.btnGoC, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosC.getModel(), ui.refPosC.value(), ui.mPosC.text(), ui.mPosC.getSuffixText()))
    QtCore.QObject.connect(ui.btnGoD, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosD.getModel(), ui.refPosD.value(), ui.mPosD.text(), ui.mPosD.getSuffixText()))
    QtCore.QObject.connect(ui.btnGoE, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosE.getModel(), ui.refPosE.value(), ui.mPosE.text(), ui.mPosE.getSuffixText()))
    QtCore.QObject.connect(ui.btnGoF, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosF.getModel(), ui.refPosF.value(), ui.mPosF.text(), ui.mPosF.getSuffixText()))
    QtCore.QObject.connect(ui.btnGoG, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosG.getModel(), ui.refPosG.value(), ui.mPosG.text(), ui.mPosG.getSuffixText()))
    QtCore.QObject.connect(ui.btnGoH, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosH.getModel(), ui.refPosH.value(), ui.mPosH.text(), ui.mPosH.getSuffixText()))
    
    # Connect the reset drive commands. Can not use TaurusCommandButton because Parameters property is broken when compiled from UI
    QtCore.QObject.connect(ui.pushButtonRst_1, QtCore.SIGNAL("clicked()"), lambda: resetDrive(1))
    QtCore.QObject.connect(ui.pushButtonRst_2, QtCore.SIGNAL("clicked()"), lambda: resetDrive(2))
    QtCore.QObject.connect(ui.pushButtonRst_3, QtCore.SIGNAL("clicked()"), lambda: resetDrive(3))
    QtCore.QObject.connect(ui.pushButtonRst_4, QtCore.SIGNAL("clicked()"), lambda: resetDrive(4))
    QtCore.QObject.connect(ui.pushButtonRst_5, QtCore.SIGNAL("clicked()"), lambda: resetDrive(5))
    QtCore.QObject.connect(ui.pushButtonRst_6, QtCore.SIGNAL("clicked()"), lambda: resetDrive(6))
    QtCore.QObject.connect(ui.pushButtonRst_7, QtCore.SIGNAL("clicked()"), lambda: resetDrive(7))
    QtCore.QObject.connect(ui.pushButtonRst_8, QtCore.SIGNAL("clicked()"), lambda: resetDrive(8))

    # Update gap & phase limit value edits
    gapCfg = taurus.Attribute('$undulator/Gap').getConfig()
    phaseCfg = taurus.Attribute('$undulator/Phase').getConfig()
    
    gapMin = float(gapCfg.getMinValue())
    gapMax = float(gapCfg.getMaxValue())
    phaseMax = float(phaseCfg.getMaxValue())

    ui.setMinGap.setValue(gapMin)
    ui.setMaxGap.setValue(gapMax)
    ui.setMaxPhase.setValue(phaseMax)
    
    # Hack. Reinitialize limits once to stop loop of QUALITY events
    gapCfg.setLimits(gapMin,gapMax)
    phaseCfg.setLimits(-phaseMax,phaseMax)  
    
    # Initialize the value of the spin boxes
    ui.vsbGap.setValue(float(taurus.Device('$undulator').Gap))
    ui.vsbPhaseOffset.setValue(float(taurus.Device('$undulator').Phase))
    
    QtCore.QObject.connect(ui.setMaxGap, QtCore.SIGNAL("valueChanged(int)"), setMaxGap)
    QtCore.QObject.connect(ui.setMinGap, QtCore.SIGNAL("valueChanged(int)"), setMinGap)
    QtCore.QObject.connect(ui.setMaxPhase, QtCore.SIGNAL("valueChanged(int)"), setMaxPhase)
    QtCore.QObject.connect(ui.motorsOnButton, QtCore.SIGNAL("clicked()"),  motorsOn)
    QtCore.QObject.connect(ui.motorsOffButton, QtCore.SIGNAL("clicked()"), motorsOff)
    
    # Add event listeners to monitor relevant Taurus attribute changes
    taurus.Attribute('$undulator/PhaseMode').addListener(phaseModeListener)
    taurus.Attribute('$undulator/StopAll').addListener(stopAllListener)
    taurus.Attribute('$undulator/Gap').addListener(gapConfigListener)
    taurus.Attribute('$undulator/Phase').addListener(phaseConfigListener)
    taurus.Attribute('$undulator/AxesFlags').addListener(axesFlagsListener)
    
    # Hack, upon server restart the abs_change seems to restart to 0 in one direction
    # hence the events are fired all the time and this slows down the GUI
    # Here we bring them back on normal value.   
    taurus.Attribute('$undulator/PhaseMode').getConfig().setParam('abs_change','1')
    taurus.Attribute('$undulator/StopAll').getConfig().setParam('abs_change','1')
    taurus.Attribute('$undulator/AxesFlags').getConfig().setParam('abs_change','1')
    taurus.Attribute('$undulator/Gap').getConfig().setParam('abs_change','5')
    taurus.Attribute('$undulator/Phase').getConfig().setParam('abs_change','5')
    
    
    
# Class representing the main UI window
# contains signals and methods that implement UI updates
class UndulatorWindow(QtGui.QMainWindow):
    # These are used to signal UI changes from non-main (non-UI) threads
    # It is not safe to update QT Widgets from non-UI threads.
    updateStopBtn = pyqtSignal(bool,name='updateStopBtn')
    updateGapLimits = pyqtSignal(float,float,name='updateGapLimits')
    updatePhaseLimits = pyqtSignal(float,name='updatePhaseLimits')
    updatePhaseMode = pyqtSignal(str,name='updatePhaseMode')
    updateAxesFlags = pyqtSignal(int,name='updateAxesFlags')
    
    
    def __init__(self, ui):
        super(QtGui.QMainWindow, self).__init__()        
        self.ui  = ui
        
        try:
            self.ui.setupUi(self)
            # Connect signals to our handlers
            self.updateStopBtn.connect(self.handle_updateStopBtn)      
            self.updateGapLimits.connect(self.handle_updateGapLimits)
            self.updatePhaseLimits.connect(self.handle_updatePhaseLimits)
            self.updatePhaseMode.connect(self.handle_updatePhaseMode)
            self.updateAxesFlags.connect(self.handle_updateAxesFlags)

            # Initialize UI state
            self.ui.cmbPhase.setCurrentIndex(int(taurus.Device('$undulator').PhaseMode))
            self.ui.cmbGearUpper.setCurrentIndex(int(taurus.Device('$gearUp').isGeared))
            self.ui.cmbGearLower.setCurrentIndex(int(taurus.Device('$gearLow').isGeared))
            self.ui.tabMain.currentChanged.connect(self.handle_tabChange)
            self.handle_updateAxesFlags(taurus.Device('$undulator').AxesFlags)

            # Hide motors group-box
            self.ui.groupBoxMotors.setVisible(False)
            
        except Exception as e:
            QtGui.QMessageBox.critical(self, 'ID GUI', 'Failed to initialize the ID GUI, please make sure the Tango devices are running and are configured properly.\n\n' + str(traceback.format_exception(*sys.exc_info())))
            exit(1)
            
    def handle_updateStopBtn(self,arg):
        if arg:
            self.ui.btnStopAll.setStyleSheet('color:green')
            text = 'Enable All'
        else:
            self.ui.btnStopAll.setStyleSheet('color:red')
            text = 'Stop All'
          
        ui.btnStopAll.setText(text)
    
    def handle_updateGapLimits(self, gapMin, gapMax):
        self.ui.labelMinGap.setText(str(gapMin))
        self.ui.labelMaxGap.setText(str(gapMax))  
    
    def handle_updatePhaseLimits(self, phaseMax):
        self.ui.labelMaxPhase.setText(str(phaseMax))
        
    def handle_updatePhaseMode(self, phaseMode):
        self.ui.phaseMode.setText(phaseMode)  
        
    def handle_tabChange(self, tabNo):
        #print 'handle_tabChange'
        self.ui.groupBoxMotors.setVisible(tabNo==1)
        
    def handle_updateAxesFlags(self, flags):
        self.ui.qLedCommZ1.ledStatus = (flags & (1<<0) ) != 0
        self.ui.qLedCommZ2.ledStatus = (flags & (1<<1) ) != 0
        self.ui.qLedCommZ3.ledStatus = (flags & (1<<2) ) != 0
        self.ui.qLedCommZ4.ledStatus = (flags & (1<<3) ) != 0
        self.ui.qLedCommX1.ledStatus = (flags & (1<<4) ) != 0
        self.ui.qLedCommX2.ledStatus = (flags & (1<<5) ) != 0
        self.ui.qLedCommX3.ledStatus = (flags & (1<<6) ) != 0
        self.ui.qLedCommX4.ledStatus = (flags & (1<<7) ) != 0
        
        self.ui.qLedEncZ1.ledStatus = (flags & (1<<8)  ) != 0
        self.ui.qLedEncZ2.ledStatus = (flags & (1<<9)  ) != 0
        self.ui.qLedEncZ3.ledStatus = (flags & (1<<10) ) != 0
        self.ui.qLedEncZ4.ledStatus = (flags & (1<<11) ) != 0
        self.ui.qLedEncX1.ledStatus = (flags & (1<<12) ) != 0
        self.ui.qLedEncX2.ledStatus = (flags & (1<<13) ) != 0
        self.ui.qLedEncX3.ledStatus = (flags & (1<<14) ) != 0
        self.ui.qLedEncX4.ledStatus = (flags & (1<<15) ) != 0

        self.ui.qLedDrvZ1.ledStatus = (flags & (1<<16) ) != 0
        self.ui.qLedDrvZ2.ledStatus = (flags & (1<<17) ) != 0
        self.ui.qLedDrvZ3.ledStatus = (flags & (1<<18) ) != 0
        self.ui.qLedDrvZ4.ledStatus = (flags & (1<<19) ) != 0
        self.ui.qLedDrvX1.ledStatus = (flags & (1<<20) ) != 0
        self.ui.qLedDrvX2.ledStatus = (flags & (1<<21) ) != 0
        self.ui.qLedDrvX3.ledStatus = (flags & (1<<22) ) != 0
        self.ui.qLedDrvX4.ledStatus = (flags & (1<<23) ) != 0        
        
if __name__ == "__main__":
    import sys
    import undulatorDesigner
    from array import * 
    import taurus.core.util.argparse as argparse
    
    parser, options, args = argparse.init_taurus_args()
    
    app = QtGui.QApplication(sys.argv)
    ui = undulatorDesigner.Ui_Main()
    Main = UndulatorWindow(ui)
    connections(ui)   

    # Show GUI
    Main.show()
    sys.exit(app.exec_())
    

