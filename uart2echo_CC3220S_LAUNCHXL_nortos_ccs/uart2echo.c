/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uart2echo.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    char         input;
    const char   echoPrompt[] = "Echoing characters:\r\n";
    unsigned char state = NULL;    // added new unsigned char state and initialized it to NULL
    UART2_Handle uart;
    UART2_Params uartParams;
    size_t       bytesRead;
    size_t       bytesWritten = 0;
    uint32_t     status = UART2_STATUS_SUCCESS;
    /* Call driver init functions */
    GPIO_init();
    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Create a UART where the default read and write mode is BLOCKING */
    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200;

    uart = UART2_open(CONFIG_UART2_0, &uartParams);

    if (uart == NULL) {
        /* UART2_open() failed */
        while (1);
    }

    /* Turned OFF user LED to indicate successful initialization
     * when turned on again */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);

    UART2_write(uart, echoPrompt, sizeof(echoPrompt), &bytesWritten);

    enum LED_States
    {
        LED0_Start, LED0_N_OR_F, LED0_F_2, LED0_ON, LED0_OFF
    } LED_State;    // declare states as enum

    /* Loop forever echoing */
    while (1)
    {
        status = UART2_read(uart, &input, 1, &bytesWritten);
        if (status != UART2_STATUS_SUCCESS)
        {
            /*UAR2 read failing*/
            while (1);
        }

        switch (LED_State)
        {                 // State machine
        case LED0_Start:
            if (input == 'O')
            {
                LED_State = LED0_N_OR_F;  // if 'o' found, move to next case
            }
            break;
        case LED0_N_OR_F:
            if (input == 'N')
            {
                LED_State = LED0_ON; // 'n' found, turn light on
            }
            else if (input == 'F')
            {
                LED_State = LED0_F_2; // 'f' found, find another f
            }
            break;
        case LED0_F_2:      // second f
            if (input == 'F')
            {
                LED_State = LED0_OFF; // deactivate LED
            }
            break;
        default:
            LED_State = LED0_Start; // Default to start
            break;
        }

        switch (LED_State)
        {                 // actions
        case LED0_on:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);  // LED0 ON
            LED_State = LED0_Start;                 // start
            break;

        case LED0_off:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF); // LED0 OFF
            LED_State = LED0_Start;                 // start
            break;
        }
        status = UART2_write(uart, &input, 1, &bytesWritten);

        if (status != UART2_STATUS_SUCCESS)
        {
            /* UART2_write() failed */
            while (1);
        }

    }
}

