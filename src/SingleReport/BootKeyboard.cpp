/*
Copyright (c) 2014-2015 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "BootKeyboard.h"

static const uint8_t _hidReportDescriptorDevice[] PROGMEM = {
	0x06,0xc2,
0xff,0x09,
0x04,0xa1,
0x01,0x15,
0x00,0x26,
0xff,0x00,
0x95,0x40,
0x75,0x08,
0x09,0x02,
0xb1,0x02,
0x09,0x02,
0x81,0x02,
0x09,0x04,
0x91,0x02,
0xc0,
};

BootDevice_::BootDevice_(void) : PluggableUSBModule(2, 1, epType), protocol(HID_REPORT_PROTOCOL), idle(1), featureReport(NULL), featureLength(64)
{
	epType[0] = EP_TYPE_INTERRUPT_IN;
	epType[1] = EP_TYPE_INTERRUPT_IN;
	PluggableUSB().plug(this);
}

int BootDevice_::getInterface(uint8_t* interfaceCount)
{
	*interfaceCount += 1; // uses 1
	CustomDeviceHIDDescriptor hidInterface = {
		D_INTERFACE(pluggedInterface, 2, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
		D_HIDREPORT(sizeof(_hidReportDescriptorDevice)),
		D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01),
		D_ENDPOINT(USB_ENDPOINT_OUT(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01)
	};
	return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
}

int BootDevice_::getDescriptor(USBSetup& setup)
{
	// In a HID Class Descriptor wIndex cointains the interface number
	if (setup.wIndex != pluggedInterface) { return 0; }

	// Check if this is a HID Class Descriptor request
	if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }

	if (setup.wValueH == HID_HID_DESCRIPTOR_TYPE) {
		// Apple UEFI and USBCV wants it
		HIDDescDescriptor desc = D_HIDREPORT(sizeof(_hidReportDescriptorDevice));
		return USB_SendControl(0, &desc, sizeof(desc));
	} else if (setup.wValueH == HID_REPORT_DESCRIPTOR_TYPE) {
		// Reset the protocol on reenumeration. Normally the host should not assume the state of the protocol
		// due to the USB specs, but Windows and Linux just assumes its in report mode.
		protocol = HID_REPORT_PROTOCOL;
		return USB_SendControl(TRANSFER_PGM, _hidReportDescriptorDevice, sizeof(_hidReportDescriptorDevice));
	}

	return 0;
}

bool BootDevice_::setup(USBSetup& setup)
{
	if (pluggedInterface != setup.wIndex) {
		return false;
	}

	uint8_t request = setup.bRequest;
	uint8_t requestType = setup.bmRequestType;

	if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
	{
		if (request == HID_GET_REPORT) {
			// TODO: HID_GetReport();
			return true;
		}
		if (request == HID_GET_PROTOCOL) {
			// TODO improve
#ifdef __AVR__
			UEDATX = protocol;
#endif
			return true;
		}
		if (request == HID_GET_IDLE) {
			// TODO improve
#ifdef __AVR__
			UEDATX = idle;
#endif
			return true;
		}
	}

	if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE)
	{
		if (request == HID_SET_PROTOCOL) {
			protocol = setup.wValueL;
			return true;
		}
		if (request == HID_SET_IDLE) {
			idle = setup.wValueH;
			return true;
		}
		if (request == HID_SET_REPORT)
		{
			return true;
		}
	}

	return false;
}

uint8_t BootDevice_::getLeds(void){
    return leds;
}

uint8_t BootDevice_::getProtocol(void){
    return protocol;
}

void BootDevice_::SendReport(void* data, int length){
	if(protocol == HID_BOOT_PROTOCOL){
		USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, sizeof(HID_BootMouseReport_Data_t));
	}
	else{
		USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, length);
	}
}

BootDevice_ BootDevice;


