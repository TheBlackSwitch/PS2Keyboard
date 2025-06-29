/*

# SP/2 Keyboard interface


---------------------------DESCRIPTION--------------------------------------

A simple library used to read a keyboard using the PS/2 protocol
Many modern keyboards still support the PS/2 protocol! 
You can use it through usb by connecting D+ (the CLOCK pin) to an interrupt 
pin and D- (the DATA pin) to any digital input pin. 
(you also have to supply the keyboard ofcourse)

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





//
//
//
//
//
//
//
//
//
//
//
//                                                                                              .
//                                                                                             @@@@@
//                                                                                            @@@-@@@@@+
//                                                                                           @@@     @@@@@@.
//                                                                                          @@@         =@@@@@@
//                                                                                         @@@              @@@@@
//                                                                                        @@@
//                                                                                       @@@
//                                                                                      @@@=
//                                                                                     :@@:
//                                                                                    +@@@
//                                                                                    @@@
//                                                                                  .@@@
//                                                                                  @@@
//                                                                                 @@@
//                                                                                @@@
//                                                                               @@@
//                                                                              @@@
//                                                                 //@@@@@@@@@@:@@@
//                                                             :@@@@@@@@@@@@@@@@@@
//                                                           @@@@@             .@@@@
//                                                         @@@@                   @@@@
//                                                         %%                       ////=
//             @::::=%@@  @           @@       -@@. =@@  @       @@  -@//  =@@  @     @        @=@::::@@:::   *@@  *@@  @        @
//             @       +// @           @:@     @//       @ @      @   =@       @ @*    @//      @ =@    @=     @=       @ @        @
//             @        @ @          @// @    //@          @    @//    %@       @  @     @      @ =@    @=    @=          @        @
//             @       @  @         :@   @   @           @   @       @@         +=    @//    @: =@    @=    @           @        @
//             @=====@@   @         @    @.  @           @ @ @          //@@:     @    @@   :@  =@    @=    @           @//=======@
//             @       .@ @        :@     @  @           @%   @=            -@   //@  @:://  @   =@    @=    @           @        @
//             @        @ @        @      @% @@          @     %@   @        @    @ -@  @  @   =@    @=    @:          @        @
//             @ =*=.   @ @       @*       @  @=       @ @       @  @@       @    //@@   =@@    =@    @=     @.      :% @        @
//             @=:=:==@@  @......:@        @+  @@*  -@@  @        @   @//   @@      @@    @@    =@    @=      @@=  =@@  @        @
//                                                         ::                       ::
//                                                         @@@@                   @@@@
//                                                           @@@@@              @@@@
//                                                             =@@@@@@@@@@@@@@@@@@
//                                                             @@@%@@@@@@@@@@@:
//                                                            @@@:
//                                                           @@@+
//                                                          :@@+
//                                                         //@@+
//                                                         @@@
//                                                       :@@@
//                                                       @@@
//                                                      @@@
//                                                     @@@
//                                                    @@@
//                                                   @@@
//                                                  @@@
//                              :@@@@              @@@
//                                //@@@@@//         @@@
//                                    @@@@@@:    @@@+
//                                        @@@@@@+@@:
//                                           =@@@@@
//                                               :
//
//
//
//
//
//
//
//
//
//
//

#ifndef PS2KEYBOARD
#define PS2KEYBOARD

#include "pins_arduino.h"
#include "Arduino.h"
#include <stdint.h>


//Not extended
#define KEY_TAB 0x0D
#define KEY_BACKSPACE 0x66
#define KEY_ENTER 0x5A
#define KEY_CAPSLOCK 0x58
#define KEY_LSHIFT 0x12
#define KEY_RSHIFT 0x59
#define KEY_ESC 0x76
#define KEY_NUMLOCK 0x77
#define KEY_SCROLLLOCK 0x7E
#define KEY_PAUSE 0xE1

//Extended
#define KEY_RCTRL_EXTENDED 0x14
#define KEY_RALT_EXTENDED 0x11
#define KEY_LEFT_EXTENDED 0x6B
#define KEY_RIGHT_EXTENDED 0x74
#define KEY_UP_EXTENDED 0x75
#define KEY_DOWN_EXTENDED 0x72
#define KEY_INSERT_EXTENDED 0x70
#define KEY_DELETE_EXTENDED 0x71
#define KEY_HOME_EXTENDED 0x6C
#define KEY_END_EXTENDED 0x69
#define KEY_PAGE_UP_EXTENDED 0x7D
#define KEY_PAGE_DOWN_EXTENDED 0x7A
#define KEY_KP_ENTER_EXTENDED 0x5A
#define KEY_KP_SLASH_EXTENDED 0x4A
#define KEY_WINDOWS_L_EXTENDED 0x1F
#define KEY_WINDOWS_R_EXTENDED 0x27
#define KEY_APPS_EXTENDED 0x2F
#define KEY_PRINT_SCREEN_EXTENDED 0x7C
#define KEY_BREAK_EXTENDED 0x7E

#define KEY_MEDIA_PLAY 0x15
#define KEY_MEDIA_NEXT 0x19
#define KEY_MEDIA_PREV 0x10
#define KEY_MEDIA_VOLUP 0x30
#define KEY_MEDIA_VOLDOWN 0x20

#endif


struct key_pressed {
  int scancode; //The unique code of this key pressed
  bool is_extended; //Whenever this key is part of the extended key codes (extended key codes can overlap with non extended ones!!)
  bool is_pressed; //True whenever the key is pressed. When it is released, it is false.
};

enum data_received {
  FALSE,
  NOT_FULLY,
  TRUE
};

class PS2Keyboard {
    private:
        data_received is_valid(int idx);
    public:
        void begin(uint8_t clock_pin /* should be interrupt */, uint8_t data_pin);
        bool has_data();
        key_pressed read_data();
        char key_to_char_azerty(key_pressed data, bool shift);
        char key_to_char_querty(key_pressed data, bool shift);
};


