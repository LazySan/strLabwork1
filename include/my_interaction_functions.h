#pragma once

extern "C" {
#include <interface.h>
}

int getBitValue(uInt8 value, uInt8 bit_n);
void setBitValue(uInt8* variable, int n_bit, int new_value_bit);

// CylinderStart related functions
void moveCylinderStartRight();
void moveCylinderStartLeft();
void stopCylinderStart();
int  getCylinderStartPos();
void gotoCylinderStart(int pos);
void calibrationCylinderStart();

// Cylinder1 related functions
void moveCylinder1Front();
void moveCylinder1Back();
void stopCylinder1();
int  getCylinder1Pos();
void gotoCylinder1(int pos);
void calibrationCylinder1();
bool isActiveCylinder1Sensor();

// Cylinder2 related functions
void moveCylinder2Front();
void moveCylinder2Back();
void stopCylinder2();
int  getCylinder2Pos();
void gotoCylinder2(int pos);
void gotoCylinder2(int pos);
void calibrationCylinder2();
bool isActiveCylinder2Sensor();

void moveConveyor();
void stopConveyor();

bool isActiveUpBrickSensor();
bool isActiveDownBrickSensor();

void turnOnFlashingLamp();
void turnOffFlashingLamp();