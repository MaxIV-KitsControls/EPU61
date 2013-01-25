#=============================================================================
#
# file :        Makefile
#
# description : Makefile to generate a TANGO device server.
#
# project :  Soleil ControlBoxV2 device server   
#
# $Author:  Mirjam Lindberg <mirjam.lindberg@maxlab.lu.se>
#
# $Revision:  1.0 (based on Soleil Makefile.linux Revision: 1.19)
#
# $Log:  $
# 
# copyleft :    MAX-lab, Lund University
#
#=============================================================================

##=============================================================================
# MAKE_ENV is the path to find common environment to buil project
#

# MPavleski - all those can be specified as enviromental variables to override
ifndef MAKE_ENV
	MAKE_ENV = /usr/local/share/pogo/preferences
endif

ifndef TANGO_HOME
	TANGO_HOME = /usr/local
endif	

ifndef YAT_ROOT
	YAT_ROOT = /usr/local
endif

ifndef YAT4TANGO_ROOT
	YAT4TANGO_ROOT = /usr/local
endif



#=============================================================================
# OUTPUT_TYPE can be one of the following :
#   - 'STATIC_LIB' for a static library (.a)
#   - 'SHARED_LIB' for a dynamic library (.so)
#   - 'DEVICE' for a device server (will automatically include and link
#	     with Tango dependencies)
#   - 'SIMPLE_EXE' for an executable with no dependency (for exemple the test tool
#		 of a library with no Tango dependencies)
#
OUTPUT_TYPE = DEVICE
RELEASE_TYPE = OPTIMIZED

#=============================================================================
# OUTPUT_DIR  is the directory which contains the build result.
# if not set, the standard location is :
#	- $HOME/DeviceServers if OUTPUT_TYPE is DEVICE
#	- ../bin for others
#
OUTPUT_DIR = ../bin


#=============================================================================
# PACKAGE_NAME is the name of the library/device/exe you want to build
#   - for a device server, PACKAGE_NAME will be prefixed by 'ds_'
#   - for a library (static or dynamic), PACKAGE_NAME will be prefixed by 'lib'
#   - for a simple executable, PACKAGE_NAME will be the name of the executable
#
PACKAGE_NAME = ControlBox

#=============================================================================
# INC_DIR_USER is the list of all include path needed by your sources
#   - for a device server, tango dependencies are automatically appended
#   - '-I ../include' and '-I .' are automatically appended in all cases
#
# -I$(SOLEIL_ROOT)/hw-support/ace/include for ACE library
# -I$(SOLEIL_ROOT)/hw-support/asl/include for ASL library
#-I/hkl/include for HKL library
# ...etc
#
INC_DIR_USER = -I$(YAT_ROOT)/include -I$(YAT4TANGO_ROOT)/include -I./galil_support

#=============================================================================
# LIB_DIR_USER is the list of user library directories
#   - for a device server, tango libraries directories are automatically appended
#   - '-L ../lib' is automatically appended in all cases
# -L $(SOLEIL_ROOT)/hw-support/ace/lib for ACE library
# -L $(SOLEIL_ROOT)/hw-support/asl/lib for ASL library
# -L $(SOLEIL_ROOT)/sw-support/hkl/lib for HKL library
# ...etc
#
LIB_DIR_USER = -L$(YAT_ROOT)/lib -L$(YAT4TANGO_ROOT)/lib -L$(TANGO_HOME)

#=============================================================================
# LFLAGS_USR is the list of user link flags
#   - for a device server, tango libraries directories are automatically appended
#   - '-ldl -lpthread' is automatically appended in all cases
#
# !!! ATTENTION !!!
# Be aware that the order matters. 
# For example if you must link with libA, and if libA depends itself on libB
# you must use '-lA -lB' in this order as link flags, otherwise you will get
# 'undefined reference' errors
#
# -lACE for ACE
# -lASL for ASL
# -lHKL for HKL
#
LFLAGS_USR = -lyat -lyat4tango
#LFLAGS_USR= -lyatd -lyat4tangod

#=============================================================================
# CXXFLAGS_USR lists the compilation flags specific for your library/device/exe
# This is the place where to put your compile-time macros using '-Dmy_macro'
#
# -DACE_HAS_EXCEPTIONS -D__ACE_INLINE__ for ACE
#
ifndef CXXFLAGS_USR
	CXXFLAGS_USR = 
endif 

#-fPIC#add this when building SHARED_LIB
#--DUSE_REF_POS = uses reference instead of actual motor position - for simulation
#-DYAT_ENABLE_TRACE
#CXXFLAGS_USR=-DDMC_4143
# to turn debug on only on device CXXFLAGS_USR=-g
#
#	include Standard TANGO compilation options
#
include $(MAKE_ENV)/tango.opt

#=============================================================================
# SVC_OBJS is the list of all objects needed to make the output
#	
#		$(OBJDIR)/ClassFactory.o  \
#		$(OBJDIR)/main.o  \
#		$(OBJDIR)/$(PACKAGE_NAME)Class.o \
#		$(OBJDIR)/$(PACKAGE_NAME).o \
#		$(OBJDIR)/$(PACKAGE_NAME)StateMachine.o
# 
# other CPP source file path : 
CPPDIRUSER=./galil_support

SVC_OBJS =  \
   $(OBJDIR)/Axis.o\
   $(OBJDIR)/CoupledAxes.o\
	 $(OBJDIR)/GearedAxes.o\
	 $(OBJDIR)/AxisFactory.o\
	 $(OBJDIR)/AxisStateManager.o\
	 $(OBJDIR)/Box.o\
	 $(OBJDIR)/AbstractSlit.o\
	 $(OBJDIR)/GearedSlit.o\
	 $(OBJDIR)/IndependantSlit.o\
	 $(OBJDIR)/GapPositionSlit.o\
	 $(OBJDIR)/HwIO.o\
	 $(OBJDIR)/HwStatusProxy.o\
	 $(OBJDIR)/ObjectManager.o\
	 $(OBJDIR)/UnsolicitedMsgReader.o\
	 $(OBJDIR)/ClassFactory.o\
	 $(OBJDIR)/main.o\
	 $(OBJDIR)/$(PACKAGE_NAME).o\
	 $(OBJDIR)/$(PACKAGE_NAME)Class.o\
	 $(OBJDIR)/$(PACKAGE_NAME)StateMachine.o\
	 $(OBJDIR)/InitTypeParser.o\
	 $(OBJDIR)/GalilAxisClass.o\
	 $(OBJDIR)/GalilAxis.o\
	 $(OBJDIR)/GalilAxisStateMachine.o\
	 $(OBJDIR)/GalilCoupledAxesClass.o\
	 $(OBJDIR)/GalilCoupledAxes.o\
	 $(OBJDIR)/GalilCoupledAxesStateMachine.o\
	 $(OBJDIR)/GalilGearedAxesClass.o\
	 $(OBJDIR)/GalilGearedAxes.o\
	 $(OBJDIR)/GalilGearedAxesStateMachine.o\
	 $(OBJDIR)/GalilSlitClass.o\
	 $(OBJDIR)/GalilSlit.o\
	 $(OBJDIR)/GalilSlitStateMachine.o\
	 $(OBJDIR)/AxisRawDataReaderClass.o\
	 $(OBJDIR)/AxisRawDataReader.o\
	 $(OBJDIR)/AxisRawDataReaderStateMachine.o

#
# Verbose mode
#
#.SILENT:

#
#	include common targets
#
include $(MAKE_ENV)/common_target.opt
 



