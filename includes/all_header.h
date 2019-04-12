/*
 * all_header.h
 *
 *  Created on: Apr 3, 2019
 *      Author: Akshay Godase
 *      This header files contains all the header files required. So that, if someone includes this header file
 *      he doesn't need to bother about all of the other files
 */

#ifndef INCLUDES_ALL_HEADER_H_
#define INCLUDES_ALL_HEADER_H_

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"
#include "uart.h"
#include "adc.h"
#include "Timer.h"
#include "SPI.h"
#include "my_can.h"
#include "ISL94212.h"
#include "General.h"
#include "device_implementation.h"
#include "Queue.h"
#include "CommandPriorityQueue.h"
#include "fault_handling.h"

//common definitions to all

#define DEBUG
#define PARTIAL_LOG

#define MAX_DEVICES         4
#define UV_LIMIT            0.555
#define UV_LIMIT_RECOV      1.20
#define OV_LIMIT            3.5
#define OV_LIMIT_RECOV      3.2
#define OT_LIMIT            degC_35
#define OT_LIMIT_RECOV      degC_30
#define MAX_CELL_NUMBER     12

#endif /* INCLUDES_ALL_HEADER_H_ */
