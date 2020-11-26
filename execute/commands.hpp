#include "Servo.h"
#include "util.hpp"
#include <arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>


#define USE_CGA
#define USE_SWEETIE_16
#define USE_NATURES_EMBRACE
#include "palettes.hpp"
uint16_t drawcolor = 0x0000; // the color the tft will draw with
uint16_t tempPalette[16]; // a temp variable

void setTempPal(byte palette) {
    switch (palette) {
        case 0:
            memcpy(&tempPalette, &cga, sizeof(cga));
            break;

        case 1:
            memcpy(&tempPalette, &sweetie, sizeof(cga));
            break;
    }
}

// init the tft,    CS, CD, WR, RD, RST 
Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);

// Definitions of stuffs
byte progRam[256][1]; // the program memory, 10, 256 byte banks
Servo servo; // the default servo

// Stuff needed to init the program
void initSerial(){
    Serial.begin(9600);
}
void initTFT(){
    tft.begin(tft.readID()); //start the tft
    tft.setRotation(3); //set proper rotation
}
void initServos(int pwmDataPin){
    // attach and test servo, moves it to 180 then zeroes it
    servo.attach(pwmDataPin);
    servo.write(180);
    delay(500);
    servo.write(0);
}

void initExecutionEngine(){
    initTFT(); //init the tft
    initSerial(); // init serial port
    // initServos(2); // removed because we dont want to reserve the pin yet
}

// used to return 2 nibbles since multiple returns isnt supported
class nibblePair {
    public:
        byte high;
        byte low;
};

// class that carries a function and a byte
// the function is the function to call
// the byte is how many parameters should be parsed, will always be 4, 8, or 16
// the exceptions are custom behavior in execute.ino used for jumps
class commandPair {
    public:
        using fntype = void(*)(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
        byte params;
        void (*function)(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);

        commandPair(byte params, fntype function) : params(params), function(function) {}
};

// splits a byte into nibbles
// returns the high
nibblePair splitByte(byte inByte){
    nibblePair out;
    out.high = inByte >> 4;
    out.low = inByte & 0x0F;

    return out;
}

//sets byte
void set(byte bank, byte _byte, byte value, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    progRam[bank][_byte] = value; 
}

//adds 2 bytes and replaces the first
void add(byte _banks, byte _byte1, byte _byte2, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    nibblePair banks;
    banks = splitByte(_banks);
    byte bank1 = banks.high;
    byte bank2 = banks.low;

    progRam[bank1][_byte1] = progRam[bank1][_byte1] + progRam[bank2][_byte2];
}

//copies a byte to another
void copy(byte _banks, byte _byte1, byte _byte2, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    nibblePair banks;
    banks = splitByte(_banks);
    byte bank1 = banks.high;
    byte bank2 = banks.low;

    progRam[bank2][_byte2] = progRam[bank1][_byte1];
}

//subtracts one byte from another and replaces the first
void sub(byte _banks, byte _byte1, byte _byte2, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    nibblePair banks;
    banks = splitByte(_banks);
    byte bank1 = banks.high;
    byte bank2 = banks.low;

    progRam[bank1][_byte1] = progRam[bank1][_byte1] + progRam[bank2][_byte2];
}

//prints to serial with specified mode
void printSerial(byte bank, byte _byte, byte mode, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    switch (mode)
    {
        case 0:
            Serial.print(progRam[bank][_byte], HEX);
            Serial.print(" ");
            break;
        
        case 1:
            Serial.print((int)progRam[bank][_byte]);
            break;
        
        case 2:
            Serial.print((char)progRam[bank][_byte]);
            break;
        
        case 3:
            Serial.println(progRam[bank][_byte], HEX);
            break;
        
        case 4:
            Serial.println((int)progRam[bank][_byte]);
            break;
        
        case 5:
            Serial.println((char)progRam[bank][_byte]);
            break;
        
        case 6:
            Serial.print("\n");
            break;
        
        default:
            break;
    }
    delay(5); //delay so it doesnt just break lol
}

//waits specified ms
void wait(byte highWait, byte midWait, byte lowWait, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    delay((((long)highWait) << 16) + (((long)midWait) << 8) + ((long)lowWait));
}

//attatches the servo on pin 2
void servoInit(byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    initServos(2); //set up pin 2 for servo control
}

//detatches servo
void servoKill(byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    servo.detach();
}

//writes degrees or radians to servo
void servoWrite(byte bank, byte _byte, byte isRads, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    servo.write(clamp(isRads ? radToDeg(progRam[bank][_byte]) : progRam[bank][_byte], 0, 180));
}

//reads the servo
void servoRead(byte bank, byte _byte, byte asRads, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    progRam[bank][_byte] = asRads ? degToRad(servo.read()) : servo.read();
}

//does nothing, used when a function has behavior defined elsewhere, namely jumps
void doNothing(byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){}

//sets tft's color from color set specified
void setTFTColorFromPalette(byte colorIndex, byte palette, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    setTempPal(palette);
    drawcolor = tempPalette[colorIndex];
}

//sets tft's color to specified custom color
void setTFTColorCustom(byte colorHigh, byte colorLow, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    drawcolor = (uint16_t)(0 | colorHigh << 8 | colorLow);
}

void fillTFT(byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    tft.fillScreen(drawcolor);
}

void rect(byte x1high, byte x1low, byte y1high, byte y1low, byte widthHigh, byte widthlow, byte heightHigh, byte heightlow, byte filled, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    switch(filled){
        case 0:
            tft.drawRect(x1high<<8|x1low, y1high<<8|y1low, widthHigh<<8|widthlow, heightHigh<<8|heightlow, drawcolor);
            break;
        case 1:
            tft.fillRect(x1high<<8|x1low, y1high<<8|y1low, widthHigh<<8|widthlow, heightHigh<<8|heightlow, drawcolor);
            break;
    }
}

void testPalette(byte palette, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    setTempPal(palette);
    
    tft.setTextSize(2);
    tft.setTextWrap(false);
    int z = 0;
    for(int x = 0; x < 4; x++){
        for(int y = 0; y < 4; y++){
            tft.fillRect(x*320/4, y*240/4, 320/4, 240/4, tempPalette[z]);
            tft.setTextColor(~tempPalette[z]);

            tft.setCursor(5+x*320/4, 5+y*240/4);
            String hexCode = String(tempPalette[z], HEX);
            for(; hexCode.length() < 4; hexCode = "0"+hexCode){} //just some jank to pas with zeroes
                
            tft.print("#" + hexCode);

            z++;
        }
    }
}

void tri(byte x1high, byte x1low, byte y1high, byte y1low, byte x2high, byte x2low, byte y2high, byte y2low, byte x3high, byte x3low, byte y3high, byte y3low, byte filled, byte _13, byte _14){
    switch(filled){
        case 0:
            tft.drawTriangle(x1high<<8|x1low, y1high<<8|y1low, x2high<<8|x2low, y2high<<8|y2low, x3high<<8|x3low, y3high<<8|y3low, drawcolor);
            break;
        case 1:
            tft.fillTriangle(x1high<<8|x1low, y1high<<8|y1low, x2high<<8|x2low, y2high<<8|y2low, x3high<<8|x3low, y3high<<8|y3low, drawcolor);
            break;
    }
}

void circ(byte x1high, byte x1low, byte y1high, byte y1low, byte radius, byte filled, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){
    switch(filled){
        case 0:
            tft.drawCircle(x1high<<8|x1low, y1high<<8|y1low, radius, drawcolor);
            break;
        case 1:
            tft.fillCircle(x1high<<8|x1low, y1high<<8|y1low, radius, drawcolor);
            break;
    }
}

void text(byte strBank, byte strOffset, byte consumeBytes, byte x1high, byte x1low, byte y1high, byte y1low, byte textSize, byte wrap, byte newline, byte moveCursor, byte _11, byte _12, byte _13, byte _14){
    tft.setTextWrap(wrap);
    tft.setTextSize(textSize);
    tft.setTextColor(drawcolor);

    switch (moveCursor){
        case 0:
            break;

        case 1:
            tft.setCursor(x1high<<8|x1low, y1high<<8|y1low);
    }

    switch (newline){
        case 0:
            for (int i = 0; i < consumeBytes; i++){
                tft.print(
                    (char)progRam[strBank][strOffset + i]
                );
            }
            break;
        
        case 1:
            for (int i = 0; i < consumeBytes; i++){
                tft.print(
                    (char)progRam[strBank][strOffset + i]
                );
            }
            tft.println("");
            break;
    }
}


//map bytes to functions, makes bytecode parsing simple
commandPair funcmap[] = {
    commandPair(4, *doNothing), //No op
    commandPair(4, *set), //Set byte
    commandPair(4, *copy), //Subtract 2 bytes and replace the first
    commandPair(4, *add), //Sdd 2 bytes and replace the first
    commandPair(4, *printSerial), //Print byte over serial
    commandPair(255, *doNothing), //Always jump. requires logic in execute.ino
    commandPair(254, *doNothing), //Conditional jump. requires logic in execute.ino
    commandPair(4, *wait), //A delay
    commandPair(4, *servoInit), //Inits the servo
    commandPair(4, *servoKill), //Kills the connection by disabling pwm
    commandPair(4, *servoWrite), //Writes to a servo
    commandPair(4, *servoRead), //Reads from servo
    commandPair(4, *setTFTColorFromPalette), //sets tft's color from cga color set specified
    commandPair(4, *setTFTColorCustom), //sets tft's color to specified custom color, must be rgb565, hex colors can be converted here: https://trolsoft.ru/en/articles/rgb565-color-picker
    commandPair(4, *fillTFT), //fills tft screen with color set using color set previously
    commandPair(16, *rect), //draws a rectangle
    commandPair(16, *tri), //draws a triangle
    commandPair(8, *circ), //draws a circle
    commandPair(16, *text), //draws text from memory. if the bytes to consume overflows, you may end up with undefined behavior due to it displaying raw ram
    commandPair(4, *testPalette) //tests specified palette
};