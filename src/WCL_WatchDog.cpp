#include "WCL_WatchDog.h"

void WCL_WatchDog::initialize(uint32_t timeInMilliseconds) {
    *(uint32_t *) WDT_RREN_REG = 0x00000001;
    *(uint32_t *) WDT_CRV_REG = (uint32_t) (timeInMilliseconds * 32.768);
    *(uint32_t *) WDT_REG = 0x00000001;
}

void WCL_WatchDog::pet() {
    *(uint32_t *) WDT_RR0_REG = WDT_RELOAD;
}

bool WCL_WatchDog::isEnabled() {
    return (*(uint32_t *) WDT_RUNSTATUS_REG == 1);
}

void WCL_WatchDog::runWhileSleeping(bool yes) {
    if (yes) {
        *(uint32_t *) WDT_CONFIG_REG |= 0x01;
    } else {
        *(uint32_t *) WDT_CONFIG_REG &= ~(0x01);
    }
}

void WCL_WatchDog::runWhileAtBreakpoint(bool yes) {
    if (yes) {
        *(uint32_t *) WDT_CONFIG_REG |= 0x08;
    } else {
        *(uint32_t *) WDT_CONFIG_REG &= ~(0x08);
    }
}