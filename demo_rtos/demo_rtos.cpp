// demo_rtos.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

extern "C" {
	#include <FreeRTOS.h>
	#include <task.h>
	#include <timers.h>
	#include <semphr.h>
	#include <interface.h>	
	#include <interrupts.h>
}


#define mainREGION_1_SIZE   8201
#define mainREGION_2_SIZE   29905
#define mainREGION_3_SIZE   7607

xSemaphoreHandle xSemaphore;


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


void vTaskCode_1(void* pvParameters)
{
	for (;; ) {
		printf("\nHello from TASK_1");
		// Although the kernel is in preemptive mode, 
		// we should help switch to another
		// task with e.g. vTaskDelay(0) or taskYELD()
		taskYIELD();
	}
}

void vTaskCode_2(void* pvParameters)
{
	for (;; )
	{
		printf("\nHello from TASK_2..");
		taskYIELD();
	}
}



void vTask_waits(void* pvParameters)
{
	while (1) {
		if (xSemaphoreTake(xSemaphore, 10000) == pdTRUE)
			printf("\nvTask_waits has been awaken\n");
		else
			printf("\nvTask_waits is tired of waiting\n");

	}
}

void vTask_signals(void* pvParameters)
{
	printf("write something\n");
		while (1) {
			char ch[101];
			gets_s(ch, 100);  // input from keyboard
			printf("\n vTask_signals got string '%s' from keyboard…", ch);
			if (strcmp(ch, "fim") == 0)
				exit(0); // terminate program...
			xSemaphoreGive(xSemaphore); //V
		}
}


void vTaskHorizontal(void* pvParameters)
{
	while (TRUE)
	{
		//go right
		uInt8 aa = readDigitalU8(2);
		writeDigitalU8(2, (aa & (0xff - 0x40)) | 0x80);

		// wait till last sensor
		while (readDigitalU8(0) & 0x01) {
			taskYIELD();
		}
		// go left		
		aa = readDigitalU8(2);
		vTaskDelay(10); // to simulate some latency  
		writeDigitalU8(2, (aa & (0xff - 0x80)) | 0x40);

		// wait till last sensor
		while ((readDigitalU8(0) & 0x04)) {
			taskYIELD();
		}
	}
}

void vTaskVertical(void* pvParameters)
{
	while (TRUE)
	{
		//go up
		uInt8 aa = readDigitalU8(2);
		writeDigitalU8(2, (aa & (0xff - 0x04)) | 0x08);

		// wait till last sensor		
		while ((readDigitalU8(0) & 0x40)) { vTaskDelay(1); }

		// go left		
		aa = readDigitalU8(2);
		vTaskDelay(10); // to simulate some latency
		writeDigitalU8(2, (aa & (0xff - 0x08)) | 0x04);

		// wait till last sensor		
		while ((readDigitalU8(1) & 0x08)) { vTaskDelay(1); }
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
	// GetTickCount64() current time in miliseconds 
	// since the system has started...
	ULONGLONG  time = GetTickCount64();
	printf("\nSwitch one RISING detected at time = %llu...", time);
}

void switch1_falling_isr(ULONGLONG lastTime) {	
	ULONGLONG  time = GetTickCount64();
	printf("\nSwitch one FALLING detected at  time = %llu...", time); 
}    

void switch2_change_isr(ULONGLONG lastTime) {	
	ULONGLONG  time = GetTickCount64();
	printf("\nSwitch two CHANGE detected at time = %llu...", time);
}


void myDaemonTaskStartupHook(void) {
	//xTaskCreate(vTaskCode_2, "vTaskCode_1", 100, NULL, 0, NULL);
	//xTaskCreate(vTaskCode_1, "vTaskCode_2", 100, NULL, 0, NULL);
	inicializarPortos();

	xSemaphore = xSemaphoreCreateCounting(10, 0);
	xTaskCreate(vTask_waits, "vTask_waits", 100, NULL, 0, NULL);
	xTaskCreate(vTask_signals, "vTaskCode2", 100, NULL, 0, NULL);
	xTaskCreate(vTaskVertical, "vTaskVertical", 100, NULL, 0, NULL);
	xTaskCreate(vTaskHorizontal, "vTaskHorizontal", 100, NULL, 0, NULL);	

	attachInterrupt(1, 5, switch1_rising_isr, RISING);
	attachInterrupt(1, 5, switch1_falling_isr, FALLING);
	attachInterrupt(1, 6, switch2_change_isr, CHANGE);

}

void myTickHook(void) {
	//printf("\ntick hook...");
	Sleep(0);
}


void myIdleHook(void) {
	// do something here
	//printf("\ntick hook...");   
	Sleep(0);
}


int main()
{
	initialiseHeap();
	vApplicationDaemonTaskStartupHook = &myDaemonTaskStartupHook;
	vApplicationTickHook              = &myTickHook;
	vApplicationIdleHook              = &myIdleHook;

	vTaskStartScheduler();
}

