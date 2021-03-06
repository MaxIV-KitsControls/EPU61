static const char *RcsId = "$Header: /usr/local/CVS/DeviceServer/Generic/Motorisation/ControlBoxV2/src/main.cpp,v 1.8 2011/08/16 14:56:22 leclercq Exp $";
//+=============================================================================
//
// file :        main.cpp
//
// description : C++ source for a TANGO device server main.
//               The main rule is to initialise (and create) the Tango
//               system and to create the DServerClass singleton.
//               The main should be the same for every Tango device server.
//
// project :     TANGO Device Server
//
// $Author: leclercq $
//
// $Revision: 1.8 $ $
//
// $Log: main.cpp,v $
// Revision 1.8  2011/08/16 14:56:22  leclercq
// Trying to fix 2011-run3 crashes...
//
// Revision 1.5  2010/10/06 20:27:50  maven
// Breakpad Migration
//
//
// copyleft :    European Synchrotron Radiation Facility
//               BP 220, Grenoble 38043
//               FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================

#include <tango.h>
#include <ObjectManager.h>

#if defined(ENABLE_CRASH_REPORT)
# include <crashreporting/crash_report.h>
#else
# define DECLARE_CRASH_HANDLER
# define INSTALL_CRASH_HANDLER
#endif

DECLARE_CRASH_HANDLER;

int main(int argc, char *argv[])
{
  INSTALL_CRASH_HANDLER;

	Tango::Util *tg = 0;
	try
	{
		//- bootstraping for yat Socket
    yat::Socket::init();

	  //- bootstraping for galil stuffs
    galil::ObjectManager::init();

		// Initialise the device server
		//----------------------------------------
		tg = Tango::Util::init(argc,argv);

		// Create the device server singleton 
		//	which will create everything
		//----------------------------------------
		tg->server_init(false);

		// Run the endless loop
		//----------------------------------------
		cout << "Ready to accept request" << endl;
		tg->server_run();

	}
	catch (const bad_alloc&)
	{
		cout << "Can't allocate memory to store device object !!!" << endl;
		cout << "Exiting" << endl;
  } 
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);
		
		cout << "Received a CORBA_Exception" << endl;
		cout << "Exiting" << endl;
	}
	catch (...)
	{
		cout << "Received an unknown exception" << endl;
		cout << "Exiting" << endl;
	}

  try
  {
	  tg->server_cleanup();
  }
  catch (...)
  {
    //- ignore any error from "server_cleanup"
  }

  //- release galil stuffs
  galil::ObjectManager::close();

	//- bootstraping for yat Socket
  yat::Socket::terminate();

	return(0);
}
