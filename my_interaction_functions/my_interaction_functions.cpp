#include "my_interaction_functions.h"
#include <interface.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <interrupts.h>


#define FRONT 1
#define BACK 0
#define LEFT 0
#define RIGHT 1

int getBitValue(uInt8 value, uInt8 bit_n)
// given a byte value, returns the value of its bit n
{
	return(value & (1 << bit_n));
}

void setBitValue(uInt8* variable, int n_bit, int new_value_bit)
// given a byte value, set the n bit to value
{
	uInt8 mask_on = (uInt8)(1 << n_bit);
	uInt8 mask_off = ~mask_on;
	if (new_value_bit) *variable |= mask_on;
	else *variable &= mask_off;
}

void moveCylinderStartRight()
{
	if (getCylinderStartPos() == RIGHT) return;

	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2
	taskEXIT_CRITICAL();

	setBitValue(&p, 0, 0); // set bit 0 to low level
	setBitValue(&p, 1, 1); // set bit 1 to high level

	taskENTER_CRITICAL();
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

void moveCylinderStartLeft()
{
	if (getCylinderStartPos() == LEFT) return;

	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2
	taskEXIT_CRITICAL();

	setBitValue(&p, 0, 1); // set bit 1 to low level
	setBitValue(&p, 1, 0); // set bit 0 to high level

	taskENTER_CRITICAL();
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

void stopCylinderStart()
{
	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2
	taskEXIT_CRITICAL();

	setBitValue(&p, 0, 0); // set bit 0 to low level
	setBitValue(&p, 1, 0); // set bit 1 to low level
	
	taskENTER_CRITICAL();
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

int getCylinderStartPos()
{
	taskENTER_CRITICAL();
	uInt8 p0 = readDigitalU8(0);
	taskEXIT_CRITICAL();

	if (getBitValue(p0, 6))
		return 0;
	if (getBitValue(p0, 5))
		return 1;
	return(-1);
}

void gotoCylinderStart(int pos)
{
	switch (pos) {
		case 0:
			moveCylinderStartLeft();
			break;
		case 1:
			moveCylinderStartRight();
			break;
	}
	//Enquanto nao chegar à posição, espera
	while (getCylinderStartPos() != pos) {
		continue;
	};
	stopCylinderStart();
}

void calibrationCylinderStart()
{
	moveCylinderStartLeft();

	//Enquanto nao chegar à posição, espera
	while (getCylinderStartPos() != 0) {
		continue;
	};
	stopCylinderStart();
}

void moveCylinder1Front()
{
	if (getCylinder1Pos() == FRONT) return;

	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2

	setBitValue(&p, 3, 0); // set bit 0 to low level
	setBitValue(&p, 4, 1); // set bit 1 to high level
	
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

void moveCylinder1Back()
{
	if (getCylinder1Pos() == BACK) return;

	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2
	taskEXIT_CRITICAL();

	setBitValue(&p, 3, 1); // set bit 0 to low level
	setBitValue(&p, 4, 0); // set bit 1 to high level
	
	taskENTER_CRITICAL();
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

void stopCylinder1()
{
	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2
	taskEXIT_CRITICAL();
	
	setBitValue(&p, 3, 0); // set bit 0 to low level
	setBitValue(&p, 4, 0); // set bit 1 to high level
	
	taskENTER_CRITICAL();
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

int getCylinder1Pos()
{
	taskENTER_CRITICAL();
	uInt8 p0 = readDigitalU8(0);
	taskEXIT_CRITICAL();

	if (!getBitValue(p0, 4))
		return 0;
	if (!getBitValue(p0, 3))
		return 1;
	return(-1);
}

void gotoCylinder1(int pos)
{
	switch (pos) {
	case 0:
		moveCylinder1Back();
		break;
	case 1:
		moveCylinder1Front();
		break;
	}
	//Enquanto nao chegar à posição, espera
	while (getCylinder1Pos() != pos) {
		continue;
	};
	stopCylinder1();
}

void calibrationCylinder1()
{
	moveCylinder1Front();

	//Enquanto nao chegar à posição, espera
	while (getCylinder1Pos() != 0) {
		if (getCylinder1Pos() == 1) {
			moveCylinder1Back();
		}
		continue;
	};
	stopCylinder1();
}

void moveCylinder2Front()
{
	if (getCylinder2Pos() == FRONT) return;

	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2

	setBitValue(&p, 5, 0); // set bit 0 to low level
	setBitValue(&p, 6, 1); // set bit 1 to high level
	
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

void moveCylinder2Back()
{
	if (getCylinder2Pos() == BACK) return;

	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2
	taskEXIT_CRITICAL();

	setBitValue(&p, 5, 1); // set bit 0 to low level
	setBitValue(&p, 6, 0); // set bit 1 to high level

	taskENTER_CRITICAL();
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

void stopCylinder2()
{
	taskENTER_CRITICAL();
	uInt8 p = readDigitalU8(2); // read port 2
	taskEXIT_CRITICAL();

	setBitValue(&p, 5, 0); // set bit 0 to low level
	setBitValue(&p, 6, 0); // set bit 1 to high level
	
	taskENTER_CRITICAL();
	writeDigitalU8(2, p); // update port 2
	taskEXIT_CRITICAL();
}

int getCylinder2Pos()
{
	taskENTER_CRITICAL();
	uInt8 p0 = readDigitalU8(0);
	taskEXIT_CRITICAL();

	if (!getBitValue(p0, 2))
		return 0;
	if (!getBitValue(p0, 1))
		return 1;
	return(-1);
}

void gotoCylinder2(int pos)
{
	switch (pos) {
	case 0:
		moveCylinder2Back();
		break;
	case 1:
		moveCylinder2Front();
		break;
	}

	//Enquanto nao chegar à posição, espera
	while (getCylinder2Pos() != pos) {
		continue;
	};
	stopCylinder2();
}

void calibrationCylinder2()
{
	moveCylinder2Front();

	//Enquanto nao chegar à posição, espera
	while (getCylinder2Pos() != 0) {
		if (getCylinder2Pos() == 1) {
			moveCylinder2Back();
		}
		continue;		
	};
	stopCylinder2();
}