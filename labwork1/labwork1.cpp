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

#define WAIT_TIME_CYLINDER0   25

#define PATH "C:/str/historic.txt"

xSemaphoreHandle xSemaphoreMenu;

xSemaphoreHandle xSemaphoreManualCalibration;

xSemaphoreHandle xSemaphoreConveyor;

xSemaphoreHandle xSemaphoreFlashingLampDeniedBlock;
xSemaphoreHandle xSemaphoreFlashingLampEmergency;
xSemaphoreHandle xSemaphoreStopFlashingLampEmergency;

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

xSemaphoreHandle xSemaphoreConveyorIsClear;

xSemaphoreHandle xSemaphoreShowHistoricMenu;
xSemaphoreHandle xSemaphoreShowHistoricAll;
xQueueHandle xQueueShowHistoricSpecific;

xQueueHandle xQueueSaveBrick;

xQueueHandle xQueueSavePort2;

xTaskHandle emergencyTask;
xTaskHandle resumeTask;
xTaskHandle blinkLight;
xTaskHandle restartSystem;

xTaskHandle taskHandle0;
xTaskHandle taskHandle1;
xTaskHandle taskHandle2;
xTaskHandle taskHandle3;
xTaskHandle taskHandle4;
xTaskHandle taskHandle5;
xTaskHandle taskHandle6;
xTaskHandle taskHandle7;
xTaskHandle taskHandle8;
xTaskHandle taskHandle9;
xTaskHandle taskHandle10;
xTaskHandle taskHandle11;
xTaskHandle taskHandle12;
xTaskHandle taskHandle13;
xTaskHandle taskHandle14;
xTaskHandle taskHandle15;
xTaskHandle taskHandle16;
xTaskHandle taskHandle17;
xTaskHandle taskHandle18;
xTaskHandle taskHandle19;
xTaskHandle taskHandle20;
xTaskHandle taskHandle21;
xTaskHandle taskHandle22;
xTaskHandle taskHandle23;
xTaskHandle taskHandle24;
xTaskHandle taskHandle25;
xTaskHandle taskHandle26;
xTaskHandle taskHandle27;
xTaskHandle taskHandle28;

struct brick {
	int type;
	bool rejected;
	tm date;
};

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
			xSemaphoreGive(xSemaphoreShowHistoricMenu);
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
	char brickType = 0;
	int message;

	while (true) {
		xSemaphoreTake(xSemaphoreInsertBrick, portMAX_DELAY);
		brickType = 0;

		system("cls");
		printf("\nEspera de blocos:\n");
		while (brickType != 27) {
			brickType = _getch();
			if (brickType >= '1' && brickType <= '3') {
				printf("Inserido bloco do tipo %d\n", brickType-'0');
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
	int brickAccepted = 1;
	int brickRejected = 0;
	brick newbrick;
	time_t t;
	while (true) {
		xQueueReceive(xQueueBrickType, &brickType, portMAX_DELAY);

		xSemaphoreGive(xSemaphoreCylinder0Push);
		xQueueReceive(xQueueConfirmedBrickType, &confirmedBrickType, portMAX_DELAY);
		
		//GET CURRENT TIME
		t = time(NULL);
		
		newbrick.type = confirmedBrickType;
		newbrick.rejected = false;
		newbrick.date = *localtime(&t);

		switch (brickType) {
		case 1:
			if (confirmedBrickType != 1) {
				xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
				xQueueSend(xQueueCylinder2Push, &brickRejected, portMAX_DELAY);
				xSemaphoreGive(xSemaphoreFlashingLampDeniedBlock);
				newbrick.rejected = true;
			}
			else {
				brickAccepted = 1;
				xQueueSend(xQueueCylinder1Push, &brickAccepted, portMAX_DELAY);
			}
			break;
		case 2:
			if (confirmedBrickType != 2) {
				xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
				xQueueSend(xQueueCylinder2Push, &brickRejected, portMAX_DELAY);
				xSemaphoreGive(xSemaphoreFlashingLampDeniedBlock);
				newbrick.rejected = true;
			}
			else {
				xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
				xQueueSend(xQueueCylinder2Push, &brickAccepted, portMAX_DELAY);
			}
			break;
		case 3:
			if (confirmedBrickType != 3) {
				xSemaphoreGive(xSemaphoreFlashingLampDeniedBlock);
				newbrick.rejected = true;
			}
			xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
			xQueueSend(xQueueCylinder2Push, &brickRejected, portMAX_DELAY);
			break;
		}
		xQueueSend(xQueueSaveBrick, &newbrick, portMAX_DELAY);
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

void vTaskFlashingLampDeniedBlock(void* pvParameters) {

	while (true) {
		xSemaphoreTake(xSemaphoreFlashingLampDeniedBlock, portMAX_DELAY);

		turnOnFlashingLamp();
		Sleep(3000);
		//Ligar durante 3 segundos
		turnOffFlashingLamp();
	}
}

void vTaskFlashingLampEmergency(void* pvParameters) {

	while (true) {
		xSemaphoreTake(xSemaphoreFlashingLampEmergency, portMAX_DELAY);

		while (!xSemaphoreTake(xSemaphoreStopFlashingLampEmergency, 0)) {
			turnOnFlashingLamp();
			Sleep(500);
			turnOffFlashingLamp();
			Sleep(500);
		}
	}
}

void vTaskConveyor(void* pvParameters) {
	while (true) {
		xSemaphoreTake(xSemaphoreConveyor, portMAX_DELAY);

		moveConveyor();
	}
}

/*void vTaskGetPassBlockCylinder1(void* pvParameters) {
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
*/

void vTaskPushBlockCylinder0(void* pvParameters)
{
	int sleepTime = WAIT_TIME_CYLINDER0;
	while (true) {
		xSemaphoreTake(xSemaphoreCylinder0Push, portMAX_DELAY);
		
		xSemaphoreGive(xSemaphoreStartBrickVerification);

		gotoCylinderStart(RIGHT);

		moveCylinderStartRight();
		vTaskDelay(sleepTime);
		stopCylinderStart();

		xSemaphoreGive(xSemaphoreEndBrickVerification);
		gotoCylinderStart(LEFT);
	}
}

void vTaskPushBlockCylinder1(void* pvParameters)
{
	int pushBrick;
	while (true) {
		xQueueReceive(xQueueCylinder1Push, &pushBrick, portMAX_DELAY);
		//xSemaphoreGive(xSemaphoreGetBlockCountCylinder1);
		//xQueueReceive(xQueueBrickPassedCylinder1, &passedCount, portMAX_DELAY);
		
		while (!isActiveCylinder1Sensor()) {
			continue;
		}
		if (pushBrick) {
			//xSemaphoreGive(xSemaphoreBlockBrickCountCylinder1);
			stopConveyor();
			gotoCylinder1(FRONT);
			gotoCylinder1(BACK);
			xSemaphoreGive(xSemaphoreConveyor);
			//xSemaphoreGive(xSemaphoreResumeBrickCountCylinder1);
		}
		while (isActiveCylinder1Sensor()) {
			continue;
		}
	}
}

void vTaskPushBlockCylinder2(void* pvParameters)
{
	int pushBrick;
	while (true) {
		xQueueReceive(xQueueCylinder2Push, &pushBrick, portMAX_DELAY);
		//xSemaphoreGive(xSemaphoreGetBlockCountCylinder1);
		//xQueueReceive(xQueueBrickPassedCylinder1, &passedCount, portMAX_DELAY);

		while (!isActiveCylinder2Sensor()) {
			continue;
		}
		if (pushBrick) {
			//xSemaphoreGive(xSemaphoreBlockBrickCountCylinder1);
			stopConveyor();
			gotoCylinder2(FRONT);
			gotoCylinder2(BACK);
			xSemaphoreGive(xSemaphoreConveyor);
			//xSemaphoreGive(xSemaphoreResumeBrickCountCylinder1);
		}
		while (isActiveCylinder2Sensor()) {
			continue;
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

void vTaskSaveBrickFile(void* pvParameters) {
	FILE* fp;

	brick brickToSave;
	struct tm tm; //Struct para definir o tempo
	char rejected[4];
	while (true)
	{
		xQueueReceive(xQueueSaveBrick, &brickToSave, portMAX_DELAY);
		fp = fopen(PATH, "a");
			
		if (fp == NULL) {
			printf("Error opening file, couldn't save current brick\n");
			break;
		}
		tm = brickToSave.date;

		if (brickToSave.rejected)
			strcpy(rejected, "YES");
		else
			strcpy(rejected, "NO");

		fprintf(fp, "%d\t%s\t%d-%02d-%02d %02d:%02d:%02d\n", brickToSave.type, 
			rejected, 
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		fclose(fp);
	}
}

void vTaskHistoricMenu(void* pvParameters) {
	int tecla = 0;
	int brickType;

	while (true) {
		xSemaphoreTake(xSemaphoreShowHistoricMenu,portMAX_DELAY);
		tecla = 0;
		system("cls");
		printf("1 - Mostrar blocos do tipo 1\n");
		printf("2 - Mostrar blocos do tipo 2\n");
		printf("3 - Mostrar blocos do tipo 3\n");
		printf("t - Mostrar todos os blocos\n");

		tecla = _getch();
		switch (tecla) {
			case 't':
				xSemaphoreGive(xSemaphoreShowHistoricAll);
				break;
			case '1':
			case '2':
			case '3':
				brickType = tecla - '0';
				xQueueSend(xQueueShowHistoricSpecific, &brickType, portMAX_DELAY);
				break;
			case 27: //esc
				xSemaphoreGive(xSemaphoreMenu);
				break;
			default:
				xSemaphoreGive(xSemaphoreShowHistoricMenu);
		}
	}
}

void vTaskShowHistoric(void* pvParameters) {
	FILE* fp;
	char c;
	int tecla = 0;
	while (true) {

		xSemaphoreTake(xSemaphoreShowHistoricAll,portMAX_DELAY);
		tecla = 0;

		system("cls");
		fp = fopen(PATH, "r");

		c = fgetc(fp);
		while (c != EOF)
		{
			printf("%c", c);
			c = fgetc(fp);
		}
		printf("Press ESC to exit");
		
		while (tecla != 27) {
			tecla = _getch();
		}
		xSemaphoreGive(xSemaphoreShowHistoricMenu);
	}

}

void vTaskShowBlockHistoric(void* pvParameters) {
	FILE* fp;
	int tecla = 0;
	int brickType;
	char rejected[4] = {0};
	bool rejectedCount=0;
	int brickCount = 0;
	int finished = 0;
	while (true) {
		xQueueReceive(xQueueShowHistoricSpecific, &brickType, portMAX_DELAY);
		brickCount = 0;
		rejectedCount = 0;

		system("cls");

		fp = fopen(PATH, "r");

		int bufferLength = 127;
		char buffer[127];
		while (fgets(buffer, bufferLength, fp)) {
			if (buffer[0] == brickType + '0') {
				printf("%s", buffer);
				brickCount++;
				//asterisco quer dizer que não vai ser guardado na variável
				sscanf(buffer,"%*d\t%3s\t%*d-%*02d-%*02d %*02d:%*02d:%*02d\n", &rejected);
				if (!strcmp(rejected, "YES")) {
					rejectedCount++;
				}

			}
		}
		printf("\nBlocos do tipo %d colocados: %d\n", brickType, brickCount);
		printf("Blocos do tipo %d aceites: %d\n", brickType, brickCount - rejectedCount);
		printf("Blocos do tipo %d rejeitados: %d\n", brickType, rejectedCount);

		fclose(fp);
		printf("Press ESC to exit");
		tecla = 0;
		while (tecla != 27) {
			tecla = _getch();
		}
		xSemaphoreGive(xSemaphoreShowHistoricMenu);
	}
}

void vTaskEmergency(void* pvParameters)
{
	while (true) {
		vTaskSuspend(NULL);
		printf("\nENTERED EMERGENCY MODE\n");
		
		uInt8 p2 = GetP2();
		xQueueSend(xQueueSavePort2,&p2,portMAX_DELAY);

		stopConveyor();
		stopCylinderStart();
		stopCylinder1();
		stopCylinder2();

		vTaskSuspend(taskHandle0);
		vTaskSuspend(taskHandle1);
		vTaskSuspend(taskHandle2);
		vTaskSuspend(taskHandle3);
		vTaskSuspend(taskHandle4);
		vTaskSuspend(taskHandle5);
		vTaskSuspend(taskHandle6);
		vTaskSuspend(taskHandle7);
		vTaskSuspend(taskHandle8);
		vTaskSuspend(taskHandle9);
		vTaskSuspend(taskHandle10);
		vTaskSuspend(taskHandle11);
		vTaskSuspend(taskHandle12);
		vTaskSuspend(taskHandle13);
		vTaskSuspend(taskHandle14);
		vTaskSuspend(taskHandle15);
		vTaskSuspend(taskHandle16);
		vTaskSuspend(taskHandle17);
		vTaskSuspend(taskHandle18);
		vTaskSuspend(taskHandle19);
		vTaskSuspend(taskHandle20);
		vTaskSuspend(taskHandle21);
		xSemaphoreGive(xSemaphoreFlashingLampEmergency);
	}
}
void vTaskResumeEmergency(void* pvParameters)
{
	uInt8 p2;
	while (true) {
		vTaskSuspend(NULL);
		printf("\nEXITED EMERGENCY MODE\n");

		while (!xSemaphoreTake(xSemaphoreConveyorIsClear, 0)) {
			continue;
		}
		xSemaphoreGive(xSemaphoreConveyorIsClear);

		vTaskResume(taskHandle0);
		vTaskResume(taskHandle1);
		vTaskResume(taskHandle2);
		vTaskResume(taskHandle3);
		vTaskResume(taskHandle4);
		vTaskResume(taskHandle5);
		vTaskResume(taskHandle6);
		vTaskResume(taskHandle7);
		vTaskResume(taskHandle8);
		vTaskResume(taskHandle9);
		vTaskResume(taskHandle10);
		vTaskResume(taskHandle11);
		vTaskResume(taskHandle12);
		vTaskResume(taskHandle13);
		vTaskResume(taskHandle14);
		vTaskResume(taskHandle15);
		vTaskResume(taskHandle16);
		vTaskResume(taskHandle17);
		vTaskResume(taskHandle18);
		vTaskResume(taskHandle19);
		vTaskResume(taskHandle20);
		vTaskResume(taskHandle21);
		xQueueReceive(xQueueSavePort2, &p2, portMAX_DELAY);
		ModifyP2(p2);
		
		xSemaphoreGive(xSemaphoreStopFlashingLampEmergency);
	}
}void vTaskRestartSystem(void* pvParameters)
{
	FILE* fp;
	int tmp=0;

	while (true) {
		vTaskSuspend(NULL);
		printf("\nRESTART SYSTEM\n");
		xSemaphoreTake(xSemaphoreConveyorIsClear,portMAX_DELAY);

		//write nada no file
		fp = fopen(PATH, "w");
		fprintf(fp, "");
		fclose(fp);

		//calibrate
		xSemaphoreGive(xSemaphoreCylinder0Calibration);
		xSemaphoreGive(xSemaphoreCylinder1Calibration);
		xSemaphoreGive(xSemaphoreCylinder2Calibration);

		//clean queue
		xQueueReset(xQueueBrickType);

		//ligar conveyor bue tempo
		moveConveyor();
		Sleep(10000);
		xSemaphoreGive(xSemaphoreConveyorIsClear);
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

void switch1_rising_isr(ULONGLONG lastTime) {
	BaseType_t xYieldRequired;
	// Resume the suspended task.
	xYieldRequired = xTaskResumeFromISR(emergencyTask);
}

void switch2_rising_isr(ULONGLONG lastTime) {
	BaseType_t xYieldRequired;
	// Resume the suspended task.
	xYieldRequired = xTaskResumeFromISR(resumeTask);
}

void switch3_rising_isr(ULONGLONG lastTime) {
	BaseType_t xYieldRequired;
	// Resume the suspended task.
	xYieldRequired = xTaskResumeFromISR(restartSystem);
}

void myDaemonTaskStartupHook(void) {
	
	inicializarPortos();

	attachInterrupt(1, 4, switch1_rising_isr, RISING);
	attachInterrupt(1, 3, switch2_rising_isr, RISING);
	attachInterrupt(1, 2, switch3_rising_isr, RISING);

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

	//HISTORICO
	xQueueSaveBrick = xQueueCreate(100, sizeof(brick));
	xSemaphoreShowHistoricMenu = xSemaphoreCreateCounting(1, 0);
	xSemaphoreShowHistoricAll = xSemaphoreCreateCounting(1, 0);
	xQueueShowHistoricSpecific = xQueueCreate(1, sizeof(int));

	//TOGGLES
	xSemaphoreConveyor = xSemaphoreCreateCounting(1, 1);
	xSemaphoreFlashingLampDeniedBlock = xSemaphoreCreateCounting(1, 0);
	xSemaphoreFlashingLampEmergency = xSemaphoreCreateCounting(1, 0);
	xSemaphoreStopFlashingLampEmergency = xSemaphoreCreateCounting(1, 0);

	xSemaphoreConveyorIsClear = xSemaphoreCreateCounting(1, 1);
	xQueueSavePort2 = xQueueCreate(1, sizeof(uInt8));

	//**********************************************************************************
	//MENUS
	xTaskCreate(vTaskMenu, "vTask_Menu", 100, NULL, 0, &taskHandle0);

	//CALIBRAÇÃO MANUAL
	xTaskCreate(vTaskManualCalibrationStart, "vTask_ManualCalibration", 100, NULL, 0, &taskHandle1);

	//INSERIR BLOCOS
	xTaskCreate(vTaskRegisterBrick, "vTask_RegisterBrick", 100, NULL, 0, &taskHandle2);
	xTaskCreate(vTaskHandleConfirmedBrick, "vTask_HandleBrick", 100, NULL, 0, &taskHandle3);

	//CONTADOR DE BLOCOS (Para saber quanto tem que ignorar)
	//xTaskCreate(vTaskGetPassBlockCylinder1, "vTask_GetPassBlockCylinder1", 100, NULL, 0, &taskHandle4);
	//xTaskCreate(vTaskGetPassBlockCylinder2, "vTask_GetPassBlockCylinder2", 100, NULL, 0, &taskHandle5);
	//DETEÇÃO DE BLOCOS
	xTaskCreate(vTaskBrickSensors, "vTask_BrickSensors", 100, NULL, 0, &taskHandle6);

	//LIMITAR CILINDROS ENTRE SENSORES
	xTaskCreate(vTaskCylinder0Limit, "vTask_Cylinder0Limit", 100, NULL, 0, &taskHandle7);
	xTaskCreate(vTaskCylinder1Limit, "vTask_Cylinder1Limit", 100, NULL, 0, &taskHandle8);
	xTaskCreate(vTaskCylinder2Limit, "vTask_Cylinder2Limit", 100, NULL, 0, &taskHandle9);

	//HISTORICO
	xTaskCreate(vTaskSaveBrickFile, "vTask_SaveBrickFile", 100, NULL, 0, &taskHandle10);

	//TOGGLES 
	xTaskCreate(vTaskConveyor, "vTask_Conveyor", 100, NULL, 0, &taskHandle11);
	xTaskCreate(vTaskFlashingLampDeniedBlock, "vTask_FlashingLamp", 100, NULL, 0, &taskHandle12);
	xTaskCreate(vTaskPushBlockCylinder0, "vTask_CylinderStart", 100, NULL, 0, &taskHandle13);
	xTaskCreate(vTaskPushBlockCylinder1, "vTask_Cylinder1", 100, NULL, 0, &taskHandle14);
	xTaskCreate(vTaskPushBlockCylinder2, "vTask_Cylinder2", 100, NULL, 0, &taskHandle15);

	//CALIBRAÇÃO
	xTaskCreate(vTaskCylinder0Calibration, "vTask_Cylinder0Calibration", 100, NULL, 0, &taskHandle16);
	xTaskCreate(vTaskCylinder1Calibration, "vTask_Cylinder1Calibration", 100, NULL, 0, &taskHandle17);
	xTaskCreate(vTaskCylinder2Calibration, "vTask_Cylinder2Calibration", 100, NULL, 0, &taskHandle18);

	xTaskCreate(vTaskShowHistoric, "vTask_ShowHistoric", 100, NULL, 0, &taskHandle19);
	xTaskCreate(vTaskHistoricMenu, "vTask_HistoricMenu", 100, NULL, 0, &taskHandle20);
	xTaskCreate(vTaskShowBlockHistoric, "vTask_ShowBlockHistoric", 100, NULL, 0, &taskHandle21);
	
	xTaskCreate(vTaskEmergency, "vTask_Emergency", 100, NULL, 0, &emergencyTask);
	xTaskCreate(vTaskResumeEmergency, "vTask_ResumeEmergency", 100, NULL, 0, &resumeTask);
	xTaskCreate(vTaskFlashingLampEmergency, "vTask_FlashingLampEmergency", 100, NULL, 0, &blinkLight);

	xTaskCreate(vTaskRestartSystem, "vTask_RestartSystem", 100, NULL, 0, &restartSystem);
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