/*
 GBCartRead
 Version: 1.5u
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Edit: Frode vdM. (fvdm1992@yahoo.no)
 Created: 18/03/2011
 Last Modified: 08/11/2014
 
 Read ROM, Read RAM or Write RAM from/to a Gameboy Cartridge.

 Build from N64 Transfer Pack header, wich includes a nifty cartridge-sense switch.
 Connect switch to +5V on one side and to digital pin 13 by a pulldown on the other.
 Also connect CLK on the GB header to GROUND for support with the GB Camera.

 You MUST use a transistor to switch the power going to the shift registers and GB header
 (except for +5V going to the sense switch). Output A2 switches the power ON when LOW,
 and OFF when HIGH. Here is a diagram that also includes a LED indicator:

                     ,--------------------- +5V
               ,,--./
              /  |<� \
 A2 ---[10K]-----|   | PNP Transistor
              \  |-. /
               ``--�\
                     `-+------------------- Power going to 74LS595's and GB Header
                       |
                       |           ^^
                       +--[ ? ]--->|---. (optional LED)
                       |               |
                       `-----[10K]-----+--- GND

 You should also put 10K resistors in series on pin A3, A4, A5, 10, 11 and 12 to prevent
 too much current from leaking through. This and the 10K pulldown in parallel with the
 optional LED is sufficient for getting the line to 0V when the transistor is blocking.

 */


int latchPin = 10;
int dataPin = 11;
int clockPin = 12;
int detectPin = 13;
int rdPin = A5;
int wrPin = A3;
int mreqPin = A4;
int powerPin = A2;

void setup() {
  Serial.begin(57600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(rdPin, OUTPUT);
  pinMode(wrPin, OUTPUT);
  pinMode(mreqPin, OUTPUT);
  pinMode(detectPin, INPUT);
  pinMode(powerPin, OUTPUT);
  inputs();
  powerOff();
}

void loop() {
  // Wait for serial input
  while (Serial.available() <= 0) {
    delay(200);
  }

  // Decode input
  char readInput[10];
  int readCount = 0;
  while (Serial.available() > 0) {
    char c = Serial.read();
    readInput[readCount] = c;
    readCount++;
  }
  readInput[readCount] = '\0';

  if (strstr(readInput, "POWERON")){
    powerOn();
    Serial.println("END");
  }

  else if (strstr(readInput, "POWEROFF")) {
    powerOff();
    Serial.println("END");
  }

  // Cartridge Header
  else if (strstr(readInput, "HEADER")) {
       Serial.println("EPROM");
       Serial.println(0);
       Serial.println(1);
       Serial.println(0);
       Serial.println(digitalRead(powerPin) == LOW);
  }

  // Dump ROM
  else if (strstr(readInput, "READROM")) {
    Serial.write(0x5A);
    Serial.write(0x55);

      for (unsigned int addr = 0; addr <= 0x7FFF; addr = addr+64) {
        Serial.write(0xA5);
        Serial.write(0xA5);
        byte dat[64];
        for(int i=0; i<64; i++){
          dat[i] = readByte(addr+i);
        }
        Serial.write(dat, 64);
    }

          for (unsigned int addr = 0; addr <= 0x7FFF; addr = addr+64) {
        Serial.write(0xA5);
        Serial.write(0xA5);
        byte dat[64];
        for(int i=0; i<64; i++){
          dat[i] = readByte(0x8000 + addr+i);
        }
        Serial.write(dat, 64);
    }
    Serial.write(0x5A);
    Serial.write(0xAA);
  }

  // Read RAM
  else if (strstr(readInput, "READRAM")) {
    Serial.write(0x5A);
    Serial.write(0x55);

    Serial.write(0x5A);
    Serial.write(0xAA);
  }
  
  // Write RAM
  else if (strstr(readInput, "WRITERAM")) {
    Serial.println("START");
    Serial.println("END");
  }
}



int readByte(int address) {
  shiftoutAddress(address); // Shift out address
  delayMicroseconds(1);
  digitalWrite(mreqPin, LOW);
  //delayMicroseconds(1);
  digitalWrite(rdPin, LOW);
  delayMicroseconds(1);
  byte bval = ((PINB<<6)|(PIND>>2))&0xFF; // Read data
  digitalWrite(rdPin, HIGH);
  digitalWrite(mreqPin, HIGH);
  return bval;
}

// Use the shift registers to shift out the address
void shiftoutAddress(unsigned int shiftAddress) {

  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 12)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 11)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 10)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 9)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 8)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 7)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 6)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 5)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 4)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 3)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 2)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress >> 1)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress << 1)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress << 2)&B00001000;
  PORTB |= B00010000;
  PORTB &= B11100111;
  PORTB |= (shiftAddress << 3)&B00001000;
  PORTB |= B00010000;

  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);
}



// Turn the data lines
void setData(int data) {
  data &= 0xFF;
  PORTD &= B00000011;
  PORTB &= B11111100;
  PORTD |= (data << 2);
  PORTB |= (data >> 6);
}



void inputs() {
  DDRB &= B11111100;
  DDRD &= B00000011;
}

void powerOn() {
    digitalWrite(latchPin, HIGH);
    digitalWrite(rdPin, HIGH); // RD off
    digitalWrite(wrPin, HIGH); // WR off
    digitalWrite(mreqPin, HIGH); // MREQ off
    delay(100);
    digitalWrite(powerPin, LOW);
}

void powerOff() {
  digitalWrite(powerPin, HIGH);
  delay(100);
  digitalWrite(latchPin, LOW);
  digitalWrite(dataPin, LOW);
  digitalWrite(rdPin, LOW); // RD off
  digitalWrite(wrPin, LOW); // WR off
  digitalWrite(mreqPin, LOW); // MREQ off
}
