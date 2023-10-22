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

xSemaphoreHandle xSemaphoreMenu;

xSemaphoreHandle xSemaphoreManualCalibration;

xSemaphoreHandle xSemaphoreConveyor;

xSemaphoreHandle xSemaphoreFlashingLamp;

xSemaphoreHandle xSemaphoreGetBlockCountCylinder1;
xSemaphoreHandle xSemaphoreGetBlockCountCylinder2;

xSemaphoreHandle xSemaphoreBlockBrickCountCylinder1;
xSemaphoreHandle xSemaphoreBlockBrickCountCylinder2;
xSemaphoreHandle xSemaphoreResumeBrickCountCylinder1;
xSemaphoreHandle xSemaphoreResumeBrickCountCylinder2;

xQueueHandle xQueueBrickPassedCylinder1;
xQueueHandle xQueueBrickPassedCylinder2;

xSemaphoreHandle xSemaphoreStartBrickVerification;
xSemaphoreHandle xSemaphoreEndBrickVerification;

xSemaphoreHandle xSemaphoreInsertBrick;
xQueueHandle xQueueBrickType;


xQueueHandle xQueueConfirmedBrickType;

xQueueHandle xQueueCylinder0Limit;
xQueueHandle xQueueCylinder1Limit;
xQueueHandle xQueueCylinder2Limit;

xSemaphoreHandle xSemaphoreCylinder0Limit;
xSemaphoreHandle xSemaphoreCylinder1Limit;
xSemaphoreHandle xSemaphoreCylinder2Limit;

xSemaphoreHandle xSemaphoreCylinder0Push;
xQueueHandle xQueueCylinder1Push;
xQueueHandle xQueueCylinder2Push;

xSemaphoreHandle xSemaphoreCylinder0Calibration;
xSemaphoreHandle xSemaphoreCylinder1Calibration;
xSemaphoreHandle xSemaphoreCylinder2Calibration;

void vTaskMenu(void* pvParameters) {

	while (true) {

		xSemaphoreTake(xSemaphoreMenu, portMAX_DELAY);

		int tecla = 0;
		system("cls");
		printf("\nMenu");
		printf("\nc - Calibrar cilindros");
		printf("\nb - Inserir blocos");
		printf("\nm - Controlo manual");
		printf("\nh - Mostrar historico");
		printf("\ne - Estatistica");

		tecla = _getch();
		switch (tecla) {

		case 'c':
			xSemaphoreGive(xSemaphoreCylinder0Calibration);
			xSemaphoreGive(xSemaphoreCylinder1Calibration);
			xSemaphoreGive(xSemaphoreCylinder2Calibration);
			xSemaphoreGive(xSemaphoreMenu);

			break;
		case 'b':
			xSemaphoreGive(xSemaphoreInsertBrick);
			break;
		case 'm':
			xSemaphoreGive(xSemaphoreManualCalibration);
			break;
		case 'h':
			printf("\nHISTORICO NAO IMPLEMENTADO");
			xSemaphoreGive(xSemaphoreMenu);
			break;
		case 'e':
			printf("\nESTATISTICA NAO IMPLEMENTADO");
			xSemaphoreGive(xSemaphoreMenu);
			break;

			//IgnorarEsc
		case 27:
			printf("\nSaida do programa");
			exit(1);
			break;

		default:
			printf("\nComando nao reconhecido");
			xSemaphoreGive(xSemaphoreMenu);
			break;
		}
	}
}

void vTaskManualCalibrationStart(void* pvParameters) {

	while (true) {

		xSemaphoreTake(xSemaphoreManualCalibration, portMAX_DELAY);
		int pos0, pos1, pos2;
		int tecla = 0;
		system("cls");
		printf("\nEntrada em modo manual:");

		while (tecla != 27) {
			tecla = _getch();
			switch (tecla) {

				//Cylinder0
			case 'q':
				moveCylinderStartLeft();
				pos0 = LEFT;
				xQueueOverwrite(xQueueCylinder0Limit, &pos0);
				break;
			case 'a':
				moveCylinderStartRight();
				pos0 = RIGHT;
				xQueueOverwrite(xQueueCylinder0Limit, &pos0);
				break;
			case 'z':
				stopCylinderStart();
				break;

				//Cylinder1
			case 'w':
				moveCylinder1Back();
				pos1 = BACK;
				xQueueOverwrite(xQueueCylinder1Limit, &pos1);
				break;
			case 's':
				moveCylinder1Front();
				pos1 = FRONT;
				xQueueOverwrite(xQueueCylinder1Limit, &pos1);
				break;
			case 'x':
				stopCylinder1();
				break;

				//Cylinder2
			case 'e':
				moveCylinder2Back();
				pos2 = BACK;
				xQueueOverwrite(xQueueCylinder2Limit, &pos2);
				break;
			case 'd':
				moveCylinder2Front();
				pos2 = FRONT;
				xQueueOverwrite(xQueueCylinder2Limit, &pos2);
				break;
			case 'c':
				stopCylinder2();
				break;

				//IgnorarEsc
			case 27:
				break;

			default:
				printf("\nComando nao reconhecido");
				break;
			}
		}
		printf("\nSaida do modo manual:");
		xSemaphoreGive(xSemaphoreMenu);
	}
}

void vTaskRegisterBrick(void* pvParameters) {

	while (true) {
		xSemaphoreTake(xSemaphoreInsertBrick, portMAX_DELAY);
		char brickType = 0;
		int message;

		system("cls");
		printf("\nEspera de blocos:\n");
		while (brickType != 'q') {
			fflush(stdin);
			scanf(" %c", &brickType);

			if (brickType >= '1' && brickType <= '3') {

				message = brickType - '0';
				xQueueSend(xQueueBrickType, &message, portMAX_DELAY);

			}
			else {
				printf("Tipo de bloco nao reconhecido\n");
			}
		}
		xSemaphoreGive(xSemaphoreMenu);
	}
}

/*
void vTaskHandleBrick(void* pvParameters) {
	int brickType;
	int confirmedBrickType;
	int ignoreCount1 = 0;
	int ignoreCount2 = 0;
	while (true) {
		xQueueReceive(xQueueBrickType, &brickType, portMAX_DELAY);

		printf("BRICK ESPERADO %d\n", brickType);

		xSemaphoreGive(xSemaphoreCylinder0Push);

		switch (brickType) {
		case 1:
			xQueueSend(xQueueCylinder1Push, &ignoreCount1, portMAX_DELAY);
			ignoreCount1 = 0;
			break;
		case 2:
			ignoreCount1++;
			xQueueSend(xQueueCylinder2Push, &ignoreCount2, portMAX_DELAY);
			ignoreCount2 = 0;
			break;
		case 3:
			ignoreCount1++;
			ignoreCount2++;
			break;
		}
	}
}
*/

void vTaskHandleConfirmedBrick(void* pvParameters) {
	int brickType;
	int confirmedBrickType;
	int ignoreCount1 = 0;
	int ignoreCount2 = 0;
	while (true) {
		xQueueReceive(xQueueBrickType, &brickType, portMAX_DELAY);

		xSemaphoreGive(xSemaphoreCylinder0Push);
		xQueueReceive(xQueueConfirmedBrickType, &confirmedBrickType, portMAX_DELAY);

		switch (brickType) {
		case 1:
			if (confirmedBrickType != 1) {
				ignoreCount1++;
				ignoreCount2++;
				xSemaphoreGive(xSemaphoreFlashingLamp);
			}
			else {
				xQueueSend(xQueueCylinder1Push, &ignoreCount1, portMAX_DELAY);
				ignoreCount1 = 0;
			}
			break;
		case 2:
			if (confirmedBrickType != 2) {
				ignoreCount1++;
				ignoreCount2++;
				xSemaphoreGive(xSemaphoreFlashingLamp);
			}
			else {
				ignoreCount1++;
				xQueueSend(xQueueCylinder2Push, &ignoreCount2, portMAX_DELAY);
				ignoreCount2 = 0;
			}
			break;
		case 3:
			if (confirmedBrickType != 3) {
				xSemaphoreGive(xSemaphoreFlashingLamp);
			}
			ignoreCount1++;
			ignoreCount2++;
			break;
		}
	}
}


void vTaskBrickSensors(void* pvParameters) {
	int brickType;
	bool sensorUpCheck;
	bool sensorDownCheck;
	bool sensorReseted;
	while (true) {
		xSemaphoreTake(xSemaphoreStartBrickVerification, portMAX_DELAY);

		brickType = 1;
		sensorUpCheck = false;
		sensorDownCheck = false;
		sensorReseted = false;

		while (!xSemaphoreTake(xSemaphoreEndBrickVerification, 0)) {

			while (!sensorReseted)
			{
				if (!isActiveUpBrickSensor() && !isActiveDownBrickSensor()) {
					sensorReseted = true;
				}

			}

			if (!sensorUpCheck) {
				if (isActiveUpBrickSensor()) {
					brickType++;
					sensorUpCheck = true;
				}
			}
			if (!sensorDownCheck) {
				if (isActiveDownBrickSensor()) {
					brickType++;
					sensorDownCheck = true;
				}
			}
		}
		xQueueSend(xQueueConfirmedBrickType, &brickType, portMAX_DELAY);
	}
}

void vTaskFlashingLamp(void* pvParameters) {

	while (true) {
		xSemaphoreTake(xSemaphoreFlashingLamp, portMAX_DELAY);

		turnOnFlashingLamp();
		vTaskDelay(3000);
		turnOffFlashingLamp();
	}
}

void vTaskConveyor(void* pvParameters) {
	while (true) {
		xSemaphoreTake(xSemaphoreConveyor, portMAX_DELAY);

		moveConveyor();
	}
}

void vTaskGetPassBlockCylinder1(void* pvParameters) {
	int passCount=0;

	while (true) {
		while (!xSemaphoreTake(xSemaphoreBlockBrickCountCylinder1, 0)) {

			while (!isActiveCylinder1Sensor()) {

				if (xSemaphoreTake(xSemaphoreGetBlockCountCylinder1, 0)) {
					xQueueSend(xQueueBrickPassedCylinder1, &passCount, portMAX_DELAY);
				}
				continue;
			}

			passCount++;

			while (isActiveCylinder1Sensor()) {
				continue;
			}
		}
		xSemaphoreTake(xSemaphoreResumeBrickCountCylinder1, portMAX_DELAY);
		passCount = 0;
	}
}

void vTaskGetPassBlockCylinder2(void* pvParameters) {
	int passCount = 0;

	while (true) {
		while (!xSemaphoreTake(xSemaphoreBlockBrickCountCylinder2, 0)) {

			while (!isActiveCylinder2Sensor()) {

				if (xSemaphoreTake(xSemaphoreGetBlockCountCylinder2, 0)) {
					xQueueSend(xQueueBrickPassedCylinder2, &passCount, portMAX_DELAY);
				}
				continue;
			}

			passCount++;

			while (isActiveCylinder2Sensor()) {
				continue;
			}
		}
		xSemaphoreTake(xSemaphoreResumeBrickCountCylinder2, portMAX_DELAY);
		passCount = 0;
	}
}

void vTaskPushBlockCylinder0(void* pvParameters)
{
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder0Push, portMAX_DELAY);

		xSemaphoreGive(xSemaphoreStartBrickVerification);
		gotoCylinderStart(RIGHT);

		moveCylinderStartRight();
		vTaskDelay(25);
		stopCylinderStart();

		xSemaphoreGive(xSemaphoreEndBrickVerification);
		gotoCylinderStart(LEFT);
	}
}

void vTaskPushBlockCylinder1(void* pvParameters)
{
	int ignoreCount;
	int passedCount;
	while (true) {
		xQueueReceive(xQueueCylinder1Push, &ignoreCount, portMAX_DELAY);
		xSemaphoreGive(xSemaphoreGetBlockCountCylinder1);
		xQueueReceive(xQueueBrickPassedCylinder1, &passedCount, portMAX_DELAY);
		
		ignoreCount -= passedCount;
		
		while (ignoreCount >= 0) {
			while (!isActiveCylinder1Sensor()) {
				continue;
			}
			if (ignoreCount <= 0) {
				xSemaphoreGive(xSemaphoreBlockBrickCountCylinder1);
				stopConveyor();
				gotoCylinder1(FRONT);
				gotoCylinder1(BACK);
				xSemaphoreGive(xSemaphoreConveyor);
				xSemaphoreGive(xSemaphoreResumeBrickCountCylinder1);
			}
			ignoreCount--;
			while (isActiveCylinder1Sensor()) {
				continue;
			}
		}
	}
}

void vTaskPushBlockCylinder2(void* pvParameters)
{
	int ignoreCount;
	int passedCount;
	while (true) {
		xQueueReceive(xQueueCylinder2Push, &ignoreCount, portMAX_DELAY);

		xSemaphoreGive(xSemaphoreGetBlockCountCylinder2);
		xQueueReceive(xQueueBrickPassedCylinder2, &passedCount, portMAX_DELAY);

		ignoreCount -= passedCount;

		while (ignoreCount >= 0) {
			while (!isActiveCylinder2Sensor()) {
				continue;
			}
			if (ignoreCount <= 0) {
				xSemaphoreGive(xSemaphoreBlockBrickCountCylinder2);
				stopConveyor();
				gotoCylinder2(FRONT);
				gotoCylinder2(BACK);
				xSemaphoreGive(xSemaphoreConveyor);
				xSemaphoreGive(xSemaphoreResumeBrickCountCylinder2);
			}
			ignoreCount--;
			while (isActiveCylinder2Sensor()) {
				continue;
			}
		}
	}
}

void vTaskCylinder0Calibration(void* pvParameters)
{
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder0Calibration, portMAX_DELAY);

		calibrationCylinderStart();
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

void vTaskCylinder0Limit(void* pvParameters)
{
	int pos;
	while (true) {
		xQueueReceive(xQueueCylinder0Limit, &pos, portMAX_DELAY);
		while (getCylinderStartPos() != pos) {
			xQueueReceive(xQueueCylinder0Limit, &pos, 0);

			continue;
		}
		stopCylinderStart();
	}
}
void vTaskCylinder1Limit(void* pvParameters)
{
	int pos;
	while (true) {
		xQueueReceive(xQueueCylinder1Limit, &pos, portMAX_DELAY);
		while (getCylinder1Pos() != pos) {
			xQueueReceive(xQueueCylinder1Limit, &pos, 0);

			continue;
		}
		stopCylinder1();
	}
}
void vTaskCylinder2Limit(void* pvParameters)
{
	int pos;
	while (true) {
		xQueueReceive(xQueueCylinder2Limit, &pos, portMAX_DELAY);
		while (getCylinder2Pos() != pos) {
			xQueueReceive(xQueueCylinder2Limit, &pos, 0);

			continue;
		}
		stopCylinder2();
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

	//**********************************************************************************
	//MENUS
	xSemaphoreMenu = xSemaphoreCreateCounting(1, 1);

	//INSERIR BLOCOS
	xQueueBrickType = xQueueCreate(100, sizeof(int));
	xSemaphoreInsertBrick = xSemaphoreCreateCounting(1, 0);
	xSemaphoreStartBrickVerification = xSemaphoreCreateCounting(1, 0);
	xSemaphoreEndBrickVerification = xSemaphoreCreateCounting(1, 0);
	xQueueConfirmedBrickType = xQueueCreate(100, sizeof(int));
	xQueueBrickPassedCylinder1 = xQueueCreate(1, sizeof(int));
	xQueueBrickPassedCylinder2 = xQueueCreate(1, sizeof(int));
	xSemaphoreGetBlockCountCylinder1 = xSemaphoreCreateCounting(1, 0);
	xSemaphoreGetBlockCountCylinder2 = xSemaphoreCreateCounting(1, 0);
	xSemaphoreBlockBrickCountCylinder1 = xSemaphoreCreateCounting(1, 0);
	xSemaphoreBlockBrickCountCylinder2 = xSemaphoreCreateCounting(1, 0);
	xSemaphoreResumeBrickCountCylinder1 = xSemaphoreCreateCounting(1, 0);
	xSemaphoreResumeBrickCountCylinder2 = xSemaphoreCreateCounting(1, 0);

	//CALIBRAÇÃO
	xSemaphoreManualCalibration = xSemaphoreCreateCounting(1, 0);
	//CALIBRAÇÃO (Começa a 1 para calibrar no inicio)
	xSemaphoreCylinder0Calibration = xSemaphoreCreateCounting(1, 1);
	xSemaphoreCylinder1Calibration = xSemaphoreCreateCounting(1, 1);
	xSemaphoreCylinder2Calibration = xSemaphoreCreateCounting(1, 1);

	//Semaforos e Queues para manter os cilindros entre os sensores
	xSemaphoreCylinder0Limit = xSemaphoreCreateCounting(1, 0);
	xSemaphoreCylinder1Limit = xSemaphoreCreateCounting(1, 0);
	xSemaphoreCylinder2Limit = xSemaphoreCreateCounting(1, 0);
	xQueueCylinder0Limit = xQueueCreate(1, sizeof(int));
	xQueueCylinder1Limit = xQueueCreate(1, sizeof(int));
	xQueueCylinder2Limit = xQueueCreate(1, sizeof(int));

	//Semaforos para saber se é preciso ou não empurrar 
	xSemaphoreCylinder0Push = xSemaphoreCreateCounting(100, 0);
	xQueueCylinder1Push = xQueueCreate(100, sizeof(int));
	xQueueCylinder2Push = xQueueCreate(100, sizeof(int));

	//TOGGLES
	xSemaphoreConveyor = xSemaphoreCreateCounting(1, 1);
	xSemaphoreFlashingLamp = xSemaphoreCreateCounting(1, 0);

	//**********************************************************************************
	//MENUS
	xTaskCreate(vTaskMenu, "vTask_Menu", 100, NULL, 0, NULL);

	//CALIBRAÇÃO MANUAL
	xTaskCreate(vTaskManualCalibrationStart, "vTask_ManualCalibration", 100, NULL, 0, NULL);

	//INSERIR BLOCOS
	xTaskCreate(vTaskRegisterBrick, "vTask_RegisterBrick", 100, NULL, 0, NULL);
	xTaskCreate(vTaskHandleConfirmedBrick, "vTask_HandleBrick", 100, NULL, 0, NULL);

	//CONTADOR DE BLOCOS (Para saber quanto tem que ignorar)
	xTaskCreate(vTaskGetPassBlockCylinder1, "vTask_GetPassBlockCylinder1", 100, NULL, 0, NULL);
	xTaskCreate(vTaskGetPassBlockCylinder2, "vTask_GetPassBlockCylinder2", 100, NULL, 0, NULL);
	//DETEÇÃO DE BLOCOS
	xTaskCreate(vTaskBrickSensors, "vTask_BrickSensors", 100, NULL, 0, NULL);

	//LIMITAR CILINDROS ENTRE SENSORES
	xTaskCreate(vTaskCylinder0Limit, "vTask_Cylinder0Limit", 100, NULL, 0, NULL);
	xTaskCreate(vTaskCylinder1Limit, "vTask_Cylinder1Limit", 100, NULL, 0, NULL);
	xTaskCreate(vTaskCylinder2Limit, "vTask_Cylinder2Limit", 100, NULL, 0, NULL);

	//TOGGLES 
	xTaskCreate(vTaskConveyor, "vTask_Conveyor", 100, NULL, 0, NULL);
	xTaskCreate(vTaskFlashingLamp, "vTask_FlashingLamp", 100, NULL, 0, NULL);
	xTaskCreate(vTaskPushBlockCylinder0, "vTask_CylinderStart", 100, NULL, 0, NULL);
	xTaskCreate(vTaskPushBlockCylinder1, "vTask_Cylinder1", 100, NULL, 0, NULL);
	xTaskCreate(vTaskPushBlockCylinder2, "vTask_Cylinder2", 100, NULL, 0, NULL);

	//CALIBRAÇÃO
	xTaskCreate(vTaskCylinder0Calibration, "vTask_Cylinder0Calibration", 100, NULL, 0, NULL);
	xTaskCreate(vTaskCylinder1Calibration, "vTask_Cylinder1Calibration", 100, NULL, 0, NULL);
	xTaskCreate(vTaskCylinder2Calibration, "vTask_Cylinder2Calibration", 100, NULL, 0, NULL);

	//**********************************************************************************
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