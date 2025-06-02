#include keyboard.h // kexboard for buttons maybe not used in this code
#include MIDIUSB.h // MIDIUSB for pots

const int NPots = 4; // Number of potentiometers
const int pots[NPots] = {A9, A8, A7, A6}; // Potentiometer pins
const int PotCState[NPots] = {0}; // current potentiometer states
const int PotPState[NPots] = {0}; // previous potentiometer states
int PotVar = 0; // pot difference 

int MidiCState[NPots] = {0}; // current midi state for pots 
int MidiPState[NPots] = {0}; // previous midi state for pots

const int TIMEOUT = 300; //time the pot will be red
const int VarThreshold = 10; // threshold for pot change
boolean potMoving = true; // if the pot is moving
unsigned long PTime[NPots]= {0}; // stored time 
unsigned long timer[NPots]= {0}; 

const int NButtons = 8; // Number of buttons
const int buttons[NButtons] = {10, 16, 14, 15, 18, 19, 20, 21}; // Button pins
const int ButtonCState[NButtons] = {}; // current button states
const int ButtonPState[NButtons] = {}; // previous button states

int Led1 = 7;  // Pin for the LED1

const int statusVAR = 0; // Status variable, not used in this code but can be used for debugging or other purposes

byte midiCh = 1; //* MIDI channel to be used
byte note = 36; //* Lowest note to be used; 36 = C2; 60 = Middle C
byte cc = 1; //* Lowest MIDI CC to be used

void setup() 
{
  Serial.begin(9600);
  
  for(int i = 0, i < NButtons, i++) // init buttons 
    {
        pinMode(buttons[i], INPUT_PULLUP); // butten pins with internal pullups
    }
}

void loop() 
{
    if (digitalRead(Button1) == LOW && digitalRead(Button2) == LOW && digitalRead(Button3) == LOW ) // if first 3 buttons are pressed then go into debug mode 
    {
        stausVAR = 1; // Set status variable to 1 for debug mode
        Serial.println("Debug mode activated");
        buttons_debvug(); // debug buttons functions
        pots_debug(); // debug pots functions
    }
    else 
    { 
        Serial.println("normal startup, no more information will be printed");
        buttons(); // buttons functions
        pots(); // pots functions
    }

}

// normal Buttons 