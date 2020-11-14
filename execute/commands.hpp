// Definitions of stuffs
byte progRam[256][10]; //the program memory, 10, 256 byte banks

// Stuff needed to init the program
void initSerial(){
    Serial.begin(9600);
}

class nibblePair {
    public:
        byte high;
        byte low;
};

class commandPair {
    public:
        using fntype = void(*)(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
        byte params;
        void (*function)(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);

        commandPair(byte params, fntype function) : params(params), function(function) {}
};

nibblePair splitByte(byte inByte){
    nibblePair out;
    out.high = inByte >> 4;
    out.low = inByte & 0x0F;

    return out;
}

void set(byte bank, byte _byte, byte value, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    progRam[bank][_byte] = value; 
}

void copy(byte _banks, byte _byte1, byte _byte2, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    nibblePair banks;
    banks = splitByte(_banks);
    byte bank1 = banks.high;
    byte bank2 = banks.low;

    progRam[bank2][_byte2] = progRam[bank1][_byte1];
}

void add(byte _banks, byte _byte1, byte _byte2, byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11){
    nibblePair banks;
    banks = splitByte(_banks);
    byte bank1 = banks.high;
    byte bank2 = banks.low;

    progRam[bank1][_byte1] = progRam[bank1][_byte1] + progRam[bank2][_byte2];
}

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
    delay(50); //delay so it doesnt just break lol
}

void doNothing(byte _0, byte _1, byte _2, byte _3, byte _4, byte _5, byte _6, byte _7, byte _8, byte _9, byte _10, byte _11, byte _12, byte _13, byte _14){}

commandPair funcmap[] = {
    commandPair(4, *doNothing),
    commandPair(4, *set),
    commandPair(4, *copy),
    commandPair(4, *add),
    commandPair(4, *printSerial)
};