
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
        static String getString(String data, char separator, int index){
            int found = 0;
            int strIndex[] = {0, -1};
            int maxIndex = data.length()-1;

            for(int i=0; i<=maxIndex && found<=index; i++){
                if(data.charAt(i)==separator || i==maxIndex){
                    found++;
                    strIndex[0] = strIndex[1]+1;
                    strIndex[1] = (i == maxIndex) ? i+1 : i;
                }
            }

            return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
        }
        static int countChar(String chaine, char c) {
            int compteur = 0;
            
            for (int i = 0; i < chaine.length(); i++) {
                if (chaine[i] == c) {
                compteur++;
                }
            }
            
            return compteur;
        }
};

#endif /* Tool_h */