#include "HID-Project.h"

String myString;

void setup() {
	delay(2000);

	Serial1.begin(115200);
	Serial1.setTimeout(0);
	Serial1.flush();

	BootMouse.begin();
	BootDevice.begin();//This is a fake boot keyboard to match my mouse
}

int xmove = 0;
int ymove = 0;
int btn = 0;
int scrl = 0;

void loop() {
	//Not the best way to read serial1, but it works
	if(Serial1.available() > 8){
		myString = Serial1.readStringUntil('a');
		
		if(myString.length()==8){
			//I didn't feel like dealing with negative values changing my byte length so I just +500 before send and -500 on receive
			xmove = myString.substring(0, 3).toInt() - 500;
			ymove = myString.substring(3, 6).toInt() - 500;
			btn = myString.charAt(6) - '0';
			scrl = (myString.charAt(7) - '0') - 5;

			//Boot mouse is modified in HID-Project.h to send data in the correct packet format for my mouse
			//Additional mouse spoofing is required in the Arduino files
			BootMouse.move(btn, xmove, ymove, scrl);
		}
	}
}
