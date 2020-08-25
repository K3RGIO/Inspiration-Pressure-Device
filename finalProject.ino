// #include <Arduino.h>
#include <Adafruit_MCP4725.h>
#include <Wire.h>

Adafruit_MCP4725 dac;

//initialize digital pins
const byte led1 = 8;
const byte led2 = 9;
const byte pushButton = 11;

//initialize alanog pins
const byte pressureTransducer = A2;
const byte valve = 12;

//initialize required variables
int breathState;
int pressure;
int pressureVal;
float prev;
const long interval = 95;
unsigned long keyPrevMillis = 0;
const unsigned long keyIntervalMs = 25;
byte longKeyPressCountMax = 80; // 80*25=2000ms
byte longKeyPressCount = 0;
byte prevKeyState = HIGH;       // button is active low

//called when button is pressed for less than 2 seconds
void shortKeyPress() {
    Serial.println("short");
    digitalWrite(led1, HIGH);
    digitalWrite(valve, HIGH);                                          // new changes
    logic();
}

//called when button is held for more than 2 seconds
void longKeyPress(){
    Serial.println("long");
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    calibration();
}

//called when key goes from not pressed to pressed
void keyPress(){
    Serial.println("key press");
    longKeyPressCount = 0;
}

//called when key goes pressed to not pressed
void keyRelease() {
    Serial.println("key release");
  
    if(longKeyPressCount >= longKeyPressCountMax){
        longKeyPress();
    }
    else{
        shortKeyPress();
    }
}

void logic(){
    Serial.println("Logic Script");
    pressure = analogRead(pressureTransducer);
    while(pressure < 10){
        pressure = analogRead(pressureTransducer);
    }
    if(pressure >= 10) {
        timer();
    }
}

void timer(){
    Serial.println("Timer Script");
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    prev = millis();
    //digitalWrite(valve, HIGH);                                        //new changes

    while(millis() - prev <= interval){
        pressureVal = analogRead(pressureTransducer);
        dac.setVoltage(pressureVal, false)*4;
    }
    delay(5);
    digitalWrite(valve, LOW);
    digitalWrite(led2, LOW);
}

void calibration(){
    Serial.println("Calibration Script");
    while(1){
        pressureVal = analogRead(pressureTransducer)*4;
        dac.setVoltage(pressureVal, false);
            delay(100);
            Serial.println(pressureVal);
    }
}

void setup(){
    //begin serialisation
    Serial.begin(115200);

    dac.begin(0x62);

    pinMode(pushButton, INPUT_PULLUP);
    pinMode(pressureTransducer, INPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
}

void loop(){
    delay(10);

    //key management section
    if(millis() - keyPrevMillis >= keyIntervalMs){
        keyPrevMillis = millis();

    byte currKeyState = digitalRead(pushButton);

    if((prevKeyState == HIGH) && (currKeyState == LOW)){
        keyPress();
    }
    else if((prevKeyState == LOW) && (currKeyState == HIGH)){
        keyRelease();
    }
    else if(currKeyState == LOW){
        longKeyPressCount++;
    }

    prevKeyState = currKeyState;
  }
}