// labwork1.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <iostream>
#include<conio.h>
#include<stdlib.h>
#include<windows.h> // for the Sleep function
#include "my_interaction_functions.h"

extern "C" {
#include <interface.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <interrupts.h>
}

#define FRONT 1
#define BACK 0

#define LEFT 0
#define RIGHT 1

#define mainREGION_1_SIZE   8201
#define mainREGION_2_SIZE   29905
#define mainREGION_3_SIZE   7607

xSemaphoreHandle xSemaphoreCalibration;

xSemaphoreHandle xSemaphoreCylinder0;
xSemaphoreHandle xSemaphoreCylinder1;
xSemaphoreHandle xSemaphoreCylinder2;

xSemaphoreHandle xSemaphoreCylinder1Calibration;
xSemaphoreHandle xSemaphoreCylinder2Calibration;

void menu() {
	int tecla = 0;

	while (tecla != 27) {
		tecla = _getch();
		if (tecla == 'c') {
			xSemaphoreGive(xSemaphoreCalibration);
		}
	}
}

void vTaskCalibrationStart(void* pvParameters) {
	//receber inputs do utilizador
}

void vTaskPushBlockCylinder0(void* pvParameters)
{
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder0, portMAX_DELAY);

		gotoCylinderStart(RIGHT);
		gotoCylinderStart(LEFT);		
	}
}

void vTaskPushBlockCylinder1(void* pvParameters)
{
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder1, portMAX_DELAY);

		gotoCylinder1(FRONT);
		gotoCylinder1(BACK);
	}
}

void vTaskPushBlockCylinder2(void* pvParameters)
{
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder2, portMAX_DELAY);

		gotoCylinder2(FRONT);
		gotoCylinder2(BACK);

	}
}

void vTaskCylinder1Calibration(void* pvParameters)
{
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder1Calibration, portMAX_DELAY);

		calibrationCylinder1();
	}
}

void vTaskCylinder2Calibration(void* pvParameters)
{
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder2Calibration, portMAX_DELAY);

		calibrationCylinder2();
	}
}

void inicializarPortos() {
	printf("\nwaiting for hardware simulator...");
	printf("\nReminding: gotoXZ requires kit calibration first...");
	createDigitalInput(0);
	createDigitalInput(1);
	createDigitalOutput(2);
	writeDigitalU8(2, 0);
	printf("\ngot access to simulator...");
}

void myDaemonTaskStartupHook(void) {
	inicializarPortos();

	//Semaforo para iniciar a calibração
	xSemaphoreCalibration = xSemaphoreCreateCounting(1, 0);

	//Começar semaforos de calibração a 1 para que calibre assim que o programa inicie
	xSemaphoreCylinder1Calibration = xSemaphoreCreateCounting(1, 1);
	xSemaphoreCylinder2Calibration = xSemaphoreCreateCounting(1, 1);

	//Semaforos para saber se é preciso ou não empurrar 
	xSemaphoreCylinder0 = xSemaphoreCreateCounting(100, 0);
	xSemaphoreCylinder1 = xSemaphoreCreateCounting(100, 0);
	xSemaphoreCylinder2 = xSemaphoreCreateCounting(100, 0);
	

	xTaskCreate(vTaskPushBlockCylinder0, "vTask_CylinderStart", 100, NULL, 0, NULL);
	xTaskCreate(vTaskPushBlockCylinder1, "vTask_Cylinder1", 100, NULL, 0, NULL);
	xTaskCreate(vTaskPushBlockCylinder2, "vTask_Cylinder2", 100, NULL, 0, NULL);
	xTaskCreate(vTaskCylinder1Calibration, "vTask_Cylinder1Calibration", 100, NULL, 0, NULL);
	xTaskCreate(vTaskCylinder2Calibration, "vTask_Cylinder2Calibration", 100, NULL, 0, NULL);

}

void vAssertCalled(unsigned long ulLine, const char* const pcFileName)
{
	static BaseType_t xPrinted = pdFALSE;
	volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;
	/* Called if an assertion passed to configASSERT() fails.  See
	http://www.freertos.org/a00110.html#configASSERT for more information. */
	/* Parameters are not used. */
	(void)ulLine;
	(void)pcFileName;
	printf("ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError());

	taskENTER_CRITICAL();
	{
		/* Cause debugger break point if being debugged. */
		__debugbreak();
		/* You can step out of this function to debug the assertion by using
		   the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
		   value. */
		while (ulSetToNonZeroInDebuggerToContinue == 0)
		{
			__asm { NOP };
			__asm { NOP };
		}
	}
	taskEXIT_CRITICAL();
}

static void  initialiseHeap(void)
{
	static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
	/* Just to prevent 'condition is always true' warnings in configASSERT(). */
	volatile uint32_t ulAdditionalOffset = 19;
	const HeapRegion_t xHeapRegions[] =
	{
		/* Start address with dummy offsetsSize */
		{ ucHeap + 1,mainREGION_1_SIZE },
		{ ucHeap + 15 + mainREGION_1_SIZE,mainREGION_2_SIZE },
		{ ucHeap + 19 + mainREGION_1_SIZE +
				mainREGION_2_SIZE,mainREGION_3_SIZE },
		{ NULL, 0 }
	};


	configASSERT((ulAdditionalOffset +
		mainREGION_1_SIZE +
		mainREGION_2_SIZE +
		mainREGION_3_SIZE) < configTOTAL_HEAP_SIZE);
	/* Prevent compiler warnings when configASSERT() is not defined. */
	(void)ulAdditionalOffset;
	vPortDefineHeapRegions(xHeapRegions);
}

int main(int argc, char** argv) {
	
	initialiseHeap();
	vApplicationDaemonTaskStartupHook = &myDaemonTaskStartupHook;
	vTaskStartScheduler();	

	Sleep(5000);
	closeChannels();

}