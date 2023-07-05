//Ran on a pro micro, but have to upload as a leonardo eth
//nRF24L01+ is used as the radio transmitter, 3.3V breakout adapter helps with noisey 3/5V input
//Arduino powered via a 9V battery w/switch
//Motor driver powered by on board battery nimh?
//Buck boost used to on servo line
 //---( Import needed libraries )---//
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//RF24 definitions
#define SCK_PIN 15
#define MISO_PIN 14
#define MOSI_PIN 16
#define CS_PIN 18
RF24 radio(19, 18); // Create a Radio(CE, CSN)

// TB9051FTGMotorCarrier pin definitions(should all be PWM)
const  uint8_t enPin = 10;
const  uint8_t pwm1Pin = 6;
const  uint8_t pwm2Pin = 5;

//FOR SERVO (Steering)
Servo myservo;
#define servoPin 3 

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe[1] = { 0xE8E8F0F0E1LL }; // Define the transmit pipe

//---( Declare Variables )---/
int data[9];
int gotArray[9];
int servoAngle = 95;
float SpeedAdjust = 1; 
bool skipCode = false; 
uint16_t counter = 1;
uint16_t prevCounter = 0;

//enum Buttons {
//    Adjust,            // 0
//    LEFT_ANALOG_X = 0, // 1
//    RIGHT_ANALOG_X,    // 2
//    LEFT_ANALOG_Y,     // 3
//    RIGHT_ANALOG_Y,    // 4
//    R2,                // 5
//    L2,                // 6
//    SpeedAdjust,       // 7
//    counter            // 8 
// need 2 more inputs to allow brakemodes?              
//};


void setup()   /**** SETUP: RUNS ONCE ****/
{
//Extra 5V output for Radio Tx
    pinMode(20, OUTPUT);
    digitalWrite(20, HIGH);
    
//Radio Tx Setup  
    Serial.begin(115200);
    delay(100);
    Serial.println("Nrf24L01 Receiver Starting");
    radio.begin();
    radio.setAutoAck(true);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.setDataRate(RF24_2MBPS);
    radio.openReadingPipe(1, pipe[0]);
    radio.setRetries(15, 15);
    radio.startListening();
    radio.setPALevel(RF24_PA_MAX); //MAX can also be HIGH and LOW?

//Driver Setup 
  pinMode(enPin, OUTPUT);
  pinMode(pwm1Pin, OUTPUT);
  pinMode(pwm2Pin, OUTPUT);

//Servo Setup
    myservo.attach(servoPin);
    
}
//--(end setup )-

void loop()   /**** LOOP: RUNS CONSTANTLY ****/
{
    if (radio.available()) {
        // Fetch the data payload        
        radio.read(&gotArray, sizeof(gotArray));
    }
    Serial.println("Received array = ");
    for (byte i = 0; i < 9; i++) {
        Serial.println(gotArray[i]);
        data[i] = gotArray[i];
                                    }
//data[8] = counter;
// if (counter == prevCounter) {
//      skipCode = true;
//    }
//    else {
//      skipCode = false;
//    }
//    counter = prevCounter;
//  
//if (!skipCode) {
//Steering Loop
int AnalogStick = map(data[1] , 1, 255, 132 , 54);
    if (data[1] = 0) {
      myservo.write(95 + data[0]);}
    else if ( data[1] < 125) {
      myservo.write(AnalogStick + data[0]); }
    else if ( data[1] > 140) {
      myservo.write(AnalogStick + data[0]); }
//    else {
//      myservo.write(95 + data[0]);
//    }
//130-131 is max left
//52-53 is max right  

//---( Section for DC Motor SpeedAdjust )---//    
    if (data[7] == 4){
      (SpeedAdjust = 1);  
    }
    if (data[7] == 3){
      (SpeedAdjust = 0.75);  
    }
    if (data[7] == 2){
      (SpeedAdjust = 0.5);  
    }
    if (data[7] == 1){
      (SpeedAdjust = 0.35);  
    }
//---( Section for DC Motor Driving )---//  
int Forwards = map(data[5] , 0, 255, 0, 255);
int Reverse = map(data[6] , 0, 255, 0, 255);

int FinalSpeedForwards = (Forwards * SpeedAdjust);
//Boundary Check
 if (FinalSpeedForwards < 0)  {
      FinalSpeedForwards = 0;
    }
 if (Reverse < 0 )  {
      Reverse = 0;
    }   
//Drive Forwards/Backwards
    if (Forwards > 5) {
      digitalWrite(enPin, HIGH);    // Enable
      analogWrite(pwm1Pin, 0 );  // Forward
      analogWrite(pwm2Pin, FinalSpeedForwards);
    }
    else if (Reverse > 5) {
      digitalWrite(enPin, HIGH); //Enable
      analogWrite(pwm1Pin, Reverse);      // Reverse
      analogWrite(pwm2Pin, 0);
    }
    else if (  Forwards < 25  && Reverse < 25 ) {
      digitalWrite(enPin, LOW);     // Enable
      analogWrite(pwm1Pin, 0);      // Coast
      analogWrite(pwm2Pin, 0);
    }
    else if (Forwards > 255 || Reverse > 255) {
      digitalWrite(enPin, HIGH);     // Enable
      analogWrite(pwm1Pin, 0);      // Stop
      analogWrite(pwm2Pin, 0);
    }
}
//} 
