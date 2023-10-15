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

// Include guard
#pragma once
#include "HID.h"

MouseAPI::MouseAPI(void) : _buttons(0)
{
	// Empty
}

void MouseAPI::begin(void)
{
    end();
}

void MouseAPI::end(void)
{
    _buttons = 0;
    //move(0, 0, 0);
}

void MouseAPI::click(uint8_t b)
{
	_buttons = b;
	//move(0,0,0);
	_buttons = 0;
	//move(0,0,0);
}



void MouseAPI::move(uint8_t btn, uint16_t x, uint16_t y, uint8_t wheel)
{
	//01
	//BB
	//000000
	//xxxxyyyy
	//SS
	HID_MouseReport_Data_t report;
	report.filler0 = 1;
	report.buttons = btn;
	report.filler1 = 0;
	report.filler2 = 0;
	report.filler3 = 0;
	report.xAxis = x;
	report.yAxis = y;
	report.wheel = wheel;
	SendReport(&report, sizeof(report));
/*
	uint8_t m[9];
	m[0] = btn;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
	m[4] = x;
	m[5] = x >> 8;
	m[6] = y;
	m[7] = y >> 8;
	m[8] = wheel;
	HID().SendReport(1,m,sizeof(m));*/
}


/*
void MouseAPI::move(signed char x, signed char y, signed char wheel)
{
	HID_MouseReport_Data_t report;
	report.buttons = _buttons;
	report.xAxis = x;
	report.yAxis = y;
	report.wheel = wheel;
	SendReport(&report, sizeof(report));
}*/

void MouseAPI::buttons(uint8_t b)
{
	if (b != _buttons)
	{
		_buttons = b;
		//move(0,0,0);
	}
}

void MouseAPI::press(uint8_t b)
{
	buttons(_buttons | b);
}

void MouseAPI::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

void MouseAPI::releaseAll(void)
{
  _buttons = 0;
	//move(0,0,0);
}

bool MouseAPI::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0)
		return true;
	return false;
}
