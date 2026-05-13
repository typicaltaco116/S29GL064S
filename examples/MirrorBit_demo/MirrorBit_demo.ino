#include <S29GL064S.h>
#include <vector>
#include <stdint.h>

#define KWORD 1024

#define ANSI_COLOR_DEFAULT "\x1B[0m"
#define ANSI_COLOR_GREEN "\x1B[32m"

// Pin setup for IC
S29GL064S memory(
  {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43},
  {54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69},
  48, 49, 50, 8, 51, 7
); 

void setup() 
{
  char buffer[45];
  SerialUSB.begin(250000);

  memory.begin();

  while (!SerialUSB);
  while ((char)SerialUSB.read() != '\r');

  for (uint32_t i = 0; i < 4194304; ++i) {
    uint32_t temp = i;
    sprintf(buffer, "%06X: ", i);
    SerialUSB.print(buffer);

    while(i < temp + 15) {
      sprintf(buffer, "%04X ", memory.word_read(i));
      SerialUSB.print(buffer);
      i++;
    }

    SerialUSB.print("\r\n");
  }

}

void loop() {
  char buffer[45];

  while ((char)Serial.read() != '\r');

  for (int i = 0; i < KWORD; ++i) {
    sprintf(buffer, "%04X ", memory.word_read(0x0000 + i));
    Serial.print(buffer);
    if (!(i % 16))
      Serial.println();
  }
}
