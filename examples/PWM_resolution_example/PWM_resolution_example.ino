/*
By default Arduino's analogWrite (and consequently pwmWrite() since it mimics analogWrite()) uses 8 bit 
pwm across all timers. 8 bit PWM allows for a total of 256 possible values. This library has some methods 
for fine tuning resolution if a higher resolution is needed:

void pwmWriteHR(uint8_t pin, uint16_t duty)
  Same a pwmWrite, except the range is 0 - 65535 (16 bit) instead 
  of 0 - 255 (8 bit)

float TimerX_GetResolution() (replace X with a timer number)
  Gets the timer's resolution in base 2. The value returned in other words 
  represents the number of bits required to represent the timer's range. ie 
  the value 7 means it takes 7 bits to represent all possible pin duties at 
  that frequency, or 7-bit resolution. Note that a float is returned, not 
  an int. 

float GetPinResolution(uint8_t pin)
  Returns the same value as TimerX_GetResolution(), but takes a pin number
  as a parameter so that the caller is timer agnostic.

There are several things to keep in mind when trying to optimize resolution:
 -pwmWriteHR() is only useful for 16 bit timers, since 8 bit timers are inherently limited to 8 bit pwm
 -The higher the frequency, the lower the resolution. pwmWriteHR() will automatically map input to the 
  actual timer resolution 
 -Resolution can be optimized in this way for 2 pins on the Uno (9 and 10), and 11 pins on the Mega (2, 
 3, 5, 6, 7, 8, 11, 12, 44, 45,  and 46)
  
Use the serial moniter to see output from this program
This example runs on mega and uno.
*/

#include <PWM.h>

//use pin 11 on the mega for this example to work
int led = 9; // the pin that the LED is attached to

void setup()
{
  InitTimersSafe(); //initialize all timers except for 0, to save time keeping functions
  Serial.begin(115200);
  Serial.println();
  
  demonstrateFrequencysEffectOnResolution();
  settingHighResolutionDuty();
}

void demonstrateFrequencysEffectOnResolution()
{
  Serial.println("As frequency increases, resolution will decrease...");
  for(int i = 1; i < 10000; i+=10)
    {
      SetPinFrequency(led, i);  //setting the frequency
      
      uint16_t frequency = Timer1_GetFrequency();
      uint16_t decimalResolution = Timer1_GetTop() + 1;
      uint16_t binaryResolution = GetPinResolution(led); //this number will be inaccurately low because the float is being truncated to a int
      
      char strOut[75];
      sprintf(strOut, "Frequency: %u Hz\r\n Number of Possible Duties: %u\r\n Resolution: %u bit\r\n", frequency, decimalResolution, binaryResolution );
      
      Serial.println(strOut);
    } 
    
    Serial.println("...Finished");
}

void settingHighResolutionDuty()
{
 SetPinFrequency(led, 10); //setting the frequency to 10 Hz
 Serial.println("\r\npwmWrite() and pwmWriteHR() are identical except for the valid range of inputs.\r\nThe following loop calls both functions to produce the same result on the \r\nLED pin. The pin should to run 10Hz at 50% duty regardless of the function called.\r\n");
 
 //the led should flicker (10Hz 50% duty) for 1 second before calling
 //the other function. This demonstrates the use of pwmWriteHR() and how its
 //use is nearly identical to pwmWrite()
 while(true)
 {
   //setting the duty to 50% with 8 bit pwm. 128 is 1/2 of 256
   pwmWrite(led, 128); 
   Serial.println("8-Bit PWM");
   delay(1000);
   
   //setting the duty to 50% with the highest possible resolution that 
   //can be applied to the timer (up to 16 bit). 1/2 of 65536 is 32768.
   pwmWriteHR(led, 32768);
   Serial.println("High Resolution PWM");
   delay(1000);
 }
 
}

void loop()
{
}
