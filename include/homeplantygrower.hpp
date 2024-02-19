#define homeplantygrower
#ifndef homeplantygrower

namespace homeplantygrower
{
#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
#include <util/delay.h>

    volatile int light;

    class grower
    {
    public:
        grower();
        void readLight();
    };

} // namespace homeplantygrower
#endif
