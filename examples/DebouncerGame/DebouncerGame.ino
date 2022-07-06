#include <Arduino.h>
#include <devnetXButtonDebouncer.h>

// Define button pins
#define BUTTON1_PIN		6
#define BUTTON2_PIN		7

#define ABS(n)			((n) < 0 ? -(n) : (n))
#define SEPARATOR()		{ Serial.println("----------------------"); }

// Create debouncer instance
devnetXButtonDebouncer<2> Debouncer;

// Game variables
uint32_t TimeStamp;
uint32_t Player1PressedMillis;
uint32_t Player2PressedMillis;
uint8_t CurrentRandom;

// Game state machine
enum GameState {
	IDLE = 0, READY, ARMING, PRESS, RESULT, RESTART
} State;

// Non-blocking delay
bool Delayed(uint32_t duration)
{
	static uint32_t StartTime = millis();
	
	if (millis() - StartTime < duration)
		return true;

	StartTime = millis();
	return false;
}

void setup()
{
	// Init Serial
	Serial.begin(57600UL);

	// Init GPIOs
	pinMode(BUTTON1_PIN, INPUT_PULLUP);
	pinMode(BUTTON2_PIN, INPUT_PULLUP);

	// Init devnetXButtonDebouncer
	Debouncer.Begin();

	// Create callbacks
	Debouncer.OnButtonPress([](void *, uint8_t buttonID) {
		if (State != PRESS)
			return;
		
		if (buttonID == 0)
			Player1PressedMillis = millis();

		if (buttonID == 1)
			Player2PressedMillis = millis();
	});

	Serial.println("Welcome to the Game!");
	SEPARATOR();
	delay(2000);

	// Init random number generator
	randomSeed(analogRead(0));
}

void loop()
{
	static uint8_t CurrentCounter = 0;
	
	// Process the buttons
	Debouncer.Process(0, !digitalRead(BUTTON1_PIN));
	Debouncer.Process(1, !digitalRead(BUTTON2_PIN));

	switch (State)
	{
		case IDLE:
			if (Delayed(1500)) return;
			Serial.println("STARTING NEW GAME >");
			Serial.println("Player 1 and 2, do NOT press your buttons until I tell you!");
			
			CurrentCounter = 0;
			CurrentRandom = random(50) + 10;
			
			TimeStamp = 0;
			Player1PressedMillis = 0;
			Player2PressedMillis = 0;
			
			State = READY;

		case READY:
			if (Delayed(1500)) return;
			Serial.print("Get ready");

			State = ARMING;

		case ARMING:
			if (Delayed(100)) return;

			if (CurrentCounter < CurrentRandom)
			{
				Serial.print(".");
				CurrentCounter++;
				return;
			}

			Serial.println(" !!! PRESS YOUR BUTTONS NOW !!!");
			TimeStamp = millis();
			State = PRESS;

		case PRESS:
			if (Player1PressedMillis && Player2PressedMillis)
				State = RESULT;

			return;

		case RESULT:
			SEPARATOR();

			if (Player1PressedMillis < Player2PressedMillis)
			{
				Serial.println("Player 1 WINS!");
			}
			else if (Player1PressedMillis == Player2PressedMillis)
			{
				Serial.println("Draw!");
			}
			else if (Player1PressedMillis > Player2PressedMillis)
			{
				Serial.println("Player 2 WINS!");
			}

			SEPARATOR();
			
			Serial.print("Player 1 Time: ");
			Serial.print(Player1PressedMillis - TimeStamp);
			Serial.println("ms!");

			Serial.print("Player 2 Time: ");
			Serial.print(Player2PressedMillis - TimeStamp);
			Serial.println("ms!");

			Serial.print("   Difference: ");
			Serial.print(ABS((int32_t)(Player1PressedMillis - Player2PressedMillis)));
			Serial.println("ms!");

			SEPARATOR();

			Serial.println();

			State = RESTART;
		case RESTART:
			if (Delayed(5000)) return;
			State = IDLE;
			break;

	}
}
