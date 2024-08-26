#include <customSerial.h>

UART_HandleTypeDef* uartHandle;

void customSerial_Init(UART_HandleTypeDef* huart) {
  uartHandle = huart;
}
