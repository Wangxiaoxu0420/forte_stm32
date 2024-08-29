#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "forteinit.h"
#include "forte_Init.h"

static const char *const TAG = "MAIN";

extern "C" {
void forte_main(void);
void wifi_init_sta(void);
}



static uint8_t ucParameterToPass;
TaskHandle_t xHandle = NULL;

void MyForteTask(void *pvParameters)
{

    printf("Executing forte...");

    forteGlobalInitialize();
    TForteInstance forteInstance = 0;
    int resultForte = forteStartInstanceGeneric(0, 0, &forteInstance);
    if (FORTE_OK == resultForte)
    {
        printf("OK %d , Could start forte\n", resultForte);
        forteJoinInstance(forteInstance);
    }
    else
    {
         printf("Error %d: Couldn't start forte\n", resultForte);
    }
    forteGlobalDeinitialize();
    vTaskDelete(NULL);
}

void forte_main(void)
{

    xTaskCreate( MyForteTask, "MY_FORTE", 10000, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle );
    configASSERT(xHandle);
}

void vApplicationMallocFailedHook____()
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("vApplicationMallocFailedHook");
    }
}

void vApplicationStackOverflowHook____(TaskHandle_t xTask, char *pcTaskName)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("vApplicationStackOverflowHook");
    }
}




static const char *const FTAG = "forte";


//TODO !!!
// if Forte is to DEBUG and ESP-IDF to Info,
// DEBUG Information gets lost.

/*! \brief print the given log message with the error level and a time stamp
 *
 * @param pa_ecLevel the message's log level
 * @param pa_acMessage the message to log
 */
enum E_MsgLevel {
  E_INFO,
  E_WARNING,
  E_ERROR,
  E_DEBUG,
  E_TRACE
};
void printLogMessage(char paLevel, char * const paMessage)
{
	size_t strlen0 = strlen(paMessage);
	size_t strlen1 = strlen0 - 1;

	if (paMessage[strlen1] == '\n') {
		paMessage[strlen1]  = '\0';
	}


    printf("%s", paMessage);
	switch (paLevel)
	{
	case E_INFO:
	    printf("%s", paMessage);
		break;
	case E_WARNING:
	    printf("%s", paMessage);
		break;
	case E_ERROR:
	    printf("%s", paMessage);
		break;
	case E_DEBUG:
	    printf("%s", paMessage); //TODO find a better way.
		break;
	case E_TRACE:
	    printf("%s", paMessage); //TODO find a better way.
		break;
	}





}

