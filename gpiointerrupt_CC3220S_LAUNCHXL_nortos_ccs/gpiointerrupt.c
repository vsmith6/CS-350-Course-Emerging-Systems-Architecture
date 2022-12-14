/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Added Timer Driver */
#include <ti/drivers/Timer.h>


int gpioButton = 0;
/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    /* Toggle an LED */
    //GPIO_toggle(CONFIG_GPIO_LED_0);
    gpioButton = 1;

}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_1.
 *  This may not be used for all boards.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn1(uint_least8_t index)
{
    /* Toggle an LED */
    //GPIO_toggle(CONFIG_GPIO_LED_1);
    gpioButton = 0;

}


volatile unsigned char TimerFlag;

void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    TimerFlag = 1; // Raise Flag
}

enum LED_states { LED_Init, LED_RedOn, LED_RedOff, LED_GreenOn, LED_GreenOff } LED_State;

// State machine functionality
void MorseCode_SOS() {   // Transitions
    static unsigned char cnt;
    switch(LED_State) {

        case LED_Init:
            LED_State = LED_RedOn;
            cnt = 0;
            break;
        case LED_RedOn:   // Red light on state
            if (cnt < 6) {
                LED_State = LED_RedOff;
            }
            else if ((cnt > 5) && (cnt < 9)) {
                LED_State = LED_RedOff;
            }
            else if (cnt == 23) {
                LED_State = LED_RedOn;
            }
            else if ((cnt > 22) && (cnt < 28)) {
                LED_State = LED_RedOff;
            }
            else if (cnt == 28) {
                LED_State = LED_RedOff;
            }

            break;
        case LED_RedOff:     // Red light off state
            if (cnt < 6) {
                LED_State = LED_RedOn;
            }
            else if ((cnt > 5) && (cnt < 9)) {
                LED_State = LED_RedOff;
            }
            else if ((cnt > 8) && (cnt < 12)) {
                LED_State = LED_GreenOn;
            }
            else if ((cnt > 22) && (cnt < 28)) {
                LED_State = LED_RedOn;
            }
            else if ((cnt > 27) && (cnt < 35)) {
                LED_State = LED_RedOff;

            }
            else if (cnt > 34) {
                LED_State = LED_Init;
            }
            break;
        case LED_GreenOn:   // Green light on state
            if ((cnt > 8) && ( cnt < 12)) {
                LED_State = LED_GreenOn;
            }
            else if ((cnt == 12) || (cnt == 16)) {
                LED_State = LED_GreenOff;
            }
            else if ((cnt > 12) && (cnt <= 16)) {
                LED_State = LED_GreenOn;
            }
            else if ((cnt > 16) && (cnt < 20)) {
                LED_State = LED_GreenOn;
            }
            else if ((cnt > 19) && (cnt < 23)) {
                LED_State = LED_GreenOff;
            }

            break;
        case LED_GreenOff:  // Green light off state
            if ((cnt == 12)) {
                LED_State = LED_GreenOff;
            }
            else if ((cnt > 12) && ( cnt < 16)) {
                LED_State = LED_GreenOn;
            }
            else if ((cnt == 16)) {
                LED_State = LED_GreenOff;
            }
            else if ((cnt > 16) && (cnt < 20)) {
                LED_State = LED_GreenOn;
            }
            else if ((cnt > 19) && (cnt < 23)) {
                LED_State = LED_GreenOff;
            }
            else if ((cnt == 23) ) {
                LED_State = LED_RedOn;
            }
            break;
        default:
            LED_State = LED_Init;


    }

        switch (LED_State) {  // State Actions
            case LED_RedOn:      // turn red light on
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                ++cnt;  // counter
                break;
            case LED_RedOff:     // turn red light off
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                ++cnt;
                break;
            case LED_GreenOn:     // turn green light on
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);
                ++cnt;
                break;
            case LED_GreenOff:     // turn green light off
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
                ++cnt;
                break;
            default:
                break;
        }

}

// State machine function for OK message
void MorseCode_OK() {
    static unsigned char num;
    switch(LED_State) {   // Transitions

        case LED_Init:   // Initial State
            LED_State = LED_GreenOn;
            num = 0;
            break;
        case LED_RedOn:   // Red light on state
            if ((num > 19) && (num < 21)) {
                LED_State = LED_RedOff;
            }

            break;
        case LED_RedOff:    // Red light off state
            if ((num > 20) && (num < 22)) {
                LED_State = LED_GreenOn;
            }
            break;

        case LED_GreenOn:   // Green light on state
            if ((num > 0) && ( num < 4)) {
                LED_State = LED_GreenOn;
            }
            else if ((num == 4)) {
                LED_State = LED_GreenOff;
            }
            else if ((num > 4) && (num < 8)) {
                LED_State = LED_GreenOn;
            }
            else if (num == 8 ) {
                LED_State = LED_GreenOff;
            }
            else if ((num > 8) && (num < 12)) {
                LED_State = LED_GreenOn;
            }
            else if ((num > 11) && (num < 15)) {
                LED_State = LED_GreenOff;
            }
            else if ((num > 14) && (num < 18)) {
                LED_State = LED_GreenOn;
            }
            else if ((num > 17) && (num < 19)) {
                LED_State = LED_GreenOff;
            }
            else if ((num > 20) && (num < 24)) {
                LED_State = LED_GreenOn;
            }
            else if ((num > 23) && (num < 30)) {
                LED_State = LED_GreenOff;
            }

            break;
        case LED_GreenOff:   // Greed light off state
            if ((num == 4)) {
                LED_State = LED_GreenOff;
            }
            else if ((num > 4) && ( num < 8)) {
                LED_State = LED_GreenOn;
            }

            else if ((num > 8) && (num < 12)) {
                LED_State = LED_GreenOn;
            }
            else if ((num > 11) && (num < 15)) {
                LED_State = LED_GreenOff;
            }
            else if ((num > 14) && (num < 18) ) {
                LED_State = LED_GreenOn;
            }
            else if ((num > 18) && (num < 20)) {
                LED_State = LED_RedOn;
            }
            else if ((num > 30)) {
                LED_State = LED_Init;

            }
            break;
        default:
            LED_State = LED_Init;
            break;
    }

        switch (LED_State) {  // State Actions
            case LED_RedOn:      // turn red light on
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                ++num;   // counter
                break;
            case LED_RedOff:     // turn red light off
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                ++num;
                break;
            case LED_GreenOn:     // turn green light on
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);
                ++num;
                break;
            case LED_GreenOff:     // turn green light off
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
                ++num;
                break;
            default:
                break;
        }

}



void initTimer(void)
{
    Timer_Handle timer0;
    Timer_Params params;

    Timer_init();
    Timer_Params_init(&params);
    params.period = 500000;  // set timer to 500000 us.
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);

    if (timer0 == NULL) {
        /* Failed to initialize timer */
        while (1) {}
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        /*Failed to start timer */
        while (1) {}
    }

}


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED and button pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);  // set pin's output to 0
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);  // interrupt on falling edge

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);

    /* Install Button callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);





    /*
     *  If more than one input pin is available for your device, interrupts
     *  will be enabled on CONFIG_GPIO_BUTTON1.
     */
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1) {
        /* Configure BUTTON1 pin */
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

        /* Install Button callback */
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);


        LED_State = LED_Init; // starting state of state machine
        initTimer();          // start timer
        while(1) {            // infinite loop

           if (gpioButton == 0) {  // if button 0 is pressed
               MorseCode_SOS();    // call function to blink SOS message
           }
           else if (gpioButton == 1) {  // if button 1 is pressed
               MorseCode_OK();          // call function to blink OK message
           }
           while(!TimerFlag){}    // wait for timer period
           TimerFlag = 0;         // lower flag for timer


        }


    }


    return (NULL);
}
