#include "Copter.h"
#include "hal_pal_lld.h"
#include "hwdef.h"
#include <sys/_stdint.h>

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up

    // mux init
    const uint8_t I2C_SLAVE_DEVICE_ADDRESS = 0x8A;
    auto i2c_bus = hal.i2c_mgr->get_device(3, 0x70);

    uint8_t default_address = 50;
    uint8_t temp = 0x00;
    uint8_t found = 0;
    i2c_bus->get_semaphore()->take_blocking();
    i2c_bus->set_retries(10);
    // test by disabling all i2c ports on mux
    if (i2c_bus->transfer(&temp, 1, nullptr, 0)) {
        // assign addresses
        for (uint8_t i = 0; i <= 7; i++) {
            uint8_t local_address = 1 << i;
            // release and take semaphores on address change
            i2c_bus->transfer(&local_address, 1, nullptr, 0);
            hal.scheduler->delay(50);
            i2c_bus->get_semaphore()->give();
            i2c_bus->set_address(0x29);
            i2c_bus->get_semaphore()->take_blocking();
            if (i2c_bus->write_register(I2C_SLAVE_DEVICE_ADDRESS, default_address)) {
                hal.console->printf("Device %d address is now %d", i, default_address);
                mux_address_used_flag |= 1 << (default_address - 50);
                default_address++;
                found++;
            }
            i2c_bus->get_semaphore()->give();
            i2c_bus->set_address(0x70);
            i2c_bus->get_semaphore()->take_blocking();
        }
        temp = 0xFF;
        i2c_bus->transfer(&temp, 1, nullptr, 0); // connect all channels
        hal.scheduler->delay(50);
        hal.console->printf("Found %d devices on mux", found);
        i2c_bus->get_semaphore()->give();
        i2c_bus->set_address(0x29);
        i2c_bus->get_semaphore()->take_blocking();
        if (i2c_bus->read_registers(I2C_SLAVE_DEVICE_ADDRESS, &temp, 1)) {
            hal.console->printf("\nProblem assigning addresses!\n");
        }
        i2c_bus->get_semaphore()->give();
    }
    else {
        hal.console->println("No devices found!");
    }
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
    // put your 10Hz code here
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    const uint8_t I2C_SLAVE_DEVICE_ADDRESS = 0x8A;
    // put your 1Hz code here
    //hal.console->printf("\nFound %d devices on mux\n", Mux_found);
    auto i2c_bus = hal.i2c_mgr->get_device(3, 0x29);
    uint8_t temp;
    i2c_bus->get_semaphore()->take_blocking();
    // "hot replug" capability -- although not advisable when more than one device is being replugged
    if (i2c_bus->read(&temp, 1)) {
        uint8_t free_addresses = ~mux_address_used_flag;
        for (uint8_t i = 0; i < 8; i++) {
            if ((1 << i) & free_addresses) {
                i2c_bus->write_register(I2C_SLAVE_DEVICE_ADDRESS, 50 + i);
            }
        }
    }
    i2c_bus->get_semaphore()->give();
}
#endif

#ifdef USERHOOK_AUXSWITCH
void Copter::userhook_auxSwitch1(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #1 handler here (CHx_OPT = 47)
}

void Copter::userhook_auxSwitch2(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #2 handler here (CHx_OPT = 48)
}

void Copter::userhook_auxSwitch3(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #3 handler here (CHx_OPT = 49)
}
#endif
