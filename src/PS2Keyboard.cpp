/*

# SP/2 PS2Keyboard interface


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

#include "PS2Keyboard.h"
#include <Arduino.h>

int CLK; // should be interrupt
int DAT;

uint8_t curr_bit = 0;

int val = 0;

long last_bit_time = 0;

int data_buffer[8];

long data_age[8];

void PS2Keyboard_interrupt()
{ // every time the clock signal goes low

    // Check if there is an error (meaning not enough bits where recieved after 10ms of waiting)
    if (val != 0 && last_bit_time + 10 < millis())
    {
        val = 0;
        curr_bit = 0;
    }

    last_bit_time = millis();

    // Store the recieved bit in a buffer
    if (digitalRead(DAT))
    {
        val = (val >> 1) | 0b10000000000; // LSBF so the bit recieved is the most significant bit for now
    }
    else
    {
        val = val >> 1;
    }
    curr_bit++;

    // if we recieved a full packet (1 start bit | 8 data bits (scancode) | 2 end bits) we store it in the buffer
    if (curr_bit > 10)
    {

        // only read the 8 databits, we don't need the rest
        val = val >> 1;
        val &= 0xFF;

        // add it at the front of the data buffer and move all items 1 backwards
        for (uint8_t i = 7; i > 0; i--)
        {
            data_buffer[i] = data_buffer[i - 1];
        }
        data_buffer[0] = uint8_t(val);

        // add it at the front of the data buffer and move all items 1 backwards
        for (uint8_t i = 7; i > 0; i--)
        {
            data_age[i] = data_age[i - 1];
        }
        data_age[0] = millis();

        // Reset our byte buffer
        val = 0;
        curr_bit = 0;
    }
}

data_received PS2Keyboard::is_valid(int idx)
{
    if (data_buffer[idx] == 0xF0 && (idx == 0 || data_buffer[idx - 1] == 0))
    { // If we haven't recieved all the data yet, the key is invalid
        // If we haven't received all the data after 100ms there may be an error so we remove this value
        if (data_age[idx] + 100 < millis())
        {
            data_buffer[idx] = 0;
            data_age[idx] = 0;
            return FALSE; // this data was not valid
        }
        else
        {
            return NOT_FULLY; // this data wasn't fully received yet
        }
    }
    else if (data_buffer[idx] == 0xE0 && (idx == 0 || data_buffer[idx - 1] == 0))
    { // Extended key but we haven't received all the data yet
        // If we haven't received all the data after 100ms there may be an error so we remove this value
        if (data_age[idx] + 100 < millis())
        {
            data_buffer[idx] = 0;
            data_age[idx] = 0;
            return FALSE; // this data was not valid
        }
        else
        {
            return NOT_FULLY; // this data wasn't fully received yet
        }
    }
    else if (data_buffer[idx] == 0xE0 && data_buffer[idx - 1] == 0xF0 && (idx == 1 || data_buffer[idx - 2] == 0))
    { // extended key being released but we don't have all the data yet
        // If we haven't received all the data after 100ms there may be an error so we remove these values
        if (data_age[idx] + 100 < millis())
        {
            data_buffer[idx] = 0;
            data_buffer[idx - 1] = 0;
            data_age[idx] = 0;
            data_age[idx - 1] = 0;
            return FALSE; // this data was not valid
        }
        else
        {
            return NOT_FULLY; // this data wasn't fully received yet
        }
    }
    else if (data_buffer[idx] == 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void PS2Keyboard::begin(uint8_t clock_pin /* should be interrupt */, uint8_t data_pin)
{
    CLK = clock_pin;
    DAT = data_pin;
    pinMode(CLK, INPUT_PULLUP);
    pinMode(DAT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLK), PS2Keyboard_interrupt, FALLING);
}

bool PS2Keyboard::has_data()
{

    // Check if we have recieved any data
    for (int i = 7; i >= 0; i--)
    {
        if (is_valid(i) == TRUE)
        { // Found valid data
            return true;
        }
        else if (is_valid(i) == NOT_FULLY)
        { // Not fully received the data yet so we don't know
            return false;
        }
    }
    return false; // No valid data found
}

key_event PS2Keyboard::read_data()
{
    key_event return_data;
    // read the first char received (that means it's at the last index of the buffer)
    for (int i = 7; i >= 0; i--)
    {
        if (is_valid(i) == TRUE)
        { // If we found some valid data, look at what type of data sequence it is

            if (data_buffer[i] == 0xF0)
            {                                              // Normal key released
                return_data.is_pressed = false;            // the key is released
                return_data.is_extended = false;           // it's not an extended char code
                return_data.scancode = data_buffer[i - 1]; // get the key that is relaesed

                // remove this key data from the buffer
                data_buffer[i] = 0;
                data_buffer[i - 1] = 0;

                // return the data
                return return_data;
            }
            else if (data_buffer[i] == 0xE0 && data_buffer[i - 1] == 0xF0)
            {                                              // Extended key released
                return_data.is_extended = true;            // it's an extended char code
                return_data.is_pressed = false;            // the key is released
                return_data.scancode = data_buffer[i - 2]; // get the key that is relaesed

                // remove this key data from the buffer
                data_buffer[i] = 0;
                data_buffer[i - 1] = 0;
                data_buffer[i - 2] = 0;

                // return the data
                return return_data;
            }
            else if (data_buffer[i] == 0xE0)
            {                                              // Extended key pressed
                return_data.is_extended = true;            // it's an extended char code
                return_data.is_pressed = true;             // the key is released
                return_data.scancode = data_buffer[i - 1]; // get the key that is relaesed

                // remove this key data from the buffer
                data_buffer[i] = 0;
                data_buffer[i - 1] = 0;

                // return the data
                return return_data;
            }
            else
            { // normal key press

                return_data.is_pressed = true;
                return_data.is_extended = false;
                return_data.scancode = data_buffer[i];
                data_buffer[i] = 0;
                return return_data;
            }
        }
    }
}

char PS2Keyboard::key_to_char_azerty(key_event data, bool shift)
{
    if (!data.is_extended)
    {
        switch (data.scancode)
        {
        // Letters
        case 0x15:
            return shift ? 'A' : 'a';
        case 0x32:
            return shift ? 'B' : 'b';
        case 0x21:
            return shift ? 'C' : 'c';
        case 0x23:
            return shift ? 'D' : 'd';
        case 0x24:
            return shift ? 'E' : 'e';
        case 0x2B:
            return shift ? 'F' : 'f';
        case 0x34:
            return shift ? 'G' : 'g';
        case 0x33:
            return shift ? 'H' : 'h';
        case 0x43:
            return shift ? 'I' : 'i';
        case 0x3B:
            return shift ? 'J' : 'j';
        case 0x42:
            return shift ? 'K' : 'k';
        case 0x4B:
            return shift ? 'L' : 'l';
        case 0x4C:
            return shift ? 'M' : 'm';
        case 0x31:
            return shift ? 'N' : 'n';
        case 0x44:
            return shift ? 'O' : 'o';
        case 0x4D:
            return shift ? 'P' : 'p';
        case 0x2D:
            return shift ? 'R' : 'r';
        case 0x1B:
            return shift ? 'S' : 's';
        case 0x2C:
            return shift ? 'T' : 't';
        case 0x3C:
            return shift ? 'U' : 'u';
        case 0x2A:
            return shift ? 'V' : 'v';
        case 0x1D:
            return shift ? 'Z' : 'z';
        case 0x22:
            return shift ? 'X' : 'x';
        case 0x35:
            return shift ? 'Y' : 'y';
        case 0x1A:
            return shift ? 'W' : 'w';
        case 0x1C:
            return shift ? 'Q' : 'q';

        // Numbers (Top row)
        case 0x16:
            return shift ? '1' : '&';
        case 0x1E:
            return shift ? '2' : 'é';
        case 0x26:
            return shift ? '3' : '"';
        case 0x25:
            return shift ? '4' : '\'';
        case 0x2E:
            return shift ? '5' : '(';
        case 0x36:
            return shift ? '6' : '§';
        case 0x3D:
            return shift ? '7' : 'è';
        case 0x3E:
            return shift ? '8' : '!';
        case 0x46:
            return shift ? '9' : 'ç';
        case 0x45:
            return shift ? '0' : 'à';

        // Numbers keypad
        case 0x69:
            return '1';
        case 0x72:
            return '2';
        case 0x7A:
            return '3';
        case 0x6B:
            return '4';
        case 0x73:
            return '5';
        case 0x74:
            return '6';
        case 0xCC:
            return '7';
        case 0x75:
            return '8';
        case 0x7D:
            return '9';
        case 0x70:
            return '0';

        // Symbols
        case 0x29:
            return ' '; // Space (no shift variant)
        case 0x0E:
            return shift ? '\0' : '²'; // No shift version
        case 0x55:
            return shift ? '_' : '-';
        case 0x4A:
            return shift ? '+' : '=';
        case 0x4E:
            return shift ? '°' : ')';
        case 0x54:
            return shift ? '¨' : '^';
        case 0x5D:
            return shift ? '£' : '$';
        case 0x52:
            return shift ? '%' : 'ù';
        case 0x5B:
            return shift ? '$' : '*'; // No shift defined
        case 0x3A:
            return shift ? '?' : ',';
        case 0x49:
            return shift ? '/' : ':';
        case 0x41:
            return shift ? '.' : ';';
        case 0x61:
            return shift ? '>' : '<';

        default:
            return '\0';
        }
    }
    return '\0';
}

char PS2Keyboard::key_to_char_querty(key_event data, bool shift)
{
    if (!data.is_extended)
    {
        switch (data.scancode)
        {
        // Letters
        case 0x1C:
            return shift ? 'A' : 'a';
        case 0x32:
            return shift ? 'B' : 'b';
        case 0x21:
            return shift ? 'C' : 'c';
        case 0x23:
            return shift ? 'D' : 'd';
        case 0x24:
            return shift ? 'E' : 'e';
        case 0x2B:
            return shift ? 'F' : 'f';
        case 0x34:
            return shift ? 'G' : 'g';
        case 0x33:
            return shift ? 'H' : 'h';
        case 0x43:
            return shift ? 'I' : 'i';
        case 0x3B:
            return shift ? 'J' : 'j';
        case 0x42:
            return shift ? 'K' : 'k';
        case 0x4B:
            return shift ? 'L' : 'l';
        case 0x3A:
            return shift ? 'M' : 'm';
        case 0x31:
            return shift ? 'N' : 'n';
        case 0x44:
            return shift ? 'O' : 'o';
        case 0x4D:
            return shift ? 'P' : 'p';
        case 0x15:
            return shift ? 'Q' : 'q';
        case 0x2D:
            return shift ? 'R' : 'r';
        case 0x1B:
            return shift ? 'S' : 's';
        case 0x2C:
            return shift ? 'T' : 't';
        case 0x3C:
            return shift ? 'U' : 'u';
        case 0x2A:
            return shift ? 'V' : 'v';
        case 0x1D:
            return shift ? 'W' : 'w';
        case 0x22:
            return shift ? 'X' : 'x';
        case 0x35:
            return shift ? 'Y' : 'y';
        case 0x1A:
            return shift ? 'Z' : 'z';

        // Numbers
        case 0x16:
            return shift ? '!' : '1';
        case 0x1E:
            return shift ? '@' : '2';
        case 0x26:
            return shift ? '#' : '3';
        case 0x25:
            return shift ? '$' : '4';
        case 0x2E:
            return shift ? '%' : '5';
        case 0x36:
            return shift ? '^' : '6';
        case 0x3D:
            return shift ? '&' : '7';
        case 0x3E:
            return shift ? '*' : '8';
        case 0x46:
            return shift ? '(' : '9';
        case 0x45:
            return shift ? ')' : '0';

        // Symbols
        case 0x29:
            return ' ';
        case 0x0E:
            return shift ? '~' : '`';
        case 0x4E:
            return shift ? '_' : '-';
        case 0x55:
            return shift ? '+' : '=';
        case 0x5D:
            return shift ? '|' : '\\';
        case 0x54:
            return shift ? '{' : '[';
        case 0x5B:
            return shift ? '}' : ']';
        case 0x4C:
            return shift ? ':' : ';';
        case 0x52:
            return shift ? '"' : '\'';
        case 0x41:
            return shift ? '<' : ',';
        case 0x49:
            return shift ? '>' : '.';
        case 0x4A:
            return shift ? '?' : '/';
        case 0x61:
            return '\\'; // Same for both

        default:
            return '\0';
        }
    }
    return '\0';
}