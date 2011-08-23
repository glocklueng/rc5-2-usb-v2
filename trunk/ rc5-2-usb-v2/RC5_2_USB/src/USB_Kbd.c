/*
 * USB_Kbd.c
 *
 *  Created on: 22/08/2011
 *      Author: ARCESCH
 */




#include "USB_Kbd.h"


/*************************************************************************
	HandleClassRequest
	==================
		HID class request handler

**************************************************************************/
Bool HandleClassRequest(TSetupPacket *pSetup, int *piLen, uint8_t **ppbData)
{
	uint8_t	*pbData = *ppbData;

	switch (pSetup->bRequest) {

   	// get_idle
	case HID_GET_IDLE:
		DBG("GET IDLE, val=%X, idx=%X\n", pSetup->wValue, pSetup->wIndex);
		pbData[0] = (_iIdleRate / 4) & 0xFF;
		*piLen = 1;
		break;

	// set_idle:
	case HID_SET_IDLE:
		DBG("SET IDLE, val=%X, idx=%X\n", pSetup->wValue, pSetup->wIndex);
		_iIdleRate = ((pSetup->wValue >> 8) & 0xFF) * 4;
		break;

	default:
		DBG("Unhandled class %X\n", pSetup->bRequest);
		return FALSE;
	}
	return TRUE;
}



/*************************************************************************
	HIDHandleStdReq
	===============
		Standard request handler for HID devices.

	This function tries to service any HID specific requests.

**************************************************************************/
Bool HIDHandleStdReq(TSetupPacket *pSetup, int *piLen, uint8_t **ppbData)
{
	uint8_t	bType, bIndex;

	if ((pSetup->bmRequestType == 0x81) &&			// standard IN request for interface
		(pSetup->bRequest == REQ_GET_DESCRIPTOR)) {	// get descriptor

		bType = GET_DESC_TYPE(pSetup->wValue);
		bIndex = GET_DESC_INDEX(pSetup->wValue);
		switch (bType) {

		case DESC_HID_REPORT:
			// report
			*ppbData = abReportDesc;
			*piLen = sizeof(abReportDesc);
			break;

		case DESC_HID_HID:
		case DESC_HID_PHYSICAL:
		default:
		    // search descriptor space
		    return USBGetDescriptor(pSetup->wValue, pSetup->wIndex, piLen, ppbData);
		}

		return TRUE;
	}
	return FALSE;
}



// CAS - rewritten to provide real mouse reports
/*
*/
void HandleFrame(uint16_t wFrame)
{
	static int iCount;

	_iFrame++;
	if ((_iFrame > 100)) {
		iCount++;
		USBHwEPWrite(INTR_IN_EP, (void *)&KbdInputReport, REPORT_SIZE);
		_iFrame = 0;
	}
}







// CAS - initial kbd report
void HIDDKbdInputReport_Initialize(HIDDKbdInputReport *report)
{
    report->Modifier = 0;
    report->Reserved = 0;
    report->Key_1 = 0;
    report->Key_2 = 0;
    report->Key_3 = 0;
    report->Key_4 = 0;
    report->Key_5 = 0;
    report->Key_6 = 0;
}



