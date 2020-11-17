#include "commands.hpp"

String dataAsStr = "01 00 00 20  01 00 01 01  03 00 00 01  04 00 00 03";

//replace with the c hex array output from https://hlorenzi.github.io/customasm/web/
const unsigned char data[] = {
	0x01, 0x00, 0x00, 0x20, 
    0x01, 0x00, 0x01, 0x01, 
    0x03, 0x00, 0x00, 0x01, 
    0x04, 0x00, 0x00, 0x03,
    0x06, 0x01, 0x01, 0x01, 0x00, 0x0C, 0x00, 0x00,
    0x06, 0x00, 0x01, 0x00, 0x01, 0x00, 0x0C, 0x00
};

const int maxProgramBytes = sizeof(data)/sizeof(data[0]);

void parseCommand(byte command[16]){
    funcmap[command[0]].function(command[1], command[2], command[3], command[4], command[5], command[6], command[7], command[8], command[9], command[10], command[11], command[12], command[13], command[14], command[15]);
}

bool parseConditionalJump(int i){
    nibblePair banks = splitByte(data[i+1]);
    byte bank1 = banks.high;
    byte bank2 = banks.low;

    switch (data[3+i]) { //Conditional jump (== as 00, != as 01, >= as 02, <= as 03, > as 04, < as 05), 8 bytes
        case 0:
            if (progRam[bank1][data[2+i]] == progRam[bank2][data[4+i]]) return true;
            else return false;
            break;

        case 1:
            if (progRam[bank1][data[2+i]] != progRam[bank2][data[4+i]]) return true;
            else return false;
            break;

        case 2:
            if (progRam[bank1][data[2+i]] >= progRam[bank2][data[4+i]]) return true;
            else return false;
            break;

        case 3:
            if (progRam[bank1][data[2+i]] <= progRam[bank2][data[4+i]]) return true;
            else return false;
            break;

        case 4:
            if (progRam[bank1][data[2+i]] > progRam[bank2][data[4+i]]) return true;
            else return false;
            break;

        case 5:
            if (progRam[bank1][data[2+i]] < progRam[bank2][data[4+i]]) return true;
            else return false;
            break;

        default:
            return false;
            break;
    }
}

byte* scanCommand(int* index, byte *command){
    int i = *index;

    switch (funcmap[data[i]].params){
        case 255: { //An always jump
            Serial.print("Selected Jump to ");
            *index += 4;
            Serial.print((data[1+i] << 8) | data[2+i], HEX);
            Serial.print(" (");
            Serial.print((data[1+i] << 8) | data[2+i], DEC);
            Serial.print(")  ");
            
            *index = (data[1+i] << 8) | data[2+i];
            break;
        }

        case 254: { //Conditional jump (== as 00, != as 01, >= as 02, <= as 03, > as 04, < as 05), 8 bytes
            Serial.print("Selected Conditional Jump to ");
            *index += 8;

            Serial.print((data[5+i] << 8) | data[6+i], HEX);
            Serial.print(" (");
            Serial.print((data[5+i] << 8) | data[6+i], DEC);
            Serial.print(")  ");
            
            bool canGo = parseConditionalJump(i);
            
            if (canGo){
                *index = (data[6+i] << 8) | data[6+i];
                Serial.print("Check Succeeded, jumping  ");
            } else {
                Serial.print("Check Failed, proceeding  ");
            }

            break;
        }

        case 4: {
            Serial.print("Selected 4  ");
            *index += 4;
            command[0] = data[i];
            command[1] = data[i+1];
            command[2] = data[i+2];
            command[3] = data[i+3];
            break;
        }

        case 8: {
            *index += 8;
            Serial.print("Selected 8  ");
            command[0] = data[i];
            command[1] = data[i+1];
            command[2] = data[i+2];
            command[3] = data[i+3];
            command[4] = data[i+4];
            command[5] = data[i+5];
            command[6] = data[i+6];
            command[7] = data[i+7];
            break;
        }

        case 16: {
            *index += 16;
            Serial.print("Selected 16  ");
            command[0] = data[i];
            command[1] = data[i+1];
            command[2] = data[i+2];
            command[3] = data[i+3];
            command[4] = data[i+4];
            command[5] = data[i+5];
            command[6] = data[i+6];
            command[7] = data[i+7];
            command[8] = data[i+8];
            command[9] = data[i+9];
            command[10] = data[i+10];
            command[11] = data[i+11];
            command[12] = data[i+12];
            command[13] = data[i+13];
            command[14] = data[i+14];
            command[15] = data[i+15];
            break;
        }

        default: {
            Serial.print("Selected 4  ");
            *index += 4;
            command[0] = data[i];
            command[1] = data[i+1];
            command[2] = data[i+2];
            command[3] = data[i+3];
            break;
        }
    }
}

void setup() {
    Serial.begin(9600);
    delay(1000);
    Serial.println("-------------------------------------");
    delay(1000);

    for (int i = 0; i < maxProgramBytes; )
    {
        byte command[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        scanCommand(&i, command);
        
        for(int x=0; x < 16; x++){
            Serial.print(command[x], HEX);
            Serial.print(" ");
            delay(10);
        }
        Serial.println(" command " + String((i/4)));
        if (command[0] < 200){
            parseCommand(command);
        }
        /*Serial.print(command[0], HEX);
        Serial.print(" ");
        Serial.print(command[1], HEX);
        Serial.print(" ");
        Serial.print(command[2], HEX);
        Serial.print(" ");
        Serial.println(command[3], HEX);*/
    }
    digitalWrite(13, LOW);
}

void loop(){}