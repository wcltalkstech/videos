#include "application.h"

#pragma once

#define WDT_REG 0x40010000
#define WDT_EVENTS_TIMEOUT_REG 0x40010100
#define WDT_INTENSET_REG 0x40010304
#define WDT_INTENCLR_REG 0x40010308
#define WDT_RUNSTATUS_REG 0x40010400
#define WDT_REQSTATUS_REG 0x40010404
#define WDT_CRV_REG 0x40010504
#define WDT_RREN_REG 0x40010508
#define WDT_CONFIG_REG 0x4001050C
#define WDT_RR0_REG 0x40010600

#define WDT_RELOAD 0x6E524635

class WCL_WatchDog {
    public:
        void initialize(uint32_t timeInMilliseconds);
        void pet();

        bool isEnabled();

        void runWhileSleeping(bool yes);
        void runWhileAtBreakpoint(bool yes);

};