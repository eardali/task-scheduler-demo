/*
 * tasks.h
 *
 *  Created on: Mar 2025
 *      Author: eardali
 */

#ifndef SRC_TASKS_H_
#define SRC_TASKS_H_

#include "main.h"
#include "OS.h"

void initTasks(void);
uint32_t taskLed(void *data);
uint32_t taskLedStop(void *data);
uint32_t taskInfo(void *data);

#endif /* SRC_TASKS_H_ */
