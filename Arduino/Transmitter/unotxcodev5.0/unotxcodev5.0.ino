//This code is ran on an arduino uno, with a 5K potentiometer
//nRF24L01+ is used as the radio transmitter, 3.3V breakout adapter helps with noisey 3/5V input
//USB host shield for PS4 controller has the VBUS 5V 2 pins shorted and the 3/5V next to ground shorted to each other respectively
//Transmitter powered via a 9V battery to the DC in with a switch wired in
//-----( Import needed libraries )-----/
#include <PS4USB.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
USB Usb;
PS4USB PS4(&Usb);

//-----( Declare Constants and Pin Numbers )---/
#define SCK_PIN 13
#define MISO_PIN 12
#define MOSI_PIN 11

#define PS4_VID         0x054C // Sony Corporation
#define PS4_PID         0x05C4 // PS4 Controller
#define PS4_PID_SLIM    0x09CC // PS4 Slim Controller

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe[1] = {0xE8E8F0F0E1LL}; // Define the transmit pipe

//Potentiometer Stuff To Allow more "On the Fly" Adjustments
int potPin = A5; //Middle pin of Pot to Analog pin A3
int potVal = 0;
int PowerPin1 = 2;//For an extra 5V output for Pot +

//---( Declare objects )---/
RF24 radio(7, 8); // Create a Radio

//---( Declare General Variables )---/
int Array[9]; //Can run into issues sending too many values, confirmed up to 8 works

void setup()   /**** SETUP: RUNS ONCE ****/
{
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();  //If disabled, will get faster receieve/transmission but may have interference  
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_2MBPS); //CAN MESS WITH THIS
  radio.stopListening();
  radio.openWritingPipe(pipe[0]);
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_HIGH);//Changing HIGH to RF24_PA_MAX
Serial.begin(115200); {
#if !defined(__MIPSEL__)
      while (!Serial); 
#endif
      if (Usb.Init() == -1) {
        Serial.print(F("\r\nOSC did not start"));
        while (1); // Halt
      }
      Serial.print(F("\r\nPS4 USB Library Started"));
   }
   
//Potentiometer Stuff, Need 5V Output to pull from
pinMode(PowerPin1, OUTPUT);
digitalWrite(PowerPin1, HIGH);

}//--(end setup )-


void loop() {
      Usb.Task();
      if (PS4.connected()) 
{ 
   potVal = analogRead(potPin);      
   int adjust = map(potVal, 0, 1000, -45, 45); 
   Array[0] = adjust ; //Potentiometer mapped value
   Array[1] = (PS4.getAnalogHat(LeftHatX));
   Array[2] = (PS4.getAnalogHat(RightHatX));
   Array[3] = (PS4.getAnalogHat(LeftHatY));  
   Array[4] = (PS4.getAnalogHat(RightHatY));  
   Array[5] = (PS4.getAnalogButton(R2));
   Array[6] = (PS4.getAnalogButton(L2));
   Array[7] = (PS4.getAnalogButton(R1));
   Array[8] = (PS4.getAnalogButton(L1));
   Array[9] = (PS4.getButtonClick(CROSS));
   Array[10] = (PS4.getButtonClick(TRIANGLE));
   Array[11] = (PS4.getButtonClick(SQUARE));
   Array[12] = (PS4.getButtonClick(CIRCLE));
//   Array[13] = (PS4.getButtonPress(R3));
//   Array[14] = (PS4.getButtonPress(L3));
//   Array[15] = (PS4.getButtonPress(LEFT));
//   Array[16] = (PS4.getButtonPress(RIGHT));
//   Array[17] = (PS4.getButtonPress(UP));
//   Array[18] = (PS4.getButtonPress(DOWN));
//   Array[19] = (PS4.getButtonPress(TOUCHPAD));
//   Array[20] = (PS4.getButtonPress(OPTIONS));
//   Array[21] = (PS4.getButtonPress(SHARE));
//   Array[22] = (PS4.getButtonPress(PS));


// Can change press to click if i want it to return true only when its initially pressed

//For Tilt Controls
//(PS4.getAngle(Pitch)); 
//(PS4.getAngle(Roll));
   // PS4.setLedFlash(0, 0); // Turn off blinking
   // PS4.setLed(Green/red/blue/yellow);
      //Can maybe do something with later if battery is dying
}
    Serial.println("Sent array = ");
    for (byte i = 0; i < 9; i++) {
        Serial.println(Array[i]);
    }
if (radio.available());
 radio.write(&Array, sizeof(Array));

  }
 
// }
//}

//--(end main loop )-
