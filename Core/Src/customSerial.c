#include <customSerial.h>
#include <string.h>

UART_HandleTypeDef* uartHandle;

BS_IssMsg processBuffer[FIFO_SIZE];
uint8_t rxIndex = FIFO_SIZE;
uint8_t wrIndex = FIFO_SIZE;
bool readReady = false;

uint8_t rxBuffer[BUF_SIZE];
uint8_t msgBuffer[BUF_SIZE];
uint8_t bufIndex = 0;

// FIFO Stuff....
bool BufferNotEmpty() {
  bool notEmpty = wrIndex != rxIndex;
  return notEmpty;
}
bool BufferNotFull() {
  size_t nextIdx = wrIndex + 1;
  if (nextIdx >= FIFO_SIZE) {
    nextIdx = 0;
  }
  return (nextIdx != rxIndex);
}

bool bufferFull = false;
bool bufferEmpty = false;

void customSerial_Init(UART_HandleTypeDef* huart) {
  uartHandle = huart;
  // Wait for first message
  readMessage();
}

void readMessage(void) {
  HAL_UART_Receive_IT(uartHandle, rxBuffer, 1);
}

void sendMessage(void) {
  HAL_UART_Transmit_IT(uartHandle, msgBuffer, BUF_SIZE);
}

bool processMessage(void) {
   bool success = false;
   if(readReady) {
     bufferEmpty = false;
     while(BufferNotEmpty()) {
       // Get message from buffer and advance readIdx.
       if (++rxIndex >= FIFO_SIZE) {
         rxIndex = 0;
       }
       ENTER_CRITIAL_SECTION();
       BS_IssMsg* curMsg = &processBuffer[rxIndex];
       if(strncmp((char*)curMsg, "ABCDEFGHIJKLM", BUF_SIZE) == 0) {
         memmove(msgBuffer, curMsg, BUF_SIZE);
         success = true;
       }
       EXIT_CRITIAL_SECTION();
     }
     bufferEmpty = false;
  }

   return success;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  // Upon entry, bufIndex should hold the index of the element that
  // was just updated. We then set up the conditions to fill the next
  // element in the buffer.

  // If packet buffer has not been filled, we wait for the next byte.
  // Otherwise we move to the next packet buffer in the FIFO and start
  // filling it with bytes.
  if(bufIndex < (BUF_SIZE-1)) {
    ++bufIndex;
    readReady = false;
    HAL_UART_Receive_IT(uartHandle, &rxBuffer[bufIndex], 1);
  } else {
    bufIndex = 0;
    readReady = true;
    if(BufferNotFull()) {
      bufferFull = false;
      // Write to the next index after checking above.
      if (++wrIndex >= FIFO_SIZE) {
        wrIndex = 0;
      }
      memmove(&processBuffer[wrIndex], rxBuffer, BUF_SIZE);
      readMessage();
    } else {
      bufferFull = true;
    }
  }
}
