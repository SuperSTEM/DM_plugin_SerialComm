Serial Library for Digital Micrograph, version 1.0 (12/04/2011)

Copyright 2011, Michael Sarahan
Distributed under the terms of the GNU public license, Version 3.  This license is detailed in the LICENSE.txt file.

This plugin is designed to allow simple serial communications.  By "simple", I mean sending and receiving data one byte or int at a time.  
It's good for very low-level communication with devices.

It is based on the serial library by Ramon de Klein, available at:
http://www.codeproject.com/KB/system/serial.aspx

DM versions supported:
GMS 1.8 (tested on 1.8.4)

INSTALLATION DIRECTIONS:
1. Close DM, if open.
2. Copy the SerialPlugIn.dll file from the bin directory to your DigitalMicrograph/plugins folder.
3. Open DM - The results window should show some information that the library has been loaded.  The functions are now available.

This library provides 6 functions to DM:
- number SerialOpen(string port, number baud, number debug)
	- Opens a serial port.  baud must be one of several integer values:
		- 110
		- 300 
		- 600 
		- 1200 
		- 2400 
		- 4800 
		- 9600
		- 14400 
		- 19200
		- 38400
		- 57600
		- 115200
		- 128000 
		- 256000
- void SerialCheck(string port)
	- Diagnostics on a given serial port.  Not terribly useful aside from telling you that a port is available.
- void SerialClose( number debug )
	- Closes the active serial connection
- number RxByte( number debug )
	- receive one byte.  returns the value of that byte.
- number RxInt( number debug )
	- receive one long integer (4 bytes).  returns the value of that int.
- number TxByte(number byte, number debug)
	- send one byte.
- number TxInt(number num, number debug)
	- send one long int (4 bytes).

In all cases, debug is either 0 (no debugging output) or 1 (debugging output). port variables are a string, of the form "COM5" (without the quotes).

Some serial communication parameters are hard-coded:

parity: none
Handshaking: none
stop bits: 1
data bits: 8

Example usage in DM script:

#####################################################################

string port="COM7"
number baud=19200
number ans
number reset_wait_delay=20
serialopen(port, baud, debug)
// send a 0.
txbyte(0,debug)
// wait for the other end to reply and say it's OK to send.
ans=rxbyte(debug)
// If the box says it's OK (255), then send the signal (255=ON, 0=OFF).
if (ans==255) txbyte(state,debug)
else if (ans==128) {
	// an answer of 128 (really 0).
	// wait, then resend the request to send input.
	delay(reset_wait_delay)
		txbyte(0,debug)
		ans=rxbyte(debug)
		if (ans==255) txbyte(state,debug)
	}
// If you close the port after any communication, this plugin should be quite robust to causing problems with DM.
serialclose(debug)

######################################################################

NOTE: issuing an RxByte or RxInt command will cause DM to wait until it has received that data.  If it seems to hang, it is probably waiting.  For USB virtual com ports, 
	you can safely disconnect and reconnect the device to make DM stop waiting.  You just need to reconnect to the device (serialopen) to start again.

Known issues:
1. The RxByte function returns 128 when it should actually return 0.  This is caused by the serial library reading the stop bit as the final data bit.
2. Does not seem to recognize COM ports above COM9.  It is often possible to change the number of USB virtual COM ports from the driver for that device.
3. If your baud rate does not match the baud rate set on your device, the serial communications will be out of sync.  You'll get all sorts of weird behavior.  Make sure these match.  
	Also, test that your device can actually support whatever speed you set it to.  If both this library and your device are set to the same high speed, 
	but the results are strange, then your device can't communicate that quickly. 