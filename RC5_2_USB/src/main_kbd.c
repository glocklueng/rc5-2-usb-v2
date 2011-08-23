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


/* Codigos USB HID for Keboard */
#define HID_CODE_PLAY 			0x13		/* P Key */
#define HID_CODE_PAUSE 			0x2C		/* Space Key */
#define HID_CODE_STOP			0x1B		/* X Key */
#define HID_CODE_FF				0x09		/* F Key */
#define HID_CODE_REW			0x15		/* R Key */
#define HID_CODE_LEFT			0x50		/* Left Arrow Key */
#define HID_CODE_RIGHT			0x4F		/* Right Arrow Key */
#define HID_CODE_UP				0x52		/* Up´Arrow Key */
#define HID_CODE_DOWN			0x51		/* Down Arrow Key */
#define HID_CODE_SELECT			0x1B		/* Enter Key */
#define HID_CODE_SHUTDOWN_MENU	0x16		/* S Key */
#define HID_CODE_HOME_MENU		0x1B		/* Escape Key */
#define HID_CODE_PARENT_MENU	0x1B		/* Backspace Key */
#define HID_CODE_VOLUME_LESS	0x56		/* - Keypad Key */
#define HID_CODE_VOLUME_MORE	0x57		/* + Keypad Key */
#define HID_CODE_ZERO			0x27		/* 0 Key */
#define HID_CODE_ONE			0x1E		/* 1 Key */
#define HID_CODE_TWO			0x1F		/* 2 Key */
#define HID_CODE_THREE			0x20		/* 3 Key */
#define HID_CODE_FOUR			0x21		/* 4 Key */
#define HID_CODE_FIVE			0x22		/* 5 Key */
#define HID_CODE_SIX			0x23		/* 6 Key */
#define HID_CODE_SEVEN			0x24		/* 7 Key */
#define HID_CODE_EIGHT			0x25		/* 8 Key */
#define HID_CODE_NINE			0x26		/* 9 Key */
#define HID_CODE_SHUTDOWN		0x4D		/* End Key */
/**
 * Funcion que toma el Código RC5 recibido y devuelve el código de Tecla a presionar
  */
uint8_t decodificar_RC5(uint8_t RC5_System, uint8_t RC5_Command )
{
	uint8_t keycode=0;
	switch (RC5_Command){
	case 0x35: 	keycode = HID_CODE_PLAY;		break;
	case 0x30:	keycode = HID_CODE_PAUSE;		break;
	case 0x36:	keycode = HID_CODE_STOP;		break;
	case 0x34:	keycode = HID_CODE_FF;			break;
	case 0x32:	keycode = HID_CODE_REW;			break;
	case 0x55:	keycode = HID_CODE_LEFT;		break;
	case 0x56:	keycode = HID_CODE_RIGHT;		break;
	case 0x50:	keycode = HID_CODE_UP;			break;
	case 0x51:	keycode = HID_CODE_DOWN;		break;
	case 0x57:	keycode = HID_CODE_SELECT;		break;
	case 0x52:	keycode = HID_CODE_HOME_MENU;	break;
	case 0x11:	keycode = HID_CODE_VOLUME_LESS;	break;
	case 0x10:	keycode = HID_CODE_VOLUME_MORE;	break;
	case 0x00:	keycode = HID_CODE_ZERO;		break;
	case 0x01:	keycode = HID_CODE_ONE;			break;
	case 0x02:	keycode = HID_CODE_TWO;			break;
	case 0x03:	keycode = HID_CODE_THREE;		break;
	case 0x04:	keycode = HID_CODE_FOUR;		break;
	case 0x05:	keycode = HID_CODE_FIVE;		break;
	case 0x06:	keycode = HID_CODE_SIX;			break;
	case 0x07:	keycode = HID_CODE_SEVEN;		break;
	case 0x08:	keycode = HID_CODE_EIGHT;		break;
	case 0x09:	keycode = HID_CODE_NINE;		break;
	case 0x4D:	keycode = HID_CODE_SHUTDOWN;	break;

	}

	return(keycode);
}



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



