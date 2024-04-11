#ifndef _SERIAL_TASK_H
#define _SERIAL_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/uart.h"

#include "common/datatypes/mutex.h"
#include "serial/serial_com.h"

static const int RX_BUF_SIZE = 128;

#define UART UART_NUM_0

#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_3)

typedef struct
{
    AerManager *am;
    SerialCom *sc;
} SerialTaskParams;

// init uart instance for receiving messages
void initUART(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // We won't use a buffer for sending data.
    uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void serial_task(void *pvParameters)
{
    SerialTaskParams *params = (SerialTaskParams *)pvParameters;
    AerManager *aerManager = params->am;
    SerialCom *serialComm = params->sc;

    initUART();
    serialComm->begin(aerManager);

    Serial.print("> Starting Serial Handler task on core ");
    Serial.println(xPortGetCoreID());
    while (1)
    {
        if (xSemaphoreTake(sys1_mutex, 40) == pdTRUE)
        {
            uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
            // read the bytes on the wire, timeout after 10ms.
            const int rxBytes = uart_read_bytes(UART, data, RX_BUF_SIZE, 10 / portTICK_RATE_MS);
            if (rxBytes > 0)
            {
                data[rxBytes] = 0;
                char *decoded = (char *)malloc(RX_BUF_SIZE * 2);
                for (int i = 0; i < RX_BUF_SIZE; i++)
                {
                    decoded[i] = 0xff;
                }
                // attempt to decode data read from wire
                if (decodeData((char *)data, rxBytes, decoded))
                {
                    // handle decoded data buffer
                    serialComm->handleEventBuffer(decoded);
                }
                free(decoded);
            }
            free(data);
            data = (uint8_t *)malloc(0);
            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif