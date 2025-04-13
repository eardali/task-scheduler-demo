/*
 * tasks.c
 *
 *  Created on: Mar 2025
 *      Author: eardali
 */

#include "tasks.h"
#include <stdio.h>

void initTasks(void){
	OS_TaskCreate(&taskLed, period_1s, BLOCKED, NULL, period_1s*5);
	OS_TaskCreate(&taskInfo, period_1s*10, BLOCKED, NULL, period_1s*2); //an early check
}

uint32_t taskInfo(void *data){
    static bool second_call = false;
    uint32_t i;
    //check some parameters of tasks
    i = OS_GetOsTime();
    printf("OS time: %ld\n", i);

    i = OS_GetTaskState(&taskLed);
    printf("Task state: %ld\n", i);

    i = OS_GetTaskPeriod(&taskLed);
    printf("Task period: %ld msec\n", i);

    i = OS_GetTaskExecuteTime(&taskLed);
    printf("Task Ptr: %p, next execution time: %ld msec\n", &taskLed, i);

    //check other task
    if(OS_TaskIsInQueue(&taskLedStop)){ //check if task in queue
        i = OS_GetTaskExecuteTime(&taskLedStop);
        printf("Task Ptr: %p, next execution time: %ld msec\n", &taskLedStop, i);
        printf("Remaning time for the task (%p): %ld msec\n", &taskLedStop, (i-OS_GetOsTime()));
    }else{
    	printf("Task Ptr: %p (which creates Task Ptr: %p) is not executed yet\n", &taskLed, &taskLedStop);
    }

    if(!second_call){
        printf("Second call scheduled to check task status later\n");
        second_call = true;
        return period_1s*6;
    }else{
    	return period_end; //no periodic execution any more, task is dropped from array
    }
}

uint32_t taskLed(void *data){
    static uint8_t i = 0;
    if(i == 0)
        OS_TaskCreate(&taskLedStop, period_1s, BLOCKED, NULL, period_1s*55); //another task started which will stop this task after defer time
    
    BSP_LED_Toggle(LED3);
    i++;

    //in a certain condition, task period can be changed via return value
    if(i<10)
        return period_1s;
    else
        return period_1s*5;
}

uint32_t taskLedStop(void *data){
    OS_SetTaskState(&taskLed, STOPPED);
    //below is also valid
    //OS_TaskCreate(&taskLed, period_1s, STOPPED, NULL, 0);
    BSP_LED_Off(LED3);
    return period_end; //run once then stop
}
