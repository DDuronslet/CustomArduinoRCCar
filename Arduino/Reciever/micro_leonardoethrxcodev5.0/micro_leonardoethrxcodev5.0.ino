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
#include <TB9051FTGMotorCarrier.h>

//RF24 definitions
#define SCK_PIN 15
#define MISO_PIN 14
#define MOSI_PIN 16
#define CS_PIN 18

// TB9051FTGMotorCarrier pin definitions
static constexpr uint8_t pwm1Pin{9};
static constexpr uint8_t pwm2Pin{6};
static constexpr uint8_t enPin{5};

// Instantiate TB9051FTGMotorCarrier
static TB9051FTGMotorCarrier driver{pwm1Pin, pwm2Pin, 255, 255, 255, enPin, 255 };
//need to short OCC, and both EN

//FOR SERVO (Steering)
Servo myservo;
#define servoPin 3 

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe[1] = { 0xE8E8F0F0E1LL }; // Define the transmit pipe

//---( Declare objects )---/
RF24 radio(19, 18); // Create a Radio(CE, CSN)

//---( Declare Variables )---/
int data[8];
int gotArray[8];
int servoAngle;
float SpeedAdjust = 1; 

//enum Buttons {
//    Adjust,            // 0
//    LEFT_ANALOG_X = 0, // 1
//    RIGHT_ANALOG_X,    // 2
//    LEFT_ANALOG_Y,     // 3
//    RIGHT_ANALOG_Y,    // 4
//    R2,                // 5
//    L2,                // 6
//    SpeedAdjust,       // 7               
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

//Motor DriverSetup
    driver.enable();

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
    for (byte i = 0; i < 8; i++) {
//        Serial.println(gotArray[i]);
        data[i] = gotArray[i];
                                    }

//Steering Loop
int AnalogStick = map(data[1] , 1, 255, 132 , 54);
    if (data[1] = 0) {
      myservo.write(95 + data[0]);}
    else if ( data[1] < 125) {
      myservo.write(AnalogStick + data[0]); }
    else if ( data[1] > 140) {
      myservo.write(AnalogStick + data[0]); }
    else {
      myservo.write(95 + data[0]);
    }
//130-131 is max left
//52-53 is max right  

//---( Section for Braking )---//
//    if (data[CROSS] == 1 ) {
//        driver.setBrakeMode(1);
//    }
//    else {
//        driver.setBrakeMode(0);
//    }
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
      (SpeedAdjust = 0.25);  
    }
//---( Section for DC Motor Driving )---//  
int Forwards = map(data[5] , 255, 5, 1, 100);
int Reverse = map(data[6] , 255, 5, 1, 100);
//    Boundary Check
    if (Forwards < 0)  {
      Forwards = 0;
    }
    if (Forwards > 100)  {
      Forwards = 100;
    }
    if (Reverse < 0 )  {
      Reverse = 0;
    }
    if (Reverse > 100)  {
      Reverse = 100;
    }
float FinalSpeedForwards = (Forwards * SpeedAdjust); 
    if (data[5] > 10) {
            driver.setOutput(-FinalSpeedForwards); //Forwards
    }
    else if (data[6] > 10) {
            driver.setOutput(Reverse);//Backwards
    }
    else if (data[5] < 10 && data[6] < 10) 
            {driver.setOutput(0);
    }
//Serial.println(FinalSpeedForwards); 
} 
//need an if radio disconnected/out of range set motor to 0
