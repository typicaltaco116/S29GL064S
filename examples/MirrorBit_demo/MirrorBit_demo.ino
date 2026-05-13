////////////////////////////////////////////////////////////////////////////////
// Filename: MirrorBit_demo.ino
// Author: Jack Pyburn
// Description: This is an example file for demonstrating basic features of the
// S29GL064S class. 
////////////////////////////////////////////////////////////////////////////////
#include <S29GL064S.h>
#include <vector>
#include <stdint.h>

static const std::vector<int> address_DUE_pins = {
  22, 23, 24, 25, 26, 27, 28, 29,
  30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43
};

static const std::vector<int> data_DUE_pins = {
  54, 55, 56, 57, 58, 59, 60, 61,
  62, 63, 64, 65, 66, 67, 68, 69
};

static const int WE_pin = 48;
static const int OE_pin = 49;
static const int CE_pin = 50;
static const int RYBY_pin = 8; // This isn't used on the V6 version of the chip
static const int RST_pin = 51;
static const int WP_pin = 7;

static S29GL064S memory(address_DUE_pins, data_DUE_pins, 
                        WE_pin, OE_pin, CE_pin, RYBY_pin, RST_pin, WP_pin);

void setup() 
{
  char buffer[45];
  SerialUSB.begin(250000);

  memory.begin();

  while (!SerialUSB);
  while ((char)SerialUSB.read() != '\r'); // wait for user to press enter

  memory.chip_erase();
  memory.word_program(0x0064, 0xAABB);

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

void loop() {}
