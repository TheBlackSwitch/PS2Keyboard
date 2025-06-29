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

This example print the current word you have typed. Basically a simple text
editor but you can't change anything you have typed yet

--------------------------------------------------------------------------*/




#include <PS2Keyboard.h>

PS2Keyboard keyboard;  //create the object, there can only be one single keyboard object


//The message data
uint8_t curr_char = 0;
char message[256];

//Whenever L/R shift is pressed
bool LShift = false;
bool RShift = false;

void setup() {
  Serial.begin(9600); //Start serial monitor
  keyboard.begin(3 /* CLOCK PIN (D+) must be interrupt*/, 4 /* DATA PIN (D-)*/); //initialize our keyboard
}

void loop() {
  if(keyboard.has_data()) {
    //If we have recieved data, read it
    key_event key = keyboard.read_data();
    /* key_event is a custom data structure consisting of 3 variables:
       - key.scancode => The unique code of this key pressed
       - key.is_extended => Whenever this key is part of the extended key codes (extended key codes can overlap with non extended ones!!)
       - key.is_pressed => True whenever the key is pressed. When it is released, it is false.
    */
    
    //Read left shift pressed
    if(key.scancode == KEY_LSHIFT && !key.is_extended) {
      if(key.is_pressed) {
        LShift = true;
      } else {
        LShift = false;
      }
    }
    
    //Read right shift pressed
    if(key.scancode == KEY_RSHIFT && !key.is_extended) {
      if(key.is_pressed) {
        RShift = true;
      } else {
        RShift = false;
      }
    }

    //Convert try to convert the key data into an actual charracter (also uses shift to change the charracter)
    //char char_pressed = keyboard.key_to_char_azerty(key, LShift || RShift); /* for an azerty keyboard */
    char char_pressed = keyboard.key_to_char_azerty(key, LShift || RShift); /* for a querty keyboard */

    if(char_pressed != '\0' && key.is_pressed) { //if the key is pressed, add the charracter (when it found one) to the message and print it to the serial moitor
      message[curr_char] = char_pressed;
      curr_char += 1;

      for(int i = 0; i < 10; i++) { //10 newlines to remove the previous message sent
        Serial.println("");
      }

      Serial.println(message);
    }
  }
}




