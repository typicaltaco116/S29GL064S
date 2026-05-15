#include "async_parallel_bus_HAL.h"

class S29GL064S : public async_parallel_bus_HAL {
    using async_parallel_bus_HAL::async_parallel_bus_HAL; // inherit constructor

protected:

public:
    struct SR_bitfields {
        bool DRB;
        bool ESSB;
        bool ESB;
        bool PSB;
        bool WBASB;
        bool PSSB;
        bool SLSB;
        bool CC;
    };

    void word_program(uint32_t address, uint16_t data);
    void sector_erase(uint32_t address);
    void evaluate_erase(uint32_t address);
    void chip_erase(void);

    SR_bitfields status_reg_read(void);

    void set_write_protect(void);
};
