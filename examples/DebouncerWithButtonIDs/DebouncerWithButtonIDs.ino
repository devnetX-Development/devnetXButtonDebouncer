#include <Arduino.h>
#include <devnetXButtonDebouncer.h>

// Define button pins
#define BUTTON1_PIN		6
#define BUTTON2_PIN		7
#define BUTTON3_PIN		8

// Create array with button pins
uint8_t const ButtonPins[] = {
	BUTTON1_PIN,
	BUTTON2_PIN,
	BUTTON3_PIN
};

// Create debouncer instance
devnetXButtonDebouncer<3> Debouncer;

void setup()
{
	// Init Serial
	Serial.begin(57600UL);

	// Init GPIOs
	for (uint8_t n = 0; n < Debouncer.ButtonCount(); n++) {
		pinMode(ButtonPins[n], INPUT_PULLUP);
	}

	// Init devnetXButtonDebouncer
	Debouncer.Begin();
}

void loop()
{
	// Process the buttons
	for (uint8_t buttonID = 0; buttonID < Debouncer.ButtonCount(); buttonID++)
	{
		switch (Debouncer.Process(buttonID, !digitalRead(ButtonPins[buttonID])))
		{
			case BDButtonState::PRESSED:
				Serial.print("Button ");
				Serial.print(buttonID);
				Serial.println(" is PRESSED!");
				break;
			
			case BDButtonState::RELEASED:
				Serial.print("Button ");
				Serial.print(buttonID);
				Serial.println(" is RELEASED!");
				break;

			default:
				break;
		}
	}
}
