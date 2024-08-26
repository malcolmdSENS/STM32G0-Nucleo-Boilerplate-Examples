#ifndef INC_CUSTOMSERIAL_H_
#define INC_CUSTOMSERIAL_H_

#include <main.h>
#include <stdbool.h>


#define BUF_SIZE 13
#define FIFO_SIZE 100

typedef struct {
  uint8_t msg[BUF_SIZE];
}BS_IssMsg;


void customSerial_Init(UART_HandleTypeDef* huart);
void sendMessage(void);
void readMessage(void);
bool processMessage(void);


#endif  // INC_CUSTOMSERIAL_H_
