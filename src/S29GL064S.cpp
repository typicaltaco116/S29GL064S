#include "S29GL064S.h"
#include "digitalWriteFast.h"
#include <Arduino.h>

void S29GL064S::set_write_protect(void)
{
    digitalWriteFast(WP_pin, LOW);
}

bool S29GL064S::word_program(uint32_t address, uint16_t data)
{
    bool fail_flag;

    digitalWriteFast(OE_pin, HIGH);
    digitalWriteFast(CE_pin, LOW);
    set_DQ_output();

    command_write_cycle(0x555, 0xAA);
    command_write_cycle(0x2AA, 0x55);
    command_write_cycle(0x555, 0xA0);
    command_write_cycle(address, data);

    set_DQ_input();

    digitalWriteFast(OE_pin, LOW);
    while ((bool)digitalRead(data_pins[7]) != (bool)(data & (1 << 7)));
    fail_flag = digitalRead(data_pins[5]);

    digitalWriteFast(CE_pin, HIGH);
    digitalWriteFast(OE_pin, HIGH);

    return fail_flag;
}

bool S29GL064S::sector_erase(uint32_t address)
{
    bool fail_flag;

    digitalWriteFast(CE_pin, LOW);
    digitalWriteFast(OE_pin, HIGH);
    set_DQ_output();

    command_write_cycle(0x555, 0xAA);
    command_write_cycle(0x2AA, 0x55);
    command_write_cycle(0x555, 0x80);
    command_write_cycle(0x555, 0xAA);
    command_write_cycle(0x2AA, 0x55);
    command_write_cycle(address, 0x30);

    set_DQ_input();

    unsigned long first_time = millis();
    while (millis() - first_time < 800) {
        digitalWriteFast(OE_pin, LOW);
        delayMicroseconds(10);
        digitalWriteFast(OE_pin, HIGH);
        delayMicroseconds(10);
    }

    //while (!digitalRead(data_pins[7]));
    fail_flag = digitalRead(data_pins[5]);

    digitalWriteFast(CE_pin, HIGH);
    digitalWriteFast(OE_pin, HIGH);

    return fail_flag;
}

bool S29GL064S::chip_erase(void)
{
    digitalWriteFast(CE_pin, LOW);
    digitalWriteFast(OE_pin, HIGH);
    set_DQ_output();

    command_write_cycle(0x0555, 0xAA);
    command_write_cycle(0x02AA, 0x55);
    command_write_cycle(0x0555, 0x80);
    command_write_cycle(0x0555, 0xAA);
    command_write_cycle(0x02AA, 0x55);
    command_write_cycle(0x0555, 0x10);

    set_DQ_input();
    digitalWriteFast(OE_pin, LOW);

    bool last_DQ_2_value;
    bool current_DQ_2_value = digitalRead(data_pins[2]);

    do {
        last_DQ_2_value = current_DQ_2_value;
        digitalWriteFast(OE_pin, HIGH);
        digitalWriteFast(OE_pin, LOW);
        current_DQ_2_value = digitalRead(data_pins[2]);
    } while (last_DQ_2_value != current_DQ_2_value);

    digitalWriteFast(CE_pin, HIGH);
    digitalWriteFast(OE_pin, HIGH);

    return false;
}

void S29GL064S::evaluate_erase(uint32_t address)
{
    digitalWriteFast(CE_pin, LOW);
    digitalWriteFast(OE_pin, HIGH);
    set_DQ_output();

    command_write_cycle(address, 0x35);

    set_DQ_input();

    delay(10);

    digitalWriteFast(CE_pin, HIGH);
}

S29GL064S::SR_bitfields S29GL064S::status_reg_read(void)
{
    uint16_t bitmask;
    uint16_t dataBuffer = 0;

    // Send SR read command
    digitalWriteFast(CE_pin, LOW);
    set_DQ_output();
    command_write_cycle(0x555, 0x70);

    pushAddress(0x0000);

    set_DQ_input();
    digitalWriteFast(OE_pin, LOW);

    delayMicroseconds(50);
    
    bitmask = 0x0001;
    for (int i = 0; i < data_pins.size(); i++) {
        if (digitalRead(data_pins[i]))
            dataBuffer |= bitmask;

        bitmask = bitmask << 1;
    }

    digitalWriteFast(CE_pin, HIGH);
    digitalWriteFast(OE_pin, HIGH);

    // Fill struct
    status_reg.DRB      = dataBuffer & (1 << 7);
    status_reg.ESSB     = dataBuffer & (1 << 6);
    status_reg.ESB      = dataBuffer & (1 << 5);
    status_reg.PSB      = dataBuffer & (1 << 4);
    status_reg.WBASB    = dataBuffer & (1 << 3);
    status_reg.PSSB     = dataBuffer & (1 << 2);
    status_reg.SLSB     = dataBuffer & (1 << 1);
    status_reg.CC       = dataBuffer & 0x1;

    // Send ASO exit command
    digitalWriteFast(CE_pin, LOW);
    set_DQ_output();
    command_write_cycle(0x000, 0xF0);
    digitalWriteFast(CE_pin, HIGH);
    set_DQ_input();

    return status_reg;
}
