//

//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Code Red Technologies Ltd. 
//
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.

/*
	LPCUSB, an USB device driver for LPC microcontrollers	
	Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The name of the author may not be used to endorse or promote products
	   derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/**********************************
 * CodeRed - extended version of LPCUSB HID example to provide PC mouse
 **********************************/

//CodeRed
//Added ref to stdio.h to pull in semihosted printf rather than using serial
#include <stdio.h>   		// Uses RedLib (Semihost) in Debug build


#include "stdint.h"			// included for uint8_t
#include "usbapi.h"
#include "usbdebug.h"

#include "LPC17xx.h"


//CRP = Code Read Protection
#include <cr_section_macros.h>
#include <NXP/crp.h>


#include "RC5.h"
#include "USB_Kbd.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;





/*************************************************************************
	main
	====
**************************************************************************/
int main(void)
{
	// USB: Init
	// CAS -> Initialization of KbdInputReport to initial values (0,0,0,0,..)
	HIDDKbdInputReport_Initialize (&KbdInputReport);

	// CAS -> Variable used to simulate the pressing of Letter A (0x04)
	int i;

	DBG("Initialising USB stack\n");
	
	// CAS -> Variable used to simulate the pressing of Letter A (0x04)
	// 	USBInit()
	//		USBHwInit()										HW Init
	//			-> Set Pins for USB, AHBClock, ACK generate interrupts
	//		USBHwRegisterDevIntHandler(HandleUsbReset)
	//			-> Define un handler (HandleUsbReset).
	//
	// initialize stack
	USBInit();
	
	// register device descriptors
	USBRegisterDescriptors(abDescriptors);

	// register HID standard request handler
	USBRegisterCustomReqHandler(HIDHandleStdReq);

	// register class request handler
	USBRegisterRequestHandler(REQTYPE_TYPE_CLASS, HandleClassRequest, abClassReqData);

	// register endpoint (2?). Interrupt In Endpoint
	USBHwRegisterEPIntHandler(INTR_IN_EP, NULL);

	// register frame handler
	USBHwRegisterFrameHandler(HandleFrame);
	
	DBG("Starting USB communication\n");


	// connect to bus
	USBHwConnect(TRUE);

	//RC5: Init
	GPIO_SetDir(0,1<<4,1);
	GPIO_SetDir(0,1<<5,1);

	GPIO_SetValue(0,1<<5);
	GPIO_SetValue(0,1<<4);


	RC5_Init();

	// main() Loop:


	// call USB interrupt handler continuously
	
	// CodeRed - either read mouse, or provide "fake mouse readings"
	while (1)
	{

		for (i=0; i<10;  i++) {


			// RC5
		    if (RC5_flag)                      // wait for RC5 code
		    	    {

		      if((RC5_System==0)&&(RC5_Command==1)){
		      GPIO_ClearValue(0,1<<4);
		      for(i=0;i<1000000;i++);
		      GPIO_SetValue(0,1<<4);
		      for(i=0;i<1000000;i++);
		      }

		      if((RC5_System==0)&&(RC5_Command==5)){
		      GPIO_ClearValue(0,1<<5);
		      for(i=0;i<1000000;i++);
		      GPIO_SetValue(0,1<<5);
		      for(i=0;i<1000000;i++);
		      }

		      RC5_flag = 0;
		      printf( "RC5 =  %d   %d\n", RC5_System, RC5_Command);
		    }
		    // RC5

		  USBHwISR();
		  if (i < 5) {
			  KbdInputReport.Key_1 = 0x04;
		  }
		  else {
			  KbdInputReport.Key_1 = 0x00;
		  }
		}
		
	}
	
	return 0;
}



