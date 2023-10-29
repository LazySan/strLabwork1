// labwork1.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.

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

#define WAIT_TIME_CYLINDER0   50

#define PATH "C:/str/historic.txt"

//**********************************************************************************
//MENUS
xSemaphoreHandle xSemaphoreMenu;

//INSERIR BLOCOS
xSemaphoreHandle xSemaphoreInsertBrick;
xQueueHandle xQueueBrickType;
xQueueHandle xQueueConfirmedBrickType;
xSemaphoreHandle xSemaphoreStartBrickVerification;
xSemaphoreHandle xSemaphoreEndBrickVerification;

//CALIBRAÇÃO
xSemaphoreHandle xSemaphoreManualCalibration;
xSemaphoreHandle xSemaphoreCylinder0Calibration;
xSemaphoreHandle xSemaphoreCylinder1Calibration;
xSemaphoreHandle xSemaphoreCylinder2Calibration;

//LIMITAR SENSORES
xQueueHandle xQueueCylinder0Limit;
xQueueHandle xQueueCylinder1Limit;
xQueueHandle xQueueCylinder2Limit;

//ATIVAR CILINDROS
xSemaphoreHandle xSemaphoreCylinder0Push;
xQueueHandle xQueueCylinder1Push;
xQueueHandle xQueueCylinder2Push;

//HISTORICO
xQueueHandle xQueueSaveBrick;
xSemaphoreHandle xSemaphoreShowHistoricMenu;
xSemaphoreHandle xSemaphoreShowHistoricAll;
xQueueHandle xQueueShowHistoricSpecific;

//TOGGLES
xSemaphoreHandle xSemaphoreConveyor;
xSemaphoreHandle xSemaphoreFlashingLampDeniedBlock;
xSemaphoreHandle xSemaphoreFlashingLampEmergency;
xSemaphoreHandle xSemaphoreStopFlashingLampEmergency;

//INTERRUPTS
xQueueHandle xQueueSavePort2;
//**********************************************************************************

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
	int expected;
	int type;
	bool rejected;
	tm date;
};

//********************************
// vTaskMenu
// 
// Gere o menu inicial, chamando as outras funções com semáforos
//********************************
void vTaskMenu(void* pvParameters) {

	while (true) {

		xSemaphoreTake(xSemaphoreMenu, portMAX_DELAY);

		int tecla = 0;
		system("cls");
		printf("\n\tMenu");
		printf("\nc => Calibrar cilindros");
		printf("\nb => Inserir blocos");
		printf("\nm => Controlo manual");
		printf("\nh => Mostrar historico\n");

		tecla = _getch();
		switch (tecla) {

		case 'c':
			//Calibrar cilindros
			xSemaphoreGive(xSemaphoreCylinder0Calibration);
			xSemaphoreGive(xSemaphoreCylinder1Calibration);
			xSemaphoreGive(xSemaphoreCylinder2Calibration);

			//Voltar a abrir este menu
			xSemaphoreGive(xSemaphoreMenu);
			break;

		case 'b':
			//Chamar o menu dos blocos inseridos
			xSemaphoreGive(xSemaphoreInsertBrick);
			break;

		case 'm':
			//Chamar o menu do controlo manual
			xSemaphoreGive(xSemaphoreManualCalibration);
			break;

		case 'h':
			//Chamar o menu do histórico
			xSemaphoreGive(xSemaphoreShowHistoricMenu);
			break;

			//Ignorar Esc
		case 27:
			printf("\nSaida do programa");
			exit(1);
			break;

			//Qualquer outra tecla ele volta ao menu atual
		default:
			xSemaphoreGive(xSemaphoreMenu);
			break;
		}
	}
}

//********************************
// vTaskManualCalibrationStart
// 
// Deixa o utilizador controlar os cilindros manualmente
//********************************
void vTaskManualCalibrationStart(void* pvParameters) {

	int pos0, pos1, pos2;
	int tecla = 0;
	while (true) {

		//Espera pelo semaforo xSemaphoreManualCalibration
		xSemaphoreTake(xSemaphoreManualCalibration, portMAX_DELAY);

		//Limpa o terminal
		system("cls");
		printf("\nEntrada em modo manual");
		printf("\nComandos:\n");
		printf("\ntecla q => move Cilindro Start para esquerda");
		printf("\ntecla a => move Cilindro Start para direita");
		printf("\ntecla z => Para cilindro start\n");
		printf("\ntecla w => Mover Cilindro1 para tras");
		printf("\ntecla s => Mover Cilindro1 para frente");
		printf("\ntecla x => Para cilindro1\n");
		printf("\ntecla e => Mover Cilindro2 para tras");
		printf("\ntecla d => Mover Cilindro2 para frente");
		printf("\ntecla c => Para cilindro2\n");




		//Reseta a variável
		tecla = 0;
		while (tecla != 27) {
			tecla = _getch();
			switch (tecla) {

				//Cylinder0
			case 'q':

				//Mover CilindroStart para a esquerda
				moveCylinderStartLeft();
				pos0 = LEFT;

				//Avisa queue que o cilindro está a ir para a esquerda
				//para limitar o movimento do cilindro
				xQueueOverwrite(xQueueCylinder0Limit, &pos0);

				break;
			case 'a':

				//Mover CilindroStart para a direita
				moveCylinderStartRight();
				pos0 = RIGHT;

				//Avisa queue que o cilindro está a ir para a direita
				//para limitar o movimento do cilindro
				xQueueOverwrite(xQueueCylinder0Limit, &pos0);

				break;
			case 'z':

				//Para cilindro start
				stopCylinderStart();
				break;

				//Cylinder1
			case 'w':

				//Mover Cilindro1 para trás
				moveCylinder1Back();
				pos1 = BACK;

				//Avisa queue que o cilindro está a ir para trás
				//para limitar o movimento do cilindro
				xQueueOverwrite(xQueueCylinder1Limit, &pos1);

				break;
			case 's':

				//Mover Cilindro1 para frente
				moveCylinder1Front();
				pos1 = FRONT;

				//Avisa queue que o cilindro está a ir para a frente
				//para limitar o movimento do cilindro
				xQueueOverwrite(xQueueCylinder1Limit, &pos1);

				break;
			case 'x':

				//Para cilindro1
				stopCylinder1();

				break;

				//Cylinder2
			case 'e':

				//Mover Cilindro2 para trás
				moveCylinder2Back();
				pos2 = BACK;

				//Avisa queue que o cilindro está a ir para a frente
				//para limitar o movimento do cilindro
				xQueueOverwrite(xQueueCylinder2Limit, &pos2);

				break;
			case 'd':

				//Mover Cilindro2 para frente
				moveCylinder2Front();
				pos2 = FRONT;

				//Avisa queue que o cilindro está a ir para trás
				//para limitar o movimento do cilindro
				xQueueOverwrite(xQueueCylinder2Limit, &pos2);

				break;
			case 'c':

				//Para cilindro2
				stopCylinder2();

				break;

				//Ignorar Esc
			case 27:
				break;

			default:
				//Qualquer outro comando só volta no switch
				printf("\nComando nao reconhecido");
				break;
			}
		}
		printf("\nSaida do modo manual:");

		//Voltar para o menu principal
		xSemaphoreGive(xSemaphoreMenu);
	}
}

//********************************
// vTaskRegisterBrick
// 
// Gere o menu de registro de bricks
//********************************
void vTaskRegisterBrick(void* pvParameters) {

	char brickType = 0;
	int message;

	while (true) {
		//Espera pelo semaforo xSemaphoreInsertBrick
		xSemaphoreTake(xSemaphoreInsertBrick, portMAX_DELAY);

		//Reseta a variável
		brickType = 0;

		//Limpa o terminal
		system("cls");
		printf("\nEspera de blocos:\n");
		while (brickType != 27) {
			brickType = _getch();

			//Bricks só podem ser do tipo 1, 2 ou 3
			if (brickType >= '1' && brickType <= '3') {

				printf("Inserido bloco do tipo %d\n", brickType - '0');

				//Passar de char para int com - '0'
				message = brickType - '0';

				//Enviar tipo de brick para o menu que gere os blocos
				xQueueSend(xQueueBrickType, &message, portMAX_DELAY);
			}
			else {
				//Brick que foi inserido não existe
				printf("Tipo de bloco nao reconhecido\n");
			}
		}
		//Voltar para o menu principal
		xSemaphoreGive(xSemaphoreMenu);
	}
}

//********************************
// vTaskHandleConfirmedBrick
// 
// Verifica se os bricks são ou não realmente os esperados, e manda ordens aos cilindros
//********************************
void vTaskHandleConfirmedBrick(void* pvParameters) {
	int brickType;
	int confirmedBrickType;
	bool brickAccepted = true;
	bool brickRejected = false;
	brick newbrick;
	time_t t;
	while (true) {
		//Espera pela fila xQueueBrickType
		//brickType vai ter o bloco esperado (o que o utilizador inseriu no teclado)
		xQueueReceive(xQueueBrickType, &brickType, portMAX_DELAY);

		//Ordena o CilinderStart a empurrar o brick
		xSemaphoreGive(xSemaphoreCylinder0Push);

		//Espera pela fila xQueueConfirmedBrickType
		//confirmedBrickType vai ter o bloco que realmente entrou (verificado pelos sensores)
		xQueueReceive(xQueueConfirmedBrickType, &confirmedBrickType, portMAX_DELAY);

		//Guardar variáveis na estrutura para depois guardar no histórico
		newbrick.expected = brickType;
		newbrick.type = confirmedBrickType;
		newbrick.rejected = false;

		//Obter o tempo atual para guardar na estrutura
		t = time(NULL);
		newbrick.date = *localtime(&t);

		switch (brickType) {
		case 1:
			//Utilizador inseriu 1

			//Se o confirmado não for 1, vamos rejeitar o brick
			if (confirmedBrickType != 1) {

				//Dizer ao cilindro1 e cilindro2 para ignorarem o brick inserido
				xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
				xQueueSend(xQueueCylinder2Push, &brickRejected, portMAX_DELAY);

				//Acender a LED que indica que o brick que foi inserido foi rejeitado
				xSemaphoreGive(xSemaphoreFlashingLampDeniedBlock);

				//Atualizar variável que está a false por default
				newbrick.rejected = true;
			}

			//O brick que o utilizador inseriu foi confirmado, coloca-lo na dock certa
			else {
				//Dizer ao cilindro1 que o próximo brick é para ele
				xQueueSend(xQueueCylinder1Push, &brickAccepted, portMAX_DELAY);
			}

			break;

		case 2:
			//Utilizador inseriu 2

			//Se o confirmado não for 2, vamos rejeitar o brick
			if (confirmedBrickType != 2) {

				//Dizer ao cilindro1 e cilindro2 para ignorarem o brick inserido
				xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
				xQueueSend(xQueueCylinder2Push, &brickRejected, portMAX_DELAY);

				//Acender a LED que indica que o brick que foi inserido foi rejeitado
				xSemaphoreGive(xSemaphoreFlashingLampDeniedBlock);

				//Atualizar variável que está a false por default
				newbrick.rejected = true;
			}

			//O brick que o utilizador inseriu foi confirmado, coloca-lo na dock certa
			else {
				//Dizer ao cilindro2 que o próximo brick é para ele e ao cilindro1 para o deixar passar
				xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
				xQueueSend(xQueueCylinder2Push, &brickAccepted, portMAX_DELAY);
			}
			break;

		case 3:
			//Utilizador inseriu 3

			//Se o confirmado não for 3, vamos rejeitar o brick
			if (confirmedBrickType != 3) {
				//Acender a LED que indica que o brick que foi inserido foi rejeitado
				xSemaphoreGive(xSemaphoreFlashingLampDeniedBlock);
				//Atualizar variável que está a false por default
				newbrick.rejected = true;
			}
			//Dizer ao cilindro1 e cilindro2 para ignorarem o brick inserido
			xQueueSend(xQueueCylinder1Push, &brickRejected, portMAX_DELAY);
			xQueueSend(xQueueCylinder2Push, &brickRejected, portMAX_DELAY);

			break;
		}

		//Guardar o brick contido na estrutura
		xQueueSend(xQueueSaveBrick, &newbrick, portMAX_DELAY);
	}
}

//********************************
// vTaskBrickSensors
// 
// Controla os sensores de verificação de bricks
//********************************
void vTaskBrickSensors(void* pvParameters) {
	int brickType;
	bool sensorUpCheck;
	bool sensorDownCheck;
	bool sensorReseted;
	while (true) {
		//Espera pelo semaforo xSemaphoreStartBrickVerification
		xSemaphoreTake(xSemaphoreStartBrickVerification, portMAX_DELAY);

		//Resetar variáveis
		brickType = 1;
		sensorUpCheck = false;
		sensorDownCheck = false;
		sensorReseted = false;

		//Enquanto o semáforo xSemaphoreEndBrickVerification não estiver ativo, verificar os sensores
		while (!xSemaphoreTake(xSemaphoreEndBrickVerification, 0)) {

			//Enquanto o sensor ainda nao tenha sido resetado
			while (!sensorReseted)
			{
				//Esperamos ambos os sensores apagaram para o resetar
				//isto simboliza o intervalo entre bricks
				if (!isActiveUpBrickSensor() && !isActiveDownBrickSensor()) {
					sensorReseted = true;
				}
			}

			//Sabendo que há 2 sensores concluimos o seguinte
			//Brick 1 -> 0 sensores
			//Brick 2 -> 1 sensor
			//Brick 3 -> 2 sensor
			//Brick = sensores+1
			//Já que brickType é inicializado a 1, vamos somar a quantidade de sensores ativos


			//Se o sensor de cima ainda nao tenha sido visto
			if (!sensorUpCheck) {
				//Se ele tiver ativo
				if (isActiveUpBrickSensor()) {

					//Incrementar o tipo de brick
					brickType++;

					//Este sensor já foi ativo
					//Esta variável serve para evitar ver o sensor mais que uma vez
					sensorUpCheck = true;

				}
			}

			//Se o sensor de baixo ainda nao tenha sido visto
			if (!sensorDownCheck) {
				//Se ele tiver ativo
				if (isActiveDownBrickSensor()) {

					//Incrementar o tipo de brick
					brickType++;


					//Este sensor já foi ativo
					//Esta variável serve para evitar ver o sensor mais que uma vez
					sensorDownCheck = true;
				}
			}
		}

		//Devolver o brick confirmado pelos sensores ao menu de registro de bricks
		xQueueSend(xQueueConfirmedBrickType, &brickType, portMAX_DELAY);
	}
}

//********************************
// vTaskFlashingLampDeniedBlock
// 
// Ligar a LED a avisar que o brick foi recusado
//********************************
void vTaskFlashingLampDeniedBlock(void* pvParameters) {

	while (true) {
		//Esperar pelo semaforo xSemaphoreFlashingLampDeniedBlock
		xSemaphoreTake(xSemaphoreFlashingLampDeniedBlock, portMAX_DELAY);

		//Ligar a LED
		turnOnFlashingLamp();
		//Esperar 3 segundos
		Sleep(3000);
		//Desligar a LED
		turnOffFlashingLamp();
	}
}

//********************************
// vTaskFlashingLampEmergency
// 
// Piscar o LED a avisar que está em modo emergência
//********************************
void vTaskFlashingLampEmergency(void* pvParameters) {

	while (true) {
		//Esperar pelo semaforo xSemaphoreFlashingLampEmergency
		xSemaphoreTake(xSemaphoreFlashingLampEmergency, portMAX_DELAY);

		//Enquanto não receber o semaforo para parar xSemaphoreStopFlashingLampEmergency
		while (!xSemaphoreTake(xSemaphoreStopFlashingLampEmergency, 0)) {

			//Ligar a LED
			turnOnFlashingLamp();
			//Esperar 500 milisegundos
			Sleep(500);
			//Desligar a LED
			turnOffFlashingLamp();
			//Esperar 500 milisegundos
			Sleep(500);
		}
	}
}

//********************************
// vTaskConveyor
// 
// Ligar conveyor
//********************************
void vTaskConveyor(void* pvParameters) {
	while (true) {
		//Esperar pelo semaforo xSemaphoreConveyor
		xSemaphoreTake(xSemaphoreConveyor, portMAX_DELAY);

		//Ligar conveyor
		moveConveyor();
	}
}

//********************************
// vTaskPushBlockCylinder0
// 
// Gerencia o cilindro0 para empurrar um brick
//********************************
void vTaskPushBlockCylinder0(void* pvParameters)
{
	//sleepTime vai ter uma constante inicializada no define
	int sleepTime = WAIT_TIME_CYLINDER0;
	//Explicação sleepTime:
	//Ao colocar bricks no cilindro0 os bricks não iam diretamente para a conveyor, então
	//adicionar um delay antes de parar o cilindro0 faz com que os bricks vão para a conveyor

	while (true) {
		//Esperar pelo semaforo xSemaphoreCylinder0Push
		xSemaphoreTake(xSemaphoreCylinder0Push, portMAX_DELAY);

		//Dizer aos sensores para perceberem que brick vai passar
		xSemaphoreGive(xSemaphoreStartBrickVerification);

		//Dizer ao Cilindro0 para ir para o sensor da direita
		gotoCylinderStart(RIGHT);

		//Andar mais um bocadinho por causa daquilo escrito acima
		moveCylinderStartRight();
		vTaskDelay(sleepTime);

		//Parar o cilindro
		stopCylinderStart();

		//Dizer aos sensores para enviarem para o menu dos bricks colocados qual foi o brick que eles detetaram
		xSemaphoreGive(xSemaphoreEndBrickVerification);

		//Dizer ao Cilindro0 para ir para o sensor da esquerda
		gotoCylinderStart(LEFT);
	}
}

//********************************
// vTaskPushBlockCylinder1
// 
// Gerencia o cilindro1 para empurrar um brick
//********************************
void vTaskPushBlockCylinder1(void* pvParameters)
{
	bool pushBrick;
	while (true) {
		//Esperar pela queue xQueueCylinder1Push
		xQueueReceive(xQueueCylinder1Push, &pushBrick, portMAX_DELAY);

		//Esperar que o sensor fique ativo
		while (!isActiveCylinder1Sensor()) {
			continue;
		}

		//Verificar se este brick é para este cilindro ou não
		if (pushBrick) {
			//Se sim, empurrá-lo

			//Parar conveyor
			stopConveyor();

			//Mover cilindro
			gotoCylinder1(FRONT);
			gotoCylinder1(BACK);

			//Ligar conveyor
			xSemaphoreGive(xSemaphoreConveyor);
		}

		//Esperar até o sensor ficar desativo para voltar ao inicio
		//Isto serve mais para ter a certeza que não há bugs quando os bricks estão muito juntos uns dos outros
		while (isActiveCylinder1Sensor()) {
			continue;
		}
	}
}

//********************************
// vTaskPushBlockCylinder2
// 
// Gerencia o cilindro2 para empurrar um brick
//********************************
void vTaskPushBlockCylinder2(void* pvParameters)
{
	bool pushBrick;
	while (true) {
		//Esperar pela queue xQueueCylinder1Push
		xQueueReceive(xQueueCylinder2Push, &pushBrick, portMAX_DELAY);

		//Esperar que o sensor fique ativo
		while (!isActiveCylinder2Sensor()) {
			continue;
		}

		//Verificar se este brick é para este cilindro ou não
		if (pushBrick) {
			//Se sim, empurrá-lo

			//Parar conveyor
			stopConveyor();

			//Mover cilindro
			gotoCylinder2(FRONT);
			gotoCylinder2(BACK);

			//Ligar conveyor
			xSemaphoreGive(xSemaphoreConveyor);
		}

		//Esperar até o sensor ficar desativo para voltar ao inicio
		//Isto serve mais para ter a certeza que não há bugs quando os bricks estão muito juntos uns dos outros
		while (isActiveCylinder2Sensor()) {
			continue;
		}
	}
}

//********************************
// vTaskCylinder0Calibration
// 
// Calibra o cilindro0
//********************************
void vTaskCylinder0Calibration(void* pvParameters)
{
	while (true) {
		//Esperar pelo semaforo xSemaphoreCylinder0Calibration
		xSemaphoreTake(xSemaphoreCylinder0Calibration, portMAX_DELAY);

		//Calibrar cilindro0
		calibrationCylinderStart();
	}
}

//********************************
// vTaskCylinder1Calibration
// 
// Calibra o cilindro1
//********************************
void vTaskCylinder1Calibration(void* pvParameters)
{
	while (true) {
		//Esperar pelo semaforo xSemaphoreCylinder1Calibration
		xSemaphoreTake(xSemaphoreCylinder1Calibration, portMAX_DELAY);

		//Calibrar cilindro1
		calibrationCylinder1();
	}
}

//********************************
// vTaskCylinder2Calibration
// 
// Calibra o cilindro2
//********************************
void vTaskCylinder2Calibration(void* pvParameters)
{
	while (true) {
		//Esperar pelo semaforo xSemaphoreCylinder2Calibration
		xSemaphoreTake(xSemaphoreCylinder2Calibration, portMAX_DELAY);

		//Calibrar cilindro2
		calibrationCylinder2();
	}
}

//********************************
// vTaskCylinder0Limit
// 
// Mantém o cilindro0 entre os sensores
//********************************
void vTaskCylinder0Limit(void* pvParameters)
{
	int pos;
	while (true) {
		//Esperar pelo semaforo xQueueCylinder0Limit
		xQueueReceive(xQueueCylinder0Limit, &pos, portMAX_DELAY);

		//pos vai ter a posição para onde o cilindro se está a mover
		//Enquanto o cilindro não tenha chegado à sua posição final
		while (getCylinderStartPos() != pos) {
			//Esta xQueueReceive serve para atualizar caso o utilizador mude de direção a meio
			xQueueReceive(xQueueCylinder0Limit, &pos, 0);

			//Não fazer nada
			continue;
		}
		//Se o cilindro chegou ao sensor desejado, parar
		stopCylinderStart();
	}
}

//********************************
// vTaskCylinder1Limit
// 
// Mantém o cilindro1 entre os sensores
//********************************
void vTaskCylinder1Limit(void* pvParameters)
{
	int pos;
	while (true) {
		//Esperar pelo semaforo xQueueCylinder1Limit
		xQueueReceive(xQueueCylinder1Limit, &pos, portMAX_DELAY);

		//pos vai ter a posição para onde o cilindro se está a mover
		//Enquanto o cilindro não tenha chegado à sua posição final
		while (getCylinder1Pos() != pos) {
			//Esta xQueueReceive serve para atualizar caso o utilizador mude de direção a meio
			xQueueReceive(xQueueCylinder1Limit, &pos, 0);

			//Não fazer nada
			continue;
		}
		//Se o cilindro chegou ao sensor desejado, parar
		stopCylinder1();
	}
}

//********************************
// vTaskCylinder2Limit
// 
// Mantém o cilindro2 entre os sensores
//********************************
void vTaskCylinder2Limit(void* pvParameters)
{
	int pos;
	while (true) {
		//Esperar pelo semaforo xQueueCylinder2Limit
		xQueueReceive(xQueueCylinder2Limit, &pos, portMAX_DELAY);

		//pos vai ter a posição para onde o cilindro se está a mover
		//Enquanto o cilindro não tenha chegado à sua posição final
		while (getCylinder2Pos() != pos) {
			//Esta xQueueReceive serve para atualizar caso o utilizador mude de direção a meio
			xQueueReceive(xQueueCylinder2Limit, &pos, 0);

			//Não fazer nada
			continue;
		}
		//Se o cilindro chegou ao sensor desejado, parar
		stopCylinder2();
	}
}

//********************************
// vTaskSaveBrickFile
// 
// Guarda o brick armazenado no ficheiro
//********************************
void vTaskSaveBrickFile(void* pvParameters) {
	FILE* fp;
	brick brickToSave;
	struct tm tm; //Struct para definir o tempo
	char rejected[4];
	while (true)
	{
		//Esperar pelo brick que queremos salvar
		xQueueReceive(xQueueSaveBrick, &brickToSave, portMAX_DELAY);

		//Abrir o ficheiro em modo append
		//append significa que ele só adiciona informação, não dá overwrite
		fp = fopen(PATH, "a");

		//Erro a abrir o ficheiro
		if (fp == NULL) {
			printf("Erro a abrir ficheiro, brick atual não foi salvo\n");
			break;
		}

		//tm é a estrutura criado pelo C para guardar o tempo
		tm = brickToSave.date;

		//Passar de bool para string para guardar no ficheiro
		if (brickToSave.rejected)
			strcpy(rejected, "SIM");
		else
			strcpy(rejected, "NAO");

		//Escrever linha no ficheiro, %02d significa que vamos guardar 2 valores daquele inteiro
		//Por exemplo: 2 fica 02
		fprintf(fp, "%d\t\t%d\t\t%s\t\t%d-%02d-%02d %02d:%02d:%02d\n",
			brickToSave.expected,
			brickToSave.type,
			rejected,
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

		//Fechar o ficheiro
		fclose(fp);
	}
}

//********************************
// vTaskHistoricMenu
// 
// Menu do histórico
//********************************
void vTaskHistoricMenu(void* pvParameters) {
	int tecla = 0;
	int brickType;

	while (true) {
		//Esperar pelo semaforo xSemaphoreShowHistoricMenu
		xSemaphoreTake(xSemaphoreShowHistoricMenu, portMAX_DELAY);

		//Resetar variável
		tecla = 0;

		//Limpar o terminal
		system("cls");
		printf("\n1 - Mostrar blocos do tipo 1\n");
		printf("2 - Mostrar blocos do tipo 2\n");
		printf("3 - Mostrar blocos do tipo 3\n");
		printf("t - Mostrar todos os blocos\n");

		tecla = _getch();
		switch (tecla) {
		case 't':
			//Mostrar o historico de todos os bricks
			xSemaphoreGive(xSemaphoreShowHistoricAll);
			break;
		case '1': //Implementado em baixo
		case '2': //Implementado em baixo
		case '3':
			//Mostrar historico de um tipo de brick em especifico

			//Passar de char para int
			brickType = tecla - '0';

			//Enviar o tipo de brick que queremos ver o historico para xQueueShowHistoricSpecific
			xQueueSend(xQueueShowHistoricSpecific, &brickType, portMAX_DELAY);
			break;
		case 27: //esc
			//Voltar ao menu principal
			xSemaphoreGive(xSemaphoreMenu);
			break;
		default:
			//Uma tecla não implementada foi primida, voltar ao menu atual
			xSemaphoreGive(xSemaphoreShowHistoricMenu);
		}
	}
}

//********************************
// vTaskShowHistoric
// 
// Mostra o histórico de todos os bricks 
//********************************
void vTaskShowHistoric(void* pvParameters) {
	FILE* fp;
	char c;
	int tecla = 0;
	while (true) {

		//Esperar pela queue xSemaphoreShowHistoricAll
		xSemaphoreTake(xSemaphoreShowHistoricAll, portMAX_DELAY);

		//Resetar variável
		tecla = 0;

		//Limpar o terminal
		system("cls");

		//Abrir o ficheiro em modo read
		//Modo read apenas deixa ler o ficheiro sem editar
		fp = fopen(PATH, "r");

		//Print da linha de cima para alinhar a informação
		printf("Esperado\tRecebido\tRejeitado\tHora\n\n");

		//c estava nao inicializado, isto só serve para ele não ser comparado a null
		c = fgetc(fp);
		while (c != EOF)
		{
			//Enquanto o char nao for EOF (End of File), dar printf, isto vai dar print do ficheiro todo
			printf("%c", c);
			c = fgetc(fp);
		}
		printf("\nClique ESC para sair");

		//Enquanto a tecla nao for ESC
		while (tecla != 27) {
			tecla = _getch();
		}

		//Quando ESC for clicado, ir para o menu do histórico
		xSemaphoreGive(xSemaphoreShowHistoricMenu);
	}
}

//********************************
// vTaskShowHistoric
// 
// Mostra o histórico de um brick em especifico
//********************************
void vTaskShowBlockHistoric(void* pvParameters) {
	FILE* fp;
	int tecla = 0;
	int brickType;
	char rejected[4] = { 0 };
	int rejectedCount = 0;
	int brickCount = 0;
	int finished = 0;
	int bufferLength = 127;
	char buffer[127];
	while (true) {
		//Resetar variáveis 
		brickCount = 0;
		rejectedCount = 0;

		//Esperar pela queue xQueueShowHistoricSpecific
		xQueueReceive(xQueueShowHistoricSpecific, &brickType, portMAX_DELAY);

		//Limpar o terminal
		system("cls");

		//Abrir o ficheiro em modo read
		//Modo read apenas deixa ler o ficheiro sem editar
		fp = fopen(PATH, "r");

		//Print da linha de cima para alinhar a informação
		printf("Esperado\tRecebido\tRejeitado\tHora\n\n");

		//fgets vai buscar a linha toda do ficheiro, se for o final retorna falso, saindo do while
		while (fgets(buffer, bufferLength, fp)) {
			//Primeiro caracter da linha vai ser o brick esperado
			//Se o brick esperado for igual ao que o utilizador quer ver, mostrar a linha obtida
			if (buffer[0] == brickType + '0') {
				//Mostrar linha
				printf("%s", buffer);

				//Começar a estatistica (bricks aceites e rejeitados)
				//brickCount apenas serve para mostrar a estatistica no final desta função
				brickCount++;

				//Asteriscos servem para dizer o valor não vai ser guardado na variável
				//Só nos interessa a variavel rejected para a estatistica
				sscanf(buffer, "%*d\t\t%*d\t\t%s\t\t%*d-%*02d-%*02d %*02d:%*02d:%*02d\n", &rejected);

				//Se o rejected for SIM, incrementar rejectedCount
				if (!strcmp(rejected, "SIM")) {
					rejectedCount++;
				}
			}
		}
		//Mostrar a estatistica
		printf("\nBlocos do tipo %d colocados: %d\n", brickType, brickCount);
		printf("Blocos do tipo %d aceites: %d\n", brickType, brickCount - rejectedCount);
		printf("Blocos do tipo %d rejeitados: %d\n", brickType, rejectedCount);

		//Fechar o ficheiro
		fclose(fp);
		printf("\nClique ESC para sair");

		//Voltar a resetar a variável
		tecla = 0;

		//Enquanto o utilizador nao clica no ESC nao faz nada
		while (tecla != 27) {
			tecla = _getch();
		}

		//Voltar ao menu do historico
		xSemaphoreGive(xSemaphoreShowHistoricMenu);
	}
}

//********************************
// vTaskEmergency
// 
// Coloca o sistema em modo emergencia
//********************************
void vTaskEmergency(void* pvParameters)
{
	uInt8 p2;
	while (true) {
		//A task suspende-se a si mesma até o interrupt lhe mandar retomar
		vTaskSuspend(NULL);
		printf("\nMODO DE EMERGENCIA ATIVADO\n");

		//Guardar o valor de P2 (vai ter informação sobre o movimento do sistema)
		p2 = GetP2();
		xQueueSend(xQueueSavePort2, &p2, portMAX_DELAY);

		//Parar tudo oq se pode mover
		stopConveyor();
		stopCylinderStart();
		stopCylinder1();
		stopCylinder2();

		//Suspender todas as tasks (menos as de emergencia)
		vTaskSuspend(taskHandle0);
		vTaskSuspend(taskHandle1);
		vTaskSuspend(taskHandle2);
		vTaskSuspend(taskHandle3);
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

		//Mandar a LED começar a piscar
		xSemaphoreGive(xSemaphoreFlashingLampEmergency);
	}
}

//********************************
// vTaskResumeEmergency
// 
// Retira o sistema em modo emergencia
//********************************
void vTaskResumeEmergency(void* pvParameters)
{
	uInt8 p2;
	while (true) {
		//A task suspende-se a si mesma até o interrupt lhe mandar voltar
		vTaskSuspend(NULL);
		printf("\nMODO DE EMERGENCIA DESATIVADO\n");

		//Recuperar o valor de P2 antes da emergencia e recupera-lo
		xQueueReceive(xQueueSavePort2, &p2, portMAX_DELAY);
		ModifyP2(p2);

		//Retomar todas as tasks (menos as de emergencia)
		vTaskResume(taskHandle0);
		vTaskResume(taskHandle1);
		vTaskResume(taskHandle2);
		vTaskResume(taskHandle3);
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

		//Mandar a LED parar a piscar
		xSemaphoreGive(xSemaphoreStopFlashingLampEmergency);
	}
}

//********************************
// vTaskRestartSystem
// 
// Limpa o ficheiro do historico
//********************************
void vTaskRestartSystem(void* pvParameters)
{
	//Função não acabada

	FILE* fp;
	while (true) {
		//A task suspende-se a si mesma até o interrupt lhe mandar voltar
		vTaskSuspend(NULL);
		printf("\HISTORICO REINICIADO\n");

		//Limpar ficheiro
		fp = fopen(PATH, "w");
		fclose(fp);
	}

}

//********************************
// inicializarPortos
// 
// Inicializa os portos necessários para correr no simulador
//********************************
void inicializarPortos() {
	printf("\nwaiting for hardware simulator...");
	printf("\nReminding: gotoXZ requires kit calibration first...");
	createDigitalInput(0);
	createDigitalInput(1);
	createDigitalOutput(2);
	writeDigitalU8(2, 0);
	printf("\ngot access to simulator...");
}

//********************************
// switch1_rising_isr
// 
// Chamado quando DOCK1 é pressionado, chama a task de emergencia
//********************************
void switch1_rising_isr(ULONGLONG lastTime) {
	BaseType_t xYieldRequired;
	// Resume the suspended task.
	xYieldRequired = xTaskResumeFromISR(emergencyTask);
}

//********************************
// switch2_rising_isr
// 
// Chamado quando DOCK2 é pressionado, chama a task para sair de emergencia
//********************************
void switch2_rising_isr(ULONGLONG lastTime) {
	BaseType_t xYieldRequired;
	// Resume the suspended task.
	xYieldRequired = xTaskResumeFromISR(resumeTask);
}

//********************************
// switch3_rising_isr
// 
// Chamado quando DOCKEND é pressionado, chama a task para limpar o historico
//********************************
void switch3_rising_isr(ULONGLONG lastTime) {
	BaseType_t xYieldRequired;
	// Resume the suspended task.
	xYieldRequired = xTaskResumeFromISR(restartSystem);
}

//********************************
// myDaemonTaskStartupHook
// 
// Inicialização de todas as tasks, semaforos e queues
//********************************
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

	//CALIBRAÇÃO
	xSemaphoreManualCalibration = xSemaphoreCreateCounting(1, 0);

	//CALIBRAÇÃO (Começa a 1 para calibrar no inicio)
	xSemaphoreCylinder0Calibration = xSemaphoreCreateCounting(1, 1);
	xSemaphoreCylinder1Calibration = xSemaphoreCreateCounting(1, 1);
	xSemaphoreCylinder2Calibration = xSemaphoreCreateCounting(1, 1);

	//LIMITAR SENSORES
	xQueueCylinder0Limit = xQueueCreate(1, sizeof(int));
	xQueueCylinder1Limit = xQueueCreate(1, sizeof(int));
	xQueueCylinder2Limit = xQueueCreate(1, sizeof(int));

	//ATIVAR CILINDROS
	xSemaphoreCylinder0Push = xSemaphoreCreateCounting(100, 0);
	xQueueCylinder1Push = xQueueCreate(100, sizeof(bool));
	xQueueCylinder2Push = xQueueCreate(100, sizeof(bool));

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

	//INTERRUPTS
	xQueueSavePort2 = xQueueCreate(1, sizeof(uInt8));

	//**********************************************************************************
	//MENUS
	xTaskCreate(vTaskMenu, "vTask_Menu", 100, NULL, 0, &taskHandle0);

	//CALIBRAÇÃO MANUAL
	xTaskCreate(vTaskManualCalibrationStart, "vTask_ManualCalibration", 100, NULL, 0, &taskHandle1);

	//INSERIR BLOCOS
	xTaskCreate(vTaskRegisterBrick, "vTask_RegisterBrick", 100, NULL, 0, &taskHandle2);
	xTaskCreate(vTaskHandleConfirmedBrick, "vTask_HandleBrick", 100, NULL, 0, &taskHandle3);

	//DETEÇÃO DE BLOCOS
	xTaskCreate(vTaskBrickSensors, "vTask_BrickSensors", 100, NULL, 0, &taskHandle6);

	//LIMITAR CILINDROS ENTRE SENSORES
	xTaskCreate(vTaskCylinder0Limit, "vTask_Cylinder0Limit", 100, NULL, 0, &taskHandle7);
	xTaskCreate(vTaskCylinder1Limit, "vTask_Cylinder1Limit", 100, NULL, 0, &taskHandle8);
	xTaskCreate(vTaskCylinder2Limit, "vTask_Cylinder2Limit", 100, NULL, 0, &taskHandle9);

	//HISTORICO
	xTaskCreate(vTaskSaveBrickFile, "vTask_SaveBrickFile", 100, NULL, 0, &taskHandle10);
	xTaskCreate(vTaskShowHistoric, "vTask_ShowHistoric", 100, NULL, 0, &taskHandle19);
	xTaskCreate(vTaskHistoricMenu, "vTask_HistoricMenu", 100, NULL, 0, &taskHandle20);
	xTaskCreate(vTaskShowBlockHistoric, "vTask_ShowBlockHistoric", 100, NULL, 0, &taskHandle21);

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

	//EMERGENCIA
	xTaskCreate(vTaskEmergency, "vTask_Emergency", 100, NULL, 0, &emergencyTask);
	xTaskCreate(vTaskResumeEmergency, "vTask_ResumeEmergency", 100, NULL, 0, &resumeTask);
	xTaskCreate(vTaskRestartSystem, "vTask_RestartSystem", 100, NULL, 0, &restartSystem);
	xTaskCreate(vTaskFlashingLampEmergency, "vTask_FlashingLampEmergency", 100, NULL, 0, &blinkLight);
	//**********************************************************************************
}

//********************************
// vAssertCalled
// 
// Função necessária para a execução do programa
//********************************
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

//********************************
// initialiseHeap
// 
// Função necessária para a execução do programa
//********************************
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

//********************************
// main
// 
// Função necessária para a execução do programa
//********************************
int main(int argc, char** argv) {

	initialiseHeap();
	vApplicationDaemonTaskStartupHook = &myDaemonTaskStartupHook;
	vTaskStartScheduler();

	Sleep(5000);
	closeChannels();

}