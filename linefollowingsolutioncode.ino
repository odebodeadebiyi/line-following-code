//****************************************//
//* Example Code for Sending             *//
//* Signed Integers over I2C             *//
//* ESP32 (Master) to Arduino (Slave)    *//
//*                                      *//
//* Master Code                          *//
//*                                      *//
//* UoN 2022 - Nat Dacombe               *//
//****************************************//

// read through all of the code and the comments before asking for help
// research 'two's complement' if this is not familiar to you as it is used to represented signed (i.e. positive and negative) values

#include <Wire.h>
#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal
//Define the pin numbers for the photodiodes and infrared LEDs
#define NUM_LEDS 6
#define NUM_PHOTODIODES 6

int ledPins[NUM_LEDS] = {32, 33, 25, 26, 27, 14};
int photodiodePins[NUM_PHOTODIODES] = {32, 33, 25, 26, 27, 14};

//Define an array to store the photodiode readings
int photodiodeReading[NUM_PHOTODIODES];

void setup()
{
  Serial.begin(115200);
  // Set up the IR LED pins as output pins
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  //Set up the photodiodes as input pins
  for (int i = 0; i < NUM_PHOTODIODES; i++) {
    pinMode(photodiodePins[i], INPUT);
  }
  Wire.begin();   // join i2c bus (address optional for the master) - on the Arduino NANO the default I2C pins are A4 (SDA), A5 (SCL)
}

// the minimum and maximum values here are determined by the amount of bits used by the chosen variable type
// for int, this is either 16-bits or 32-bits
// due to two's complement, the minimum value is -2^(N-1), and the maximum is (2^(N-1))-1; where N is the number of bits
int x;
int y;
int z;


void loop()
{
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device #4
  /* depending on the mirocontroller, the int variable is stored as 32-bits or 16-bits
     if you want to increase the value range, first use a suitable variable type and then modify the code below
     for example; if the variable used to store x and y is 32-bits and you want to use signed values between -2^31 and (2^31)-1
     uncomment the four lines below relating to bits 32-25 and 24-17 for x and y
     for my microcontroller, int is 32-bits hence x and y are AND operated with a 32 bit hexadecimal number - change this if needed

     >> X refers to a shift right operator by X bits
  */
  //Wire.write((byte)((x & 0xFF000000) >> 24)); // bits 32 to 25 of x
  //Wire.write((byte)((x & 0x00FF0000) >> 16)); // bits 24 to 17 of x
  Wire.write((byte)((x & 0x0000FF00) >> 8));    // first byte of x, containing bits 16 to 9
  Wire.write((byte)(x & 0x000000FF));           // second byte of x, containing the 8 LSB - bits 8 to 1
  //Wire.write((byte)((y & 0xFF000000) >> 24)); // bits 32 to 25 of y
  //Wire.write((byte)((y & 0x00FF0000) >> 16)); // bits 24 to 17 of y
  Wire.write((byte)((y & 0x0000FF00) >> 8));    // first byte of y, containing bits 16 to 9
  Wire.write((byte)(y & 0x000000FF));           // second byte of y, containing the 8 LSB - bits 8 to 1 
  Wire.write((byte)((z & 0x0000FF00) >> 8));
  Wire.write((byte)(z & 0x000000FF)); 
  Wire.endTransmission();   // stop transmitting
  delay(100);

  //Turns on each LED and takes a reading from its corresponding sensor
  for (int i = 0; i < NUM_LEDS; i++) {
    //Turn on the LED
    digitalWrite(ledPins[i], HIGH);
    delayMicroseconds(10);
    //Read the sensor value
    photodiodeReading[i] = analogRead(photodiodePins[i]);
    //Turn off the LED
    digitalWrite(ledPins[i], LOW);
  }

  //Ouputs photodiode readings to the serial monitor
  for (int i = 0; i < NUM_PHOTODIODES; i++) {
    Serial.print(photodiodeReading[i]);
    Serial.print(" ");
  }
  Serial.println();

  //sensor values used to calculate an error value
  int error =  0;
  for (int i = 0; i < NUM_LEDS; i++) {
    error += (i-2) * photodiodeReading[i];
  }

  //turns left if error less than 0
  if (error < 0){
    //Turn left
    x = 100;
    y = -210;
    z = 72;
  } 
  // turns right if error value greater than 0
  else if (error > 0) {
    //Turn right
    x = 200;
    y = -100;
    z = 92;
  } 
  // if error value is equal to 0 the car goes straight
  else {
    //Go straight
    x = 200;
    y = -210;
    z = 82;
  }
}
