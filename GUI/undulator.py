import PyTango
import time
from PyQt4 import QtGui,QtCore

class MyThread(QtCore.QThread):
	def __init__(self, parent = None):
		QtCore.QThread.__init__(self, parent)	
			
	# Main loop	
	def run(self):	
		
		while True:	
			# Update phase string
			pMode = PyTango.AttributeProxy('$undulator/PhaseModeReadback').read().value
			self.ui.phaseMode.setText(self.phaseToString(pMode))	
			#Update config
			self.updateConfigRange()
			# Sleep for 3 seconds.	
			time.sleep(3)	
			
	# Thread starts after it gets reference to GUI		
	def setUI(self, ui):
		self.ui = ui
		self.start()
		
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
		
	# Reading min/max gap and max phase from config
	def updateConfigRange(self):
		#Get gap config and set lcds
		gapConfig = PyTango.AttributeProxy('$undulator/GapSetpoint').get_config()
		self.ui.lcdMinGap.display(gapConfig.min_value)
		self.ui.lcdMaxGap.display(gapConfig.max_value)
		self.ui.lcdMaxPhase.display(PyTango.AttributeProxy('$undulator/PhaseSetpoint').get_config().max_value)


# Inicialization
def init(ui):	
	# Set phase mode combobox to current phase mode.
	ui.cmbPhase.setCurrentIndex(int(PyTango.AttributeProxy('$undulator/PhaseModeSetpoint').read().value))
	ui.cmbGearUpper.setCurrentIndex(int(PyTango.AttributeProxy('$gearUp/isGeared').read().value))
	ui.cmbGearLower.setCurrentIndex(int(PyTango.AttributeProxy('$gearLow/isGeared').read().value))
	
		
# Set max gap configuration		
def setMaxGap(value):
	gapConfig = PyTango.AttributeProxy('$undulator/GapSetpoint').get_config()
	if 	gapConfig.max_value != value:
		gapConfig.max_value = str(value)	
		PyTango.AttributeProxy('$undulator/GapSetpoint').set_config(gapConfig)		

# Set min gap configuration
def setMinGap(value):
	gapConfig = PyTango.AttributeProxy('$undulator/GapSetpoint').get_config()
	if 	gapConfig.min_value != value:
		gapConfig.min_value = str(value)	
		PyTango.AttributeProxy('$undulator/GapSetpoint').set_config(gapConfig)

# Set max phase configuration	
def setMaxPhase(value):
	phaseConfig = PyTango.AttributeProxy('$undulator/PhaseSetpoint').get_config()	
	if (phaseConfig.max_value != value) or (phaseConfig.min_value != -value) :
		phaseConfig.min_value = str(-value)
		phaseConfig.max_value = str(value)	
		PyTango.AttributeProxy('$undulator/PhaseSetpoint').set_config(gapConfig)
		
# Sets phase mode
def updatePhaseMode(phaseMode):
    try:
		#Write phase number to device server		
		PyTango.AttributeProxy('$undulator/PhaseModeSetpoint').write(phaseMode)
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
	QtCore.QObject.connect(ui.btnStopAll, QtCore.SIGNAL("clicked()"), lambda: stopAll(self.chkStopAll.isChecked()))
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
	app = QtGui.QApplication(sys.argv)
	Main = QtGui.QMainWindow()
	ui = undulatorDesigner.Ui_Main()
	ui.setupUi(Main) 
	init(ui) 
	connections(ui)   
	thread = MyThread()
	thread.setUI(ui)
	Main.show()
	sys.exit(app.exec_())
	

