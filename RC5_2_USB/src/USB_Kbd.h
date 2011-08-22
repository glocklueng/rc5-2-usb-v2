/*
 * USB_Kbd.h
 *
 *  Created on: 22/08/2011
 *      Author: ARCESCH
 */

#ifndef USB_KBD_H_
#define USB_KBD_H_


//CodeRed
//Added ref to stdio.h to pull in semihosted printf rather than using serial
//#include <stdio.h>   		// Uses RedLib (Semihost) in Debug build

//#include "stdint.h"			// included for uint8_t
#include "usbapi.h"
#include "usbdebug.h"
//#include "LPC17xx.h"


#define INTR_IN_EP		0x81

#define MAX_PACKET_SIZE	64

#define LE_WORD(x)		((x)&0xFF),((x)>>8)

// CAS
#define REPORT_SIZE			8


// CAS
typedef struct {

    uint8_t Modifier;			/// ..
    uint8_t Reserved;			/// ..
    uint8_t Key_1;				/// ..
    uint8_t Key_2;				/// ..
    uint8_t Key_3;				/// ..
    uint8_t Key_4;				/// ..
    uint8_t Key_5;				/// ..
    uint8_t Key_6;				/// ..

} __attribute__ ((packed)) HIDDKbdInputReport; // GCC

HIDDKbdInputReport KbdInputReport;


static uint8_t	abClassReqData[4];
static int	_iIdleRate = 0;
static int	_iFrame = 0;

// CAS - Kbd Descriptor
static uint8_t abReportDesc[] = {
		0x05, 0x01,			// Usage_Page (Generic Desktop)
		0x09, 0x06,			// Usage (Keyboard)
		0xA1, 0x01,			// Collection (Application)
		0x05, 0x07,			//   Usage page (Key Codes)
		0x19, 0xE0,			//   Usage_Minimum (224)
		0x29, 0xE7,			//   Usage_Maximum (231)
		0x15, 0x00,			//   Logical_Minimum (0)
		0x25, 0x01,			//   Logical_Maximum (1)
		0x75, 0x01,			//   Report_Size (1)
		0x95, 0x08,			//   Report_Count (8)
		0x81, 0x02,			//   Input (Data,Var,Abs) = Modifier Byte
		0x95, 0x01,			//   Report Count (1)
		0x75, 0x08,			//   Report Size (8)
		0x81, 0x01,			//	 Input (Constant), ;Reserved byte
		0x95, 0x05,			//	 Report Count (5),
		0x75, 0x01,			//	 Report Size (1),
		0x05, 0x08,			//   Usage Page (LEDs)
		0x19, 0x01,			//	 Usage Minimum (1),
		0x29, 0x05,			// 	 Usage Maximum (5),
		0x91, 0x02,			//	 Output (Data, Variable, Absolute), ;LED report
		0x95, 0x01,			//	 Report Count (1),
		0x75, 0x03,			//	 Report Size (3),
		0x91, 0x01,			//	 Output (Constant), ;LED report padding
		0x95, 0x06,			//	 Report Count (6),
		0x75, 0x08,			//	 Report Size (8),
		0x15, 0x00,			//	 Logical Minimum (0),
		0x25, 0x65,			//	 Logical Maximum(101),
		0x05, 0x07,			//	 Usage Page (Key Codes),
		0x19, 0x00,			//	 Usage Minimum (0),
		0x29, 0x65,			//	 Usage Maximum (101),
		0x81, 0x00,			//   Input (Data, Array), ;Key arrays (6 bytes)
		0xC0				// End_Collection
};

static const uint8_t abDescriptors[] = {

/* Device descriptor */
		0x12,              		// Lenght = 0x12
		DESC_DEVICE,       		// Constant
		LE_WORD(0x0110),		// bcdUSB				// CAS MADOFICADO!
		0x00,              		// bDeviceClass 		// The interface descriptor specifies the class and the function doesn’t use an interface association descriptor.
		0x00,              		// bDeviceSubClass		// If bDeviceClass is 00h, bDeviceSubclass must be 00h.
		0x00,              		// bDeviceProtocol		// ¿?
		MAX_PACKET_SIZE0,  		// bMaxPacketSize		// For USB 2.0, the maximum packet size equals the field’s value and must be 8 for low speed; 8, 16, 32, or 64 for full speed; and 64 for high speed.
		LE_WORD(0xFDFF),		// idVendor				// Vendor ID
		LE_WORD(0x0021),		// idProduct			// Product ID
		LE_WORD(0x0100),		// bcdDevice			// Device’s release number in BCD format.
		0x01,              		// iManufacturer		// Index that points to a string ...
		0x02,              		// iProduct
		0x03,              		// iSerialNumber
		0x01,              		// bNumConfigurations	// Equals the number of configurations the device supports at the current operating speed.

// configuration
		0x09,					// Lenght = 0x09
		DESC_CONFIGURATION,		// Constant
		LE_WORD(0x22),  		// wTotalLength 		// The number of bytes in the configuration descriptor and all of its subordinate descriptors
														// 0x22 = 34. Len(ConfDesc=9) + Len(IntDesc=9) + Len(HID_Desc=9) + Len(EP_Desc=7) -> 9+9+9+7=34 ???
		0x01,  					// bNumInterfaces
		0x01,  					// bConfigurationValue
		0x00,  					// iConfiguration
		0x80,  					// bmAttributes			// Bus powered
		0x32,  					// bMaxPower

// interface
		0x09,   				// Lenght = 0x09
		DESC_INTERFACE,			// Constant
		0x00,  		 			// bInterfaceNumber		// The default interface is 00h.
		0x00,   				// bAlternateSetting	// Identifies the default interface setting or an alternate setting.
		0x01,   				// bNumEndPoints		// Number of endpoints the interface supports in addition to endpoint zero.
		0x03,   				// bInterfaceClass 		// HID = 0x03
		0x00,   				// bInterfaceSubClass	// Similar to bDeviceSubClass in the device descriptor, but for devices with a class defined by the interface.
		0x00,   				// bInterfaceProtocol	// Similar to bDeviceProtocol in the device descriptor, but for devices whose class is defined by the interface.
		0x00,   				// iInterface			// An index to a string that describes the interface (0 no string).

// HID descriptor
		0x09, 					//
		DESC_HID_HID, 			// bDescriptorType		// HID Class
		LE_WORD(0x0110),		// bcdHID
		0x00,   				// bCountryCode
		0x01,   				// bNumDescriptors = report
		DESC_HID_REPORT,   		// bDescriptorType
		LE_WORD(sizeof(abReportDesc)),

// EP descriptor
		0x07,   				// Lenght = 0x09
		DESC_ENDPOINT,   		// Constant
		INTR_IN_EP,				// bEndpointAddress		// 0x81 = 1000 0001 = Endpoint 1, Direction IN (Data from Endpoint to Host)
		0x03,   				// bmAttributes			// Type of transfer = Interrupt
		LE_WORD(MAX_PACKET_SIZE),// wMaxPacketSize
		10,						// bInterval

// string descriptors
	0x04,
	DESC_STRING,
	LE_WORD(0x0409),

	// manufacturer string
	0x0E,
	DESC_STRING,
	'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

	// product string
	0x12,
	DESC_STRING,
	'P', 0, 'r', 0, 'o', 0, 'd', 0, 'u', 0, 'c', 0, 't', 0, 'X', 0,

	// serial number string
	0x12,
	DESC_STRING,
	'D', 0, 'E', 0, 'A', 0, 'D', 0, 'C', 0, '0', 0, 'D', 0, 'E', 0,

	// terminator
	0
};


Bool HandleClassRequest(TSetupPacket *pSetup, int *piLen, uint8_t **ppbData);
Bool HIDHandleStdReq(TSetupPacket *pSetup, int *piLen, uint8_t **ppbData);
void HandleFrame(uint16_t wFrame);
void HIDDKbdInputReport_Initialize(HIDDKbdInputReport *report);



#endif /* USB_KBD_H_ */
