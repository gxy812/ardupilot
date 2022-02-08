#include "Copter.h"
#include "hal_pal_lld.h"
#include "hwdef.h"

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up

    // turn on GPIO pin for use as 3v3
    hal.gpio->pinMode(54, HAL_GPIO_OUTPUT);
    hal.gpio->write(54, TRUE);

    // add time for devices to turn on
    hal.scheduler->delay(100);

    // mux init
    const uint8_t I2C_SLAVE_DEVICE_ADDRESS = 0x8A;
    auto i2c_bus = hal.i2c_mgr->get_device(3, 0x70);

    uint8_t default_address = 50;
    uint8_t temp = 0xFF;
    uint8_t final_address = 0;
    uint8_t found = 0;
    i2c_bus->get_semaphore()->take_blocking();
    i2c_bus->set_retries(10);
    // test by enabling all i2c ports on mux
    // allows debugging using lua scripts (i2c_scan) if anything fails
    if (i2c_bus->transfer(&temp, 1, nullptr, 0)) {
        // assign addresses
        for (uint8_t i = 0; i < 8; i++) {
            uint8_t local_address = 1 << i;
            i2c_bus->set_address(0x70);
            i2c_bus->transfer(&local_address, 1, nullptr, 0);
            hal.scheduler->delay(50);
            i2c_bus->set_address(0x29);
            if (i2c_bus->write_register(I2C_SLAVE_DEVICE_ADDRESS, default_address)) {
                hal.console->printf("Device %d address is now %d", i, default_address);
                final_address |= local_address;
                default_address++;
                found++;
            }
        }
        temp = 0xFF;
        i2c_bus->transfer(&final_address, 1, nullptr, 0); // connect all channels
        hal.scheduler->delay(50);
        hal.console->printf("Found %d devices on mux", found);
        i2c_bus->set_address(0x29);
        if (i2c_bus->read_registers(I2C_SLAVE_DEVICE_ADDRESS, &temp, 1)) {
            hal.console->printf("\nProblem assigning addresses!\n");
        }
    }
    else {
        hal.console->println("No devices found!");
    }
    i2c_bus->set_address(0x70);
    i2c_bus->get_semaphore()->give();
    hal.scheduler->delay(500);
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
    // put your 1Hz code here
    //hal.console->printf("\nFound %d devices on mux\n", Mux_found);
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
