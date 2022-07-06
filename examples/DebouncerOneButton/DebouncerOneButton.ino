#include <Arduino.h>
#include <devnetXButtonDebouncer.h>

// Define button pin
#define BUTTON_PIN		6

// Create debouncer instance
devnetXButtonDebouncer<1> Debouncer;

void setup()
{
	// Init Serial
	Serial.begin(57600UL);

	// Init GPIO pin
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	// Init devnetXButtonDebouncer
	Debouncer.Begin();
}

void loop()
{
	// Process the button. If you use only one button, the buttonID is always 0!
	switch (Debouncer.Process(0, !digitalRead(BUTTON_PIN)))
	{
		case BDButtonState::PRESSED:
			Serial.println("Button is PRESSED!");
			break;
		
		case BDButtonState::HOLD:
			Serial.println("Button is HOLD!");
			break;

		case BDButtonState::REPEAT:
			Serial.println("Button is REPEAT!");
			break;

		case BDButtonState::RELEASED:
			Serial.println("Button is RELEASED!");
			break;

		default:
			break;
	}

}
