#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Init buttons before callback */
int leftButton = 0;                     // Left button for decrement
int rightButton = 0;                    // Right button for increment

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    /* Button function */
    leftButton = 1;
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
    /* Button function */
    rightButton = 1;
}

/*
 *  ======== UART code begin ========
 */
#define DISPLAY(x) UART_write(uart, &output, x);

// UART Global Variables
char output[64];
int bytesToSend;

// Driver Handles - Global variables
UART_Handle uart;
void initUART(void)
{
    UART_Params uartParams;
    // Init the driver
    UART_init();
    // Configure the driver
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 115200;
    // Open the driver
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL)
    {
        /* UART_open() failed */
        while (1);
    }
}

/*
 *  ======== I2C code begin ========
 */
// I2C Global Variables
static const struct
{
    uint8_t address;
    uint8_t resultReg;
    char *id;
}

sensors[3] =
{
    { 0x48, 0x0000, "11X" },
    { 0x49, 0x0000, "116" },
    { 0x41, 0x0001, "006" }
};
uint8_t txBuffer[1];
uint8_t rxBuffer[2];
I2C_Transaction i2cTransaction;

// Driver Handles - Global variables
I2C_Handle i2c;

// Make sure you call initUART() before calling this function.
void initI2C(void)
{
    int8_t i, found;
    I2C_Params i2cParams;
    DISPLAY(snprintf(output, 64, "Initializing I2C Driver - "));

    // Init the driver
    I2C_init();

    // Configure the driver
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    // Open the driver
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL)
    {
        DISPLAY(snprintf(output, 64, "Failed\n\r"));
        while (1);
    }

    DISPLAY(snprintf(output, 32, "Passed\n\r"));

    // Boards were shipped with different sensors.
    // Welcome to the world of embedded systems.
    // Try to determine which sensor we have.
    // Scan through the possible sensor addresses

    /* Common I2C transaction setup */
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;
    found = false;
    for (i = 0; i < 3; ++i)
    {
        i2cTransaction.slaveAddress = sensors[i].address;
        txBuffer[0] = sensors[i].resultReg;
        DISPLAY(snprintf(output, 64, "Is this %s? ", sensors[i].id));
        if (I2C_transfer(i2c, &i2cTransaction))
        {
            DISPLAY(snprintf(output, 64, "Found\n\r"));
            found = true;
            break;
        }
        DISPLAY(snprintf(output, 64, "No\n\r"));
    };

    if (found)
    {
        DISPLAY(snprintf(output, 64, "Detected TMP%s I2C address: %x\n\r", sensors[i].id, i2cTransaction.slaveAddress));
    }

    else
    {
        DISPLAY(snprintf(output, 64, "Temperature sensor not found,"
                                     "contact professor\n\r"));
    }
}

int16_t readTemp(void)
{
    int16_t temperature = 0;
    i2cTransaction.readCount = 2;
    if (I2C_transfer(i2c, &i2cTransaction))
    {
        /*
         * Extract degrees C from the received data;
         * see TMP sensor datasheet
         */
        temperature = (rxBuffer[0] << 8) | (rxBuffer[1]);
        temperature *= 0.0078125;

        /*
         * If the MSB is set '1', then we have a 2's complement
         * negative value which needs to be sign extended
         */
        if (rxBuffer[0] & 0x80)
        {
            temperature |= 0xF000;
        }
    }

    else
    {
        DISPLAY(snprintf(output, 64, "Error reading temperature sensor (%d)\n\r"
                                     ,i2cTransaction.status));
        DISPLAY(snprintf(output, 64, "Please power cycle your board by unplugging "
                                     "USB and plugging back in.\n\r"));
    }

    return temperature;
}

/*
 *  ======== Timer code begin ========
 */
// Driver Handles - Global variables
Timer_Handle timer0;
volatile unsigned char TimerFlag = 0;
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    TimerFlag = 1;
}
void initTimer(void)
{
    Timer_Params params;
    // Init the driver
    Timer_init();
    // Configure the driver
    Timer_Params_init(&params);
    params.period = 1000000;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.periodUnits = Timer_PERIOD_US;
    params.timerCallback = timerCallback;

    // Open the driver
    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL)
    {
        /* Failed to initialized timer */
        while (1){}
    }
    if (Timer_start(timer0) == Timer_STATUS_ERROR)
    {
        /* Failed to start timer */
        while (1){}
    }
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();

    /* Configure the red LED */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Configure BUTTON0 pin */
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

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

        /* Enable interrupts */
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }



    /* Call driver init functions */
    initUART();
    initI2C();
    initTimer();

    /* Init variables*/
    unsigned long buttonCheck = 0;                              // time for button
    unsigned long tempCheck = 0;                                // time for temp
    unsigned long displayCheck = 0;                             // time for display
    const unsigned long buttonPeriod = 200;                    // button period
    const unsigned long tempPeriod = 500;                      // temp period
    const unsigned long displayPeriod = 1000;                  // display period
    const unsigned long timerPeriod = 100;                     // timer period
    int setpoint = 25;                                          // setpoint
    int heat = 1;                                               // heat
    int seconds = 0;                                            // seconds
    int temperature = 0;                                         // temperature

    /*
     *  ======== Main while loop ========
     */
    while (1)
    {
        readTemp();                                                 // Read
        /* check button presses */
        if (buttonCheck >= buttonPeriod)                   // Button check
        {
            if (rightButton == 1)                                   // Button on right side raises setpoint by 1
            {
                setpoint += 1;
                rightButton = 0;
            }
            if (leftButton == 1)                                    // Button on left side lowers setpoint by 1
            {
                setpoint -= 1;
                leftButton = 0;
            }
        }


        if (tempCheck >= tempPeriod)                       // if temp check time equals or exceeds period
        {
            temperature = readTemp();                               // set temp to equal readTemp
            if (temperature < setpoint)                             // if temp lower than thermostat, LED turns on
            {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                heat = 1;
            }
            else                                                    // If temp is higher than thermostat, the LED turns off
            {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                heat = 0;
            }
        }

        /* Every 1000ms output to UART to display temperature */
        if (displayCheck >= displayPeriod)
        {
            DISPLAY(snprintf(output, 64, "<%02d,%02d,%d,%04d>\n\r", // Display to terminal (temperature, setpoint, heat, seconds)
                            temperature, setpoint, heat, seconds));
            ++seconds;
        }
        while (!TimerFlag){}
        TimerFlag = 0;                                              // Lower flag raised by timer
        buttonCheck += timerPeriod;
        tempCheck += timerPeriod;
        displayCheck += timerPeriod;
    }
}
