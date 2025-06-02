#include "MIDIUSB.h" // MIDIUSB for pots

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

unsigned long lastDebounceTime[NButtons] = {0};  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    //** the debounce time; increase if the output flickers

const int statusVAR = 0; // Status variable, not used in this code but can be used for debugging or other purposes

byte midiCh = 1; //* MIDI channel to be used
byte note = 36; //* Lowest note to be used; 36 = C2; 60 = Middle C
byte cc = 1; //* Lowest MIDI CC to be used

void setup() 
{
  Serial.begin(9600); // begin serial communication for debug mode
  
  for(int i = 0; i < NButtons; i++); // init buttons 
  {
    pinMode(buttons[i], INPUT_PULLUP); // button pins with internal pullups
  }
}

void loop() 
{
    if (digitalRead(Button1) == LOW && digitalRead(Button2) == LOW && digitalRead(Button3) == LOW ); // if first 3 buttons are pressed then go into debug mode 
    {
        stausVAR = 1; // Set status variable to 1 for debug mode
        Serial.println("Debug mode activated");
        Buttons_debug(); // debug buttons functions
        Pots_debug(); // debug pots functions
    }
    else 
    { 
        Serial.println("normal startup, no more information will be printed");
        Buttons(); // buttons functions
        Pots(); // pots functions
    }

}

// normal Buttons 
void Buttons() 
{

  for (int i = 0; i < NButtons; i++) 
  {
    buttonCState[i] = digitalRead(buttonPin[i]);  // read pins from arduino

    if ((millis() - lastDebounceTime[i]) > debounceDelay) 
    {
      if (buttonPState[i] != buttonCState[i]) 
      {
        lastDebounceTime[i] = millis();
        if (buttonCState[i] == LOW) 
        {
            //send node
          noteOn(midiCh, note + i, 127);  // channel, note, velocity
          MidiUSB.flush();
        }
        else 
        {
            //sends no note
          noteOn(midiCh, note + i, 0);  // channel, note, velocity
          MidiUSB.flush();

        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

// debug Buttons
void Buttons_debug() 
{
  for (int i = 0; i < NButtons; i++) 
  {
    buttonCState[i] = digitalRead(buttonPin[i]);  // read pins from arduino

    if ((millis() - lastDebounceTime[i]) > debounceDelay) 
    {
      if (buttonPState[i] != buttonCState[i]) 
      {
        lastDebounceTime[i] = millis();
        if (buttonCState[i] == LOW) 
        {
          Serial.print("Button ");
          Serial.print(i);
          Serial.println(" pressed");
          noteOn(midiCh, note + i, 127);  // channel, note, velocity
          MidiUSB.flush();
        }
        else 
        {
          Serial.print("Button ");
          Serial.print(i);
          Serial.println(" released");
          noteOn(midiCh, note + i, 0);  // channel, note, velocity
          MidiUSB.flush();
        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

// normal Pots
void Pots()
{
  for (int i = 0; i < NPots; i++) // Loops through all the potentiometers
  { 
    PotCState[i] = analogRead(pots[i]); // reads the pins from arduino

    MidiCState[i] = map(PotCState[i], 0, 1023, 0, 127); // Maps the reading of the potCState to a value usable in midi

    PotVar = abs(PotCState[i] - PotPState[i]); // Calculates the absolute value between the difference between the current and previous state of the pot

    if (PotVar > VarThreshold) // Opens the gate if the potentiometer variation is greater than the threshold
    { 
      PTime[i] = millis(); // Stores the previous time
    }

    timer[i] = millis() - PTime[i]; // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
    { 
      potMoving = true;
    }
    else 
    {
      potMoving = false;
    }

    if (potMoving == true) // If the potentiometer is still moving, send the change control
    { 
      if (midiPState[i] != midiCState[i]) 
      {
        // Sends  MIDI CC 
        // Use if using with ATmega32U4 (micro, pro micro, leonardo...)
        controlChange(midiCh, cc + i, midiCState[i]); //  (channel, CC number,  CC value)
        MidiUSB.flush();

        PotPState[i] = PotCState[i]; // Stores the current reading of the potentiometer to compare with the next
        MidiPState[i] = MidiCState[i];
      }
    }
  }
}

// debug Pots
void Pots_debug()
{
  for (int i = 0; i < NPots; i++) // Loops through all the potentiometers
  { 
    PotCState[i] = analogRead(pots[i]); // reads the pins from arduino

    MidiCState[i] = map(PotCState[i], 0, 1023, 0, 127); // Maps the reading of the potCState to a value usable in midi

    PotVar = abs(PotCState[i] - PotPState[i]); // Calculates the absolute value between the difference between the current and previous state of the pot

    if (PotVar > VarThreshold) // Opens the gate if the potentiometer variation is greater than the threshold
    { 
      PTime[i] = millis(); // Stores the previous time
    }

    timer[i] = millis() - PTime[i]; // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
    { 
      potMoving = true;
      Serial.println("Pot movings");
    }
    else 
    {
      potMoving = false;
    }

    if (potMoving == true) // If the potentiometer is still moving, send the change control
    { 
      if (midiPState[i] != midiCState[i]) 
      {
        Serial.print("Potentiometer ");
        Serial.print(i);
        Serial.print(" changed to: ");
        Serial.println(MidiCState[i]);
        
        // Sends MIDI CC 
        controlChange(midiCh, cc + i, midiCState[i]); //  (channel, CC number,  CC value)
        MidiUSB.flush();

        PotPState[i] = PotCState[i]; // Stores the current reading of the potentiometer to compare with the next
        MidiPState[i] = MidiCState[i];
      }
    }
  }
}