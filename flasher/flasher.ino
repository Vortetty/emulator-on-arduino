#include <EEPROM.h>

const int maxProgramBytes = 768;

char programBytes[maxProgramBytes]{
    0x01,
    0x00,
    0x00,
    0x20,
    0x01,
    0x00,
    0x01,
    0x01,
    0x03,
    0x00,
    0x00,
    0x01,
    0x04,
    0x00,
    0x00,
    0x01
};
    /*
    "program here, max of 768 characters, "
    "if you chose to write in hexadecimal you must convert to ascii before pasting here, remove line breaks before converting"
    "you can easily do this at: https://www.rapidtables.com/convert/number/hex-to-ascii.html"
    "if you used my assembler then it automatically generates the ascii needed for parsing and flashing"
};*/

void dash(){
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(100);
}

void dot(){
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
}

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    int x = 0;
    for(char y : programBytes){
        if(x < maxProgramBytes){
            EEPROM.write(x, y);
        }
        x++;
    }

    //flash "done!" in morse code so you know it is done
    //code from https://morsecode.world/international/morse2.html
    //exclamation is not recommended code but i use it anyway
    dash();
    dot();
    dot();
    delay(100);
    dash();
    dash();
    dash();
    delay(100);
    dash();
    dot();
    delay(100);
    dot();
    delay(100);
    dash();
    dot();
    dash();
    dot();
    dash();
    dash();
}

void loop(){}