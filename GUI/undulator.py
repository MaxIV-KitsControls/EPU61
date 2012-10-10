import PyTango
import time
from PyQt4 import QtGui,QtCore

class PhaseModeCallback():
	# Reference to user interface
	def __init__(self, ui):
		self.ui = ui
	# Push event for subscription callback
	def push_event(self, event):
		if not event.err:
			# Update textbox with phase mode name.			
			self.ui.phaseMode.setText(self.phaseToString(event.attr_value.value))		
		else:
			print event.errors
			
	# Parses phase mode number to string	
	def phaseToString(self, number):
		if number == 0:
			name = "Phase A"          
		elif number == 1:
			name = "Phase B"          
		elif number == 2:
			name = "Phase C"           
		elif number == 3:
			name = "Phase D"
		else:
			name = "WRONG PHASE"            
		return name
			
class GapConfigCallback():
	# Reference to user interface
	def __init__(self, ui):
		self.ui = ui
	# Push event for subscription callback	
	def push_event(self, event):
		if not event.err:			
			#Update gap min max configuration lcds			
			self.ui.lcdMinGap.display(event.attr_conf.min_value)
			self.ui.lcdMaxGap.display(event.attr_conf.max_value)	
			
		else:
			print event.errors
			
class PhaseConfigCallback():
	# Reference to user interface
	def __init__(self, ui):
		self.ui = ui
	# Push event for subscription callback	
	def push_event(self, event):
		if not event.err:			
			self.ui.lcdMaxPhase.display(event.attr_conf.max_value)		
		else:
			print event.errors


# Inicialization
def init(ui):	
	# Set phase mode combobox to current phase mode.
	ui.cmbPhase.setCurrentIndex(int(PyTango.AttributeProxy('$undulator/PhaseMode').read().value))
	ui.cmbGearUpper.setCurrentIndex(int(PyTango.AttributeProxy('$gearUp/isGeared').read().value))
	ui.cmbGearLower.setCurrentIndex(int(PyTango.AttributeProxy('$gearLow/isGeared').read().value))
	
		
# Set max gap configuration		
def setMaxGap(value):
	gapConfig = PyTango.AttributeProxy('$undulator/Gap').get_config()
	if 	gapConfig.max_value != value:
		gapConfig.max_value = str(value)	
		PyTango.AttributeProxy('$undulator/Gap').set_config(gapConfig)		

# Set min gap configuration
def setMinGap(value):
	gapConfig = PyTango.AttributeProxy('$undulator/Gap').get_config()
	if 	gapConfig.min_value != value:
		gapConfig.min_value = str(value)	
		PyTango.AttributeProxy('$undulator/Gap').set_config(gapConfig)

# Set max phase configuration	
def setMaxPhase(value):
	phaseConfig = PyTango.AttributeProxy('$undulator/Phase').get_config()	
	if (phaseConfig.max_value != value) or (phaseConfig.min_value != -value) :
		phaseConfig.min_value = str(-value)
		phaseConfig.max_value = str(value)	
		PyTango.AttributeProxy('$undulator/Phase').set_config(gapConfig)
		
# Sets phase mode
def updatePhaseMode(phaseMode):
    try:
		#Write phase number to device server		
		PyTango.AttributeProxy('$undulator/PhaseMode').write(phaseMode)
    except:
		pass
		
# Enables\Disables gearing on lower girder
# geared = 1 Enable gearing
# geared != 1 Disable gearing
def gearUp(geared):
    try:
		if geared == 1:
			#Enable gearing
			PyTango.DeviceProxy('$gearUp').command_inout('SetGearedMode')
		else:
			#Disable gearing
			PyTango.DeviceProxy('$gearUp').command_inout('SetIndependantMode')
    except: 
        pass

# Enables\Disables gearing on lower girder
# geared = 1 Enable gearing
# geared != 1 Disable gearing
def gearDown(geared):
    try:
		if geared == 1:
			#Enable gearing
			PyTango.DeviceProxy('$gearLow').command_inout('SetGearedMode')
		else:
			#Disable gearing
			PyTango.DeviceProxy('$gearLow').command_inout('SetIndependantMode')
    except: 
		print "Lower gearing problem. Can not set gearing to: " + str(geared)
		
        
# Toggles stop all state
# decision = true  Enable stop all
# decision = false Disable stop all
def stopAll(decision):	
	try:
		if decision == True:
			PyTango.DeviceProxy('$undulator').command_inout('ToggleStopAll', True)
		else:
			PyTango.DeviceProxy('$undulator').command_inout('ToggleStopAll', False)
	except:
		print "Error sending stop all command. " + str(decision)
		
		
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
		PyTango.AttributeProxy(link).write(pos)
	except:
		print "Problem moving " + str(link)
		

# Sends calibrate gap command with center and gap parameters.		
def gapToRef(gap, center):
	
	try:
		# Set up array
		params=array('d',[gap,center])	
		# Send command with array as parameter	
		PyTango.DeviceProxy('$undulator').command_inout("CalibrateGap",params)	
	except:
		print "Problem with gap to reference command."
		
# Enable or Disable engineering lock
def lock(locked):
	try:
		PyTango.AttributeProxy('$undulator/EngineeringLock').write(locked)
	except:
		print "Engineering lock command didn't work. " + str(locked)

def connections(ui):
	QtCore.QObject.connect(ui.btnLock, QtCore.SIGNAL("toggled(bool)"), lock)
	QtCore.QObject.connect(ui.btnLock2, QtCore.SIGNAL("toggled(bool)"), lock)
	QtCore.QObject.connect(ui.cmbGearUpper, QtCore.SIGNAL("currentIndexChanged(int)"), gearUp)        
	QtCore.QObject.connect(ui.cmbGearLower, QtCore.SIGNAL("currentIndexChanged(int)"), gearDown)	
	QtCore.QObject.connect(ui.btnGapRef, QtCore.SIGNAL("clicked()"), lambda: gapToRef(ui.vspGapRef.text(), ui.vspCenterRef.text()))
	QtCore.QObject.connect(ui.cmbPhase, QtCore.SIGNAL("currentIndexChanged(int)"), updatePhaseMode)
	QtCore.QObject.connect(ui.btnStopAll, QtCore.SIGNAL("clicked()"), lambda: stopAll(ui.chkStopAll.isChecked()))
	QtCore.QObject.connect(ui.btnGoA, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosA.getModel(), ui.refPosA.value(), ui.mPosA.text(), ui.mPosA.getSuffixText()))
	QtCore.QObject.connect(ui.btnGoB, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosB.getModel(), ui.refPosB.value(), ui.mPosB.text(), ui.mPosB.getSuffixText()))
	QtCore.QObject.connect(ui.btnGoC, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosC.getModel(), ui.refPosC.value(), ui.mPosC.text(), ui.mPosC.getSuffixText()))
	QtCore.QObject.connect(ui.btnGoD, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosD.getModel(), ui.refPosD.value(), ui.mPosD.text(), ui.mPosD.getSuffixText()))
	QtCore.QObject.connect(ui.btnGoE, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosE.getModel(), ui.refPosE.value(), ui.mPosE.text(), ui.mPosE.getSuffixText()))
	QtCore.QObject.connect(ui.btnGoF, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosF.getModel(), ui.refPosF.value(), ui.mPosF.text(), ui.mPosF.getSuffixText()))
	QtCore.QObject.connect(ui.btnGoG, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosG.getModel(), ui.refPosG.value(), ui.mPosG.text(), ui.mPosG.getSuffixText()))
	QtCore.QObject.connect(ui.btnGoH, QtCore.SIGNAL("clicked()"), lambda: goCommand(ui.mPosH.getModel(), ui.refPosH.value(), ui.mPosH.text(), ui.mPosH.getSuffixText()))
	QtCore.QObject.connect(ui.setMaxGap, QtCore.SIGNAL("valueChanged(int)"), setMaxGap)
	QtCore.QObject.connect(ui.setMinGap, QtCore.SIGNAL("valueChanged(int)"), setMinGap)
	QtCore.QObject.connect(ui.setMaxPhase, QtCore.SIGNAL("valueChanged(int)"), setMaxPhase)


if __name__ == "__main__":
	import sys
	import undulatorDesigner
	from array import *
	import taurus.core
	taurus.core.changeDefaultPollingPeriod(300)
	app = QtGui.QApplication(sys.argv)
	Main = QtGui.QMainWindow()
	ui = undulatorDesigner.Ui_Main()
	ui.setupUi(Main) 
	init(ui) 
	connections(ui)   
	# Create callbacks
	phaseModeCB = PhaseModeCallback(ui)	
	gapConfigCB = GapConfigCallback(ui)
	phaseConfigCB = PhaseConfigCallback(ui)
	# Device proxy
	undulatorProxy = PyTango.DeviceProxy('$undulator')	
	# Subscribe to events
	evPhaseMode = undulatorProxy.subscribe_event('PhaseMode',PyTango.EventType.CHANGE_EVENT, phaseModeCB)
	evGapConfig = undulatorProxy.subscribe_event('Gap',PyTango.EventType.ATTR_CONF_EVENT, gapConfigCB)
	evPhaseConfig = undulatorProxy.subscribe_event('Phase',PyTango.EventType.ATTR_CONF_EVENT, phaseConfigCB)
	# Show GUI
	Main.show()
	sys.exit(app.exec_())
	

