/// \file      devnetXButtonDebouncer.h
/// \brief     A simple Arduino Button Debouncer Library that provides easy button handling.
/// \author    Alexej Goujine (dev@devnetx.at)
/// \copyright Copyright (c) 2019-2022 devnetX Development

#pragma once

#include <Arduino.h>

#define BD_DEFAULT_DEBOUNCE_PRESS_TIME		50UL
#define BD_DEFAULT_HOLD_TIME				250U
#define BD_DEFAULT_REPEAT_INTERVAL_TIME		100UL
#define BD_DEFAULT_DEBOUNCE_RELEASE_TIME	50UL

typedef enum class BDButtonState : uint8_t
{
	IDLE_UNCHANGED =	0U,
	PRESSED	=			1U,
	HOLD =				2U,
	REPEAT =			4U,
	RELEASED =			8U
} BDButtonState_t;

constexpr inline BDButtonState operator| (const BDButtonState x, const BDButtonState y)
{
 	return (BDButtonState)((uint8_t)x | (uint8_t)y);
}

constexpr inline bool operator& (const BDButtonState x, const BDButtonState y)
{
 	return ((uint8_t)x & (uint8_t)y);
}

template<uint8_t N>
class devnetXButtonDebouncer
{
	public:

		void *Parent;
		
		typedef struct ButtonSettings
		{
			uint8_t				ButtonDebouncePressTime =		BD_DEFAULT_DEBOUNCE_PRESS_TIME;
			uint16_t			ButtonHoldTime =				BD_DEFAULT_HOLD_TIME;
			uint16_t			ButtonRepeatIntervalTime =		BD_DEFAULT_REPEAT_INTERVAL_TIME;
			uint8_t				ButtonDebounceReleaseTime =		BD_DEFAULT_DEBOUNCE_RELEASE_TIME;
		} ButtonSettings_t;

		// Define Settings array for the specified amount of buttons
		ButtonSettings_t		Settings[N];

	private:

		typedef struct ButtonData
		{
			uint32_t			ButtonTime =					0;
			BDButtonState_t		ButtonState =					BDButtonState::IDLE_UNCHANGED;
			bool 				ButtonPressed =					false;
		} ButtonData_t;

		// Define Data array for the specified amount of buttons
		ButtonData_t			Data[N];
		
	public:

		/// Create a new devnetXButtonDebouncer object
		/// 
		devnetXButtonDebouncer(void)
		{
			this->Parent = NULL;
		}

		/// Initialize the Button Bebouncer
		/// 
		/// @param parent Optional Pointer to the creator instance (this)
		void Begin(void *parent = NULL)
		{
			this->Parent = parent;

			this->ButtonPressFunction = NULL;
			this->ButtonHoldFunction = NULL;
			this->ButtonRepeatFunction = NULL;
			this->ButtonPressDoneFunction = NULL;
			this->ButtonRepeatDoneFunction = NULL;
			this->ButtonReleasedFunction = NULL;
		}

		/// Return the ButtonCount 
		/// 
		/// @return uint8_t Amount of buttons handled by this debouncer instance
		constexpr inline uint8_t const ButtonCount(void) const
		{
			return N;
		}

		/// Process the specified button
		/// 
		/// @param buttonID ID of the button
		/// @param active bool button pin (positive logic)
		/// @return true A callback was raised
		/// @return false No action
		BDButtonState_t Process(uint8_t buttonID, bool active)
		{
			uint32_t currentMillis = millis();

			// Sanity check
			if (buttonID > N - 1)
				buttonID = N - 1;

			// Check button state and handle it
			if (active == true)
			{
				if (this->Data[buttonID].ButtonState == BDButtonState::IDLE_UNCHANGED)
				{
					// Button is now pressed, debouncing it
					this->Data[buttonID].ButtonState = BDButtonState::PRESSED;
					this->Data[buttonID].ButtonTime  = currentMillis;
					this->Data[buttonID].ButtonPressed = true;

					return BDButtonState::IDLE_UNCHANGED;
				}
				else if ((this->Data[buttonID].ButtonState == BDButtonState::PRESSED) && (currentMillis - this->Data[buttonID].ButtonTime  > this->Settings[buttonID].ButtonDebouncePressTime))
				{
					// Button is pressed and debounced
					this->Data[buttonID].ButtonState = BDButtonState::HOLD;
					this->Data[buttonID].ButtonTime  = currentMillis;

					if (this->ButtonPressFunction != NULL)
						(*ButtonPressFunction)(this->Parent, buttonID);

					return BDButtonState::PRESSED;
				}
				else if ((this->Data[buttonID].ButtonState == BDButtonState::HOLD) && (currentMillis - this->Data[buttonID].ButtonTime  > this->Settings[buttonID].ButtonHoldTime))
				{
					// Button is pressed and hold
					this->Data[buttonID].ButtonState = BDButtonState::REPEAT;
					this->Data[buttonID].ButtonTime  = currentMillis;

					if (this->ButtonHoldFunction != NULL)
						(*ButtonHoldFunction)(this->Parent, buttonID);

					return BDButtonState::HOLD;
				}
				else if ((this->Data[buttonID].ButtonState == BDButtonState::REPEAT) && (currentMillis - this->Data[buttonID].ButtonTime  > this->Settings[buttonID].ButtonRepeatIntervalTime))
				{
					// Button is pressed and hold: repeat button
					this->Data[buttonID].ButtonTime = currentMillis;

					if (this->ButtonRepeatFunction != NULL)
						(*ButtonRepeatFunction)(this->Parent, buttonID);

					return BDButtonState::REPEAT;
				}
			}
			else
			{
				if (this->Data[buttonID].ButtonState == BDButtonState::PRESSED)
				{
					// Button was released during debounce. Debounce the release
					this->Data[buttonID].ButtonState = BDButtonState::RELEASED;
					this->Data[buttonID].ButtonTime  = currentMillis;

					return BDButtonState::IDLE_UNCHANGED;
				}
				else if (this->Data[buttonID].ButtonState == BDButtonState::HOLD)
				{
					// Button is now released. Debounce it
					this->Data[buttonID].ButtonState = BDButtonState::RELEASED;
					this->Data[buttonID].ButtonTime  = currentMillis;

					if (this->ButtonPressDoneFunction != NULL)
						(*ButtonPressDoneFunction)(this->Parent, buttonID);

					return BDButtonState::IDLE_UNCHANGED;
				}
				else if (this->Data[buttonID].ButtonState == BDButtonState::REPEAT)
				{
					// Button is now released. Debounce it
					this->Data[buttonID].ButtonState = BDButtonState::RELEASED;
					this->Data[buttonID].ButtonTime  = currentMillis;

					if (this->ButtonRepeatDoneFunction != NULL)
						(*ButtonRepeatDoneFunction)(this->Parent, buttonID);
					
					return BDButtonState::IDLE_UNCHANGED;
				}
				else if ((this->Data[buttonID].ButtonState == BDButtonState::RELEASED) && (currentMillis - this->Data[buttonID].ButtonTime  > this->Settings[buttonID].ButtonDebounceReleaseTime))
				{
					// Button released and debounced
					this->Data[buttonID].ButtonState = BDButtonState::IDLE_UNCHANGED;
					
					if (!this->Data[buttonID].ButtonPressed)
						return BDButtonState::IDLE_UNCHANGED;

					this->Data[buttonID].ButtonPressed = false;

					if (this->ButtonReleasedFunction != NULL)
						(*ButtonReleasedFunction)(this->Parent, buttonID);

					return BDButtonState::RELEASED;
				}
			}

			return BDButtonState::IDLE_UNCHANGED;
		}

		/// Set the Debounce Press Time of all buttons
		/// 
		/// @param interval Time interval in ms
		void SetDebouncePressTime(uint8_t interval)
		{
			for (uint8_t buttonID = 0; buttonID < N; buttonID++)
			{
				this->Settings[buttonID].ButtonDebouncePressTime = interval;
			}
		}

		/// Set the Hold Time of all buttons
		/// 
		/// @param interval Time interval in ms
		void SetHoldTime(uint16_t interval)
		{
			for (uint8_t buttonID = 0; buttonID < N; buttonID++)
			{
				this->Settings[buttonID].ButtonHoldTime = interval;
			}
		}

		/// Set the Repeat Interval Time of all buttons
		/// 
		/// @param interval Time interval in ms
		void SetRepeatIntervalTime(uint16_t interval)
		{
			for (uint8_t buttonID = 0; buttonID < N; buttonID++)
			{
				this->Settings[buttonID].ButtonRepeatIntervalTime = interval;
			}
		}

		/// Set the Debounce Release Time of all buttons
		/// 
		/// @param interval 
		void SetDebounceReleaseTime(uint8_t interval)
		{
			for (uint8_t buttonID = 0; buttonID < N; buttonID++)
			{
				this->Settings[buttonID].ButtonDebounceReleaseTime = interval;
			}
		}

		// Callbacks
		void OnButtonPress(void (*func)(void *parent, uint8_t buttonID))		{ this->ButtonPressFunction = func;		 }
		void OnButtonHold(void (*func)(void *parent, uint8_t buttonID))			{ this->ButtonHoldFunction = func;		 }
		void OnButtonRepeat(void (*func)(void *parent, uint8_t buttonID))		{ this->ButtonRepeatFunction = func;	 }
		void OnButtonPressDone(void (*func)(void *parent, uint8_t buttonID))	{ this->ButtonPressDoneFunction = func;	 }
		void OnButtonRepeatDone(void (*func)(void *parent, uint8_t buttonID))	{ this->ButtonRepeatDoneFunction = func; }
		void OnButtonReleased(void (*func)(void *parent, uint8_t buttonID))		{ this->ButtonReleasedFunction = func;	 }

		/// Current button pressed state
		/// 
		/// @param buttonID ID of the button
		/// @return true Button is pressed
		/// @return false Button is not pressed
		bool IsButtonPressed(uint8_t buttonID)
		{
			return this->Data[buttonID].ButtonPressed;
		}

		/// Current button released state
		/// 
		/// @param buttonID ID of the button
		/// @return true Button is pressed
		/// @return false Button is not pressed
		bool IsButtonReleased(uint8_t buttonID)
		{
			return !this->Data[buttonID].ButtonPressed;
		}

	private:

		void (*ButtonPressFunction)(void *parent, uint8_t buttonID);
		void (*ButtonHoldFunction)(void *parent, uint8_t buttonID);
		void (*ButtonRepeatFunction)(void *parent, uint8_t buttonID);
		void (*ButtonPressDoneFunction)(void *parent, uint8_t buttonID);
		void (*ButtonRepeatDoneFunction)(void *parent, uint8_t buttonID);
		void (*ButtonReleasedFunction)(void *parent, uint8_t buttonID);

};
