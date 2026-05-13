#include "async_parallel_bus_HAL.h"

#include <vector>
#include <stdint.h>
#include <Arduino.h>
#include "digitalWriteFast.h"

// Disable C++ Exception
namespace std {
    void __throw_bad_alloc() { while (1); }
}

void async_parallel_bus_HAL::pushAddress(uint32_t address)
{
    uint32_t bitmask = 1;
    for (int i = 0; i < address_pins.size(); ++i) {
        digitalWriteFast(address_pins[i], address & bitmask);
        bitmask = bitmask << 1;
    }
}

void async_parallel_bus_HAL::pushData(uint16_t data)
{
    uint16_t bitmask = 1;
    for (int i = 0; i < data_pins.size(); ++i) {
        digitalWriteFast(data_pins[i], data & bitmask);
        bitmask = bitmask << 1;
    }
}

void async_parallel_bus_HAL::build_DQ_mask()
{
    port_A_DQ_mask = 0;
    port_B_DQ_mask = 0;
    port_C_DQ_mask = 0;
    port_D_DQ_mask = 0;

    for (int i = 0; i < data_pins.size(); ++i) {
        int pin = data_pins[i];

        if (digitalPinToPort(pin) == PIOA) 
            port_A_DQ_mask |= digitalPinToBitMask(pin);
        else if (digitalPinToPort(pin) == PIOB) 
            port_B_DQ_mask |= digitalPinToBitMask(pin);
        else if (digitalPinToPort(pin) == PIOC) 
            port_C_DQ_mask |= digitalPinToBitMask(pin);
        else if (digitalPinToPort(pin) == PIOD) 
            port_D_DQ_mask |= digitalPinToBitMask(pin);
    }

}

void async_parallel_bus_HAL::set_DQ_input()
{
    for (int i = 0; i < data_pins.size(); ++i)
        pinMode(data_pins[i], INPUT);
    /*
    PIOA->PIO_ODR = port_A_DQ_mask;
    PIOB->PIO_ODR = port_B_DQ_mask;
    PIOC->PIO_ODR = port_C_DQ_mask;
    PIOD->PIO_ODR = port_D_DQ_mask;
    */
}

void async_parallel_bus_HAL::set_DQ_output()
{
    for (int i = 0; i < data_pins.size(); ++i)
        pinMode(data_pins[i], OUTPUT);
    /*
    PIOA->PIO_OER = port_A_DQ_mask;
    PIOB->PIO_OER = port_B_DQ_mask;
    PIOC->PIO_OER = port_C_DQ_mask;
    PIOD->PIO_OER = port_D_DQ_mask;
    */
}

async_parallel_bus_HAL::async_parallel_bus_HAL(
    const std::vector<int>& ADDR_pins, const std::vector<int>& DQ_pins,
    int WE, int OE, int CE, int RYBY, int RST, int WP)
{
    address_pins = ADDR_pins;
    data_pins = DQ_pins;
    WE_pin = WE;
    OE_pin = OE;
    CE_pin = CE;
    RYBY_pin = RYBY;
    RST_pin = RST;
    WP_pin = WP;
}

void async_parallel_bus_HAL::begin()
{
    build_DQ_mask();

    for (int i = 0; i < address_pins.size(); ++i)
        pinMode(address_pins[i], OUTPUT);

    for (int i = 0; i < data_pins.size(); ++i)
        pinMode(data_pins[i], INPUT);

    pinMode(RYBY_pin, INPUT_PULLUP);
    pinMode(WE_pin, OUTPUT);
    pinMode(OE_pin, OUTPUT);
    pinMode(CE_pin, OUTPUT);
    pinMode(WP_pin, OUTPUT);
    pinMode(RST_pin, OUTPUT);

    digitalWriteFast(CE_pin, HIGH); // place device into standby mode
    digitalWriteFast(OE_pin, HIGH);
    digitalWriteFast(WE_pin, HIGH);
    digitalWriteFast(WP_pin, HIGH);
    digitalWriteFast(RST_pin, HIGH);
}

uint16_t async_parallel_bus_HAL::word_read(uint32_t address)
{
    uint16_t bitmask;
    uint16_t dataBuffer = 0;

    set_DQ_input(); // set the pinModes to input

    digitalWriteFast(CE_pin, LOW); // when CE or OE is low the IO lines are no longer high-z
    digitalWriteFast(WE_pin, HIGH); // disable writing to the EEPROM

    pushAddress(address);

    digitalWriteFast(OE_pin, LOW);

    delayMicroseconds(50);

    bitmask = 0x0001;
    for (int i = 0; i < data_pins.size(); i++) {
        if (digitalRead(data_pins[i]))
            dataBuffer |= bitmask;

        bitmask = bitmask << 1;
    }

    digitalWriteFast(CE_pin, HIGH); // standby mode
    digitalWriteFast(OE_pin, HIGH);

    return dataBuffer;
}

// void function with device state consequences
void async_parallel_bus_HAL::command_write_cycle(uint32_t address, uint16_t data)
{
    pushAddress(address);
    delayMicroseconds(10);
    digitalWriteFast(WE_pin, LOW);
    pushData(data);
    delayMicroseconds(10);
    digitalWriteFast(WE_pin, HIGH);
}
