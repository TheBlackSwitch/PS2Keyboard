/*

# PS2Keyboard interface


---------------------------DESCRIPTION--------------------------------------

A simple library used to read a PS2Keyboard using the PS/2 protocol
Many modern PS2Keyboards still support the PS/2 protocol!
You can use it through usb by connecting D+ (the CLOCK pin) to an interrupt
pin and D- (the DATA pin) to any digital input pin.
(you also have to supply the PS2Keyboard ofcourse)

------------------------------LICENSE---------------------------------------

 This software by theblackswitch is licensed under CC-BY-NC
 https://creativecommons.org/licenses/by-nc/4.0/

 meaning you are allowed to:
 - Copy the code for personal use
 - Redistribute the code (for non commercial purposes) and make changes

 But under the following terms:
 - You must give appropriate credit
 - The reditributed material must be licensed under the same license

----------------------------------------------------------------------------
*/

/*--------------------------------------------------------------------------

This example turns on an led at pin 13 when you press SHIFT and 
turns it off when you press DOWN ARROW

--------------------------------------------------------------------------*/




#include <PS2Keyboard.h>

PS2Keyboard keyboard;  //create the object, there can only be one single keyboard object

void setup() {
  Serial.begin(9600);                                                             //Start serial monitor
  keyboard.begin(3 /* CLOCK PIN (D+) must be interrupt*/, 4 /* DATA PIN (D-)*/);  //initialize our keyboard
  pinMode(13, OUTPUT);                                                            //Say that we want to use pin 13 as an output
}

void loop() {
  if (keyboard.has_data()) {                 //When we recieved data
    key_event key = keyboard.read_data();  //Read the recieved data (this can only be done once as it is removed from the buffer once it has been read)
   /* key_event is a custom data structure consisting of 3 variables:
       - key.scancode => The unique code of this key pressed
       - key.is_extended => Whenever this key is part of the extended key codes (extended key codes can overlap with non extended ones!!)
       - key.is_pressed => True whenever the key is pressed. When it is released, it is false.
   */

   if(key.scancode == KEY_LSHIFT && !key.is_extended) { //When we recieve an update for LEFT SHIFT. Note that we need to specify that KEY_LSHIFT is a non extended key
                                                        // as there may be another extended key with the same scan code
      if(key.is_pressed) { //When the key is pressed, turn on the led
         digitalWrite(13, HIGH);
      }
   }
   if(key.scancode == KEY_DOWN_EXTENDED && key.is_extended) { //When we recieve an update for DOWN ARROW. Note that we need to specify that KEY_DOWN_EXTENDED is an extended key
                                                              // as there may be another non extended key with the same scan code
      if(key.is_pressed) { //When the key is pressed, turn off the led
         digitalWrite(13, LOW);
      }
   }
  }
}
