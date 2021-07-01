//===================================================================================
// Code to drive a dance pad based on Promit's awesome work and design:
// https://ventspace.wordpress.com/2018/04/09/danceforce-v3-diy-dance-pad-controller/
//
// Some of this code is Teensy-specific. Make sure your Arduino IDE is setup as
// described according to Promit's instructions at the link above (in particular
// don't overlook installing the Teensy additions to the Arduino IDE and setting
// the USB type). For example, that will pull in the right joystick code described
// here: https://www.pjrc.com/teensy/td_joystick.html
//
// The definitions below are meant to be tweaked to adapt to your individual pad.
//===================================================================================

// This is the LED pin for a Teensy LC, may need to change on other boards
const int LedPin = 13;

// Pin mappings for where things got soldered
// These are the #'s from the "A#" pins in the Teensy LC pinout diagram
int pin_mappings[6] = {0, 1, 2, 3, 4, 5};

// The analog threshold value for triggering a button
const int TriggerThreshold = 4;

// How many consecutive times to detect a press before sending the button event
// Don't stress that this is kind of a lot. Testing with 5 still showed enough bouncing
// to cause frequent missed steps. It was very frustrating. Steps would sail on by that
// I knew I had hit. One way to see this more clearly is to use a tool like jstest on Linux
// that prints out each down/up event for each button and then dance on it like you would in
// the real game.
const int DebounceThreshold = 10;

// Enable SERIAL_DEBUG if you need to debug the electricals of the pad
//#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
int serial_debug_count = 0;
#endif

//===================================================================================
// The rest below should mostly "just work" as is, but feel free to tweak as needed!
//===================================================================================

// Counts how many consecutive times each button has been detected as pressed
int debounce_count[6] = {0};

// Whether the Joystick.button() status is pressed or not
int button_status[6] = {0};

void setup()
{
  Serial.begin(38400);
  pinMode(LedPin, OUTPUT);

  // The analog pins are configured with internal pull-up resistors, which makes for a very simple
  // circuit. However, this method does not support useful pressure sensitivity adjustments.
  // By soldering 1K resistors as pull-ups on the board, you can make the buttons require more pressure.
  // The first version did that, but making the buttons more difficult didn't seem very desirable.
  pinMode(pin_mappings[0] + 14, INPUT_PULLUP);
  pinMode(pin_mappings[1] + 14, INPUT_PULLUP);
  pinMode(pin_mappings[2] + 14, INPUT_PULLUP);
  pinMode(pin_mappings[3] + 14, INPUT_PULLUP);
  pinMode(pin_mappings[4] + 14, INPUT_PULLUP);
  pinMode(pin_mappings[5] + 14, INPUT_PULLUP);
}

void loop()
{
  // analog read values
  int analog_values[6] = {0};
  // check if any buttons are pressed, so we know whether to light the LED
  bool button_pressed = false;

  // read each pin, and set that Joystick button appropriately
  for(int idx = 0; idx < 6; ++idx)
  {
    analog_values[idx] = analogRead(pin_mappings[idx]);
    if(analog_values[idx] < TriggerThreshold)
    {
      if (debounce_count[idx] < DebounceThreshold)
      {
        debounce_count[idx]++;
      }

      if (debounce_count[idx] == DebounceThreshold)
      {
        button_pressed = true;
        if (!button_status[idx])
        {
          Joystick.button(idx+1, 1);
          button_status[idx] = 1;
        }
      }
    }
    else
    {
      if (debounce_count[idx] > 0)
      {
        debounce_count[idx]--;
      }

      if (debounce_count[idx] == 0)
      {
        if (button_status[idx])
        {
          Joystick.button(idx+1, 0);
          button_status[idx] = 0;
        }
      }
    }
  }

  // Illuminate the LED if a button is pressed
  if(button_pressed)
    digitalWrite(LedPin, HIGH);
  else
    digitalWrite(LedPin, LOW);

#ifdef SERIAL_DEBUG
  serial_debug_count++;
  if(serial_debug_count == 500)
  {
    serial_debug_count = 0;
    Serial.printf("Levels: %d %d %d %d (%d %d)\n",
                  analog_values[0], analog_values[1], analog_values[2],
                  analog_values[3], analog_values[4], analog_values[5]);
  }
#endif
}
