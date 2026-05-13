#include <vector>
#include <Arduino.h>
#include <stdint.h>

class async_parallel_bus_HAL
{
  protected:
    std::vector<int> address_pins;
    std::vector<int> data_pins;
    int WE_pin, OE_pin, CE_pin, RYBY_pin, RST_pin, WP_pin;

    uint32_t port_A_DQ_mask;
    uint32_t port_B_DQ_mask;
    uint32_t port_C_DQ_mask;
    uint32_t port_D_DQ_mask;

    void build_DQ_mask();
    void set_DQ_input();
    void set_DQ_output();
    void pushAddress(uint32_t address);
    void pushData(uint16_t data);

    void command_write_cycle(uint32_t address, uint16_t data);

  public:
    async_parallel_bus_HAL(
        const std::vector<int>& ADDR_pins, const std::vector<int>& DQ_pins,
        int WE, int OE, int CE, int RYBY, int RST, int WP
    );
    void begin();
    uint16_t word_read(uint32_t address);
};
