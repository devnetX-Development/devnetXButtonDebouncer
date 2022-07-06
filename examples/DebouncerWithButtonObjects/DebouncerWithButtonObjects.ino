#include <Arduino.h>
#include <devnetXButtonDebouncer.h>

// Define button pins
#define BUTTON1_PIN		6
#define BUTTON2_PIN		7
#define BUTTON3_PIN		8

// Create debouncer instances
devnetXButtonDebouncer<1> Button1;
devnetXButtonDebouncer<1> Button2;
devnetXButtonDebouncer<1> Button3;

void setup()
{
	// Init Serial
	Serial.begin(57600UL);

	// Init GPIOs
	pinMode(BUTTON1_PIN, INPUT_PULLUP);
	pinMode(BUTTON2_PIN, INPUT_PULLUP);
	pinMode(BUTTON3_PIN, INPUT_PULLUP);

	// Init devnetXButtonDebouncer
	Button1.Begin();
	Button2.Begin();
	Button3.Begin();

	// Create callbacks
	Button1.OnButtonPress([](void *, uint8_t buttonID) {
		Serial.println("OnButtonPress: Button 1");
	});

	Button2.OnButtonPress([](void *, uint8_t buttonID) {
		Serial.println("OnButtonPress: Button 2");
	});

	Button3.OnButtonPress([](void *, uint8_t buttonID) {
		Serial.println("OnButtonPress: Button 3");
	});
}

void loop()
{
	// Process the buttons. If you use only one button, the buttonID is always 0!
	Button1.Process(0, !digitalRead(BUTTON1_PIN));
	Button2.Process(0, !digitalRead(BUTTON2_PIN));
	Button3.Process(0, !digitalRead(BUTTON3_PIN));
}
