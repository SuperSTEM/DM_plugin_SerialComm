
#define _GATAN_USE_STL_STRING		// Provide conversion from 'DM::String' to 'std::string'

#define _GATANPLUGIN_USES_LIBRARY_VERSION 2
#include "DMPlugInBasic.h"

#define _GATANPLUGIN_USE_CLASS_PLUGINMAIN
#include "DMPlugInMain.h"

using namespace Gatan;

#include <cassert>
#include <tchar.h>
#include <Serial.h>
#include <iostream>
#include <sstream>

HANDLE hSerial;

class SerialPlugIn : public Gatan::PlugIn::PlugInMain
{
	virtual void Start();
	virtual void Run();
	virtual void Cleanup();
	virtual void End();

	void AddDMFunctions( void );
	void RemoveDMFunctions( void );

	typedef std::vector<DM::Function> FunctionList;

	FunctionList fFunctionList;
};

void SerialOpen(char * port, long baud)
{
	PLUG_IN_ENTRY

	CSerial serial;
	int status;

	status=serial.Open(_T(port));
	serial.Setup(CSerial::EBaudrate(baud),
      CSerial::EDataBits(8),
      CSerial::EParity(NOPARITY),
      CSerial::EStopBits(ONESTOPBIT));
	serial.SetupHandshaking(CSerial::EHandshakeOff);

	if (status != 0) {
		DM::Result("Error opening port.  Code:\n");
//		DM::Result();
		DM::Result("\n");
	}
	else
		DM::Result( DM::StringAppend(DM::StringAppend(DM::String("COM port "), port), " opened.\n" ));

	PLUG_IN_EXIT
}

void SerialCheck( char * port )
{
	CSerial serial;
	short status;
	const char * str="";

	status=serial.CheckPort(_T(str));

	switch (status) {
		case -1:		
			// Unknown error occurred
			DM::Result("Unknown error.\n");
			break;
		case 0:		
			// Port is available
			DM::Result("Com port available.\n");
			break;
		case 1:		
			// Port is not present
			DM::Result("No such com port.\n");
			break;
		case 2:		
			// Port is in use
			DM::Result("COM port is already in use.\n");
			break;
	}
}

void SerialClose( void )
{
	PLUG_IN_ENTRY
	CSerial serial;
	serial.Close();
	DM::Result( "COM port closed.\n" );

	PLUG_IN_EXIT
}

short RxByte( void )
{
	PLUG_IN_ENTRY
	
	char str[3];

	CSerial serial;
	DWORD dwBytesRead = 0;
	short byte[1];
	
	serial.Read(byte,1,&dwBytesRead);

	sprintf(str, "3.0f", byte);
	DM::Result("Received byte.\n");
	//DM::Result( DM::StringAppend(DM::StringAppend(DM::String("Received byte: "), DM::String(str)), ".\n" ));
	return byte[1];
	PLUG_IN_EXIT
}

void TxByte(short number)
{
	PLUG_IN_ENTRY
	char msg[1];
	msg[1]=(char)(number);
	CSerial serial;
	serial.Write(_T(msg));
	DM::Result("Sent byte.\n");
	//DM::Result( DM::StringAppend(DM::StringAppend(DM::String("Sent byte: "), msg[1]), ".\n" ));

	PLUG_IN_EXIT
}

void TxInt(unsigned int num)
{
	PLUG_IN_ENTRY
	short c;
	for (int i=0; i<4; i+=1){
		c=(char)(num>>i*8);
		TxByte(c);
	}
	DM::Result( "Sent int: \n" );
	PLUG_IN_EXIT
}

void SerialPlugIn::AddDMFunctions( void )
{
	fFunctionList.push_back( AddFunction( 0, "void SerialOpen(char * port, long baud)", 3, (void *) &SerialOpen ) );
	fFunctionList.push_back( AddFunction( 0, "void SerialCheck(char * port)", 3, (void *) &SerialCheck ) );
	fFunctionList.push_back( AddFunction( 0, "void SerialClose( void )", 3, (void *)  &SerialClose ) );
	fFunctionList.push_back( AddFunction( 0, "short RxByte( void )", 3, (void *) &RxByte ) );
	fFunctionList.push_back( AddFunction( 0, "void TxByte(short byte)", 3, (void *) &TxByte ) );
	fFunctionList.push_back( AddFunction( 0, "void TxInt(unsigned int num)", 3, (void *) &TxInt ) );
}

void SerialPlugIn::RemoveDMFunctions( void )
{
	FunctionList::const_iterator functionPtr;

	for(	functionPtr = fFunctionList.begin(); 
			functionPtr != fFunctionList.end(); 
			++functionPtr )
	{
		RemoveFunction( *functionPtr );
	}
}

///
/// This is called when the plugin is loaded.  Whenever DM is
/// launched, it calls 'Start' for each installed plug-in.
/// When it is called, there is no guarantee that any given
/// plugin has already been loaded, so the code should not
/// rely on scripts installed from other plugins.  The primary
/// use is to install script functions.
///

void SerialPlugIn::Start()
{
	this->AddDMFunctions();
	DM::Result("*** Serial communication plug-in started OK.\n");
}
void SerialPlugIn::Run()
{
}
void SerialPlugIn::Cleanup()
{
	this->RemoveDMFunctions();
}

void SerialPlugIn::End()
{

}

SerialPlugIn gSerialPlugIn;