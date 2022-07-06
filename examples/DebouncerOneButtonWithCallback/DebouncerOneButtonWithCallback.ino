#include <Arduino.h>
#include <devnetXButtonDebouncer.h>

// Define button pin
#define BUTTON_PIN		6

// Create debouncer instance
devnetXButtonDebouncer<1> Debouncer;
uint8_t Counter;

// Callback function
void buttonPressed(void *, uint8_t)
{
	Counter++;
	Serial.print("Button pressed ");
	Serial.print(Counter);
	Serial.println(" time(s)!");
}

void setup()
{
	// Init Serial
	Serial.begin(57600UL);

	// Init GPIO pin
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	// Init devnetXButtonDebouncer
	Debouncer.Begin();

	// Register button-press callback
	Debouncer.OnButtonPress(buttonPressed);
}

void loop()
{
	// Process the button. If you use only one button, the buttonID is always 0!
	Debouncer.Process(0, !digitalRead(BUTTON_PIN));
}
