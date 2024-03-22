
#ifndef Tool_h
#define Tool_h

#include <Arduino.h>
#include <functional>
#include <utility>


class Tools{
    public : 
        static int sign(int val) {
        return (0 < val) - (val < 0);
        }

        typedef bool (*Function)(void);
        static bool startingUp(Function action, uint32_t length = 10000, uint32_t wait = 10 ) {
            bool result = false;
            uint32_t t0 = millis();
            while (millis() - t0 < length) {
                result |= action();
                delay(wait);
            }
            return result;
        }

};

#endif /* Tool_h */