# devnetXButtonDebouncer
A simple Arduino Button Debouncer Library that provides easy button handling.

## Instantiation
```c++
#include <Arduino.h>
#include <devnetXButtonDebouncer.h>

devnetXButtonDebouncer<4> Debouncer;
```
Specify how many buttons should be handled by this instance inside the angle brackets.
This is a compile-time instantiation, variables are not allowed!

## Initialization
```c++
void Begin(void *parent = NULL);
```
Initializes the library. You are responsible for initialization of used GPIO ports (or port expander).
The initialization must be done **before** calling any other library functions.

*Note:* If you use this library inside a class, you can use the optional ```void *parent``` parameter to hold a reference to the instance that created the debouncer object.
This can be useful when your code gets more complex and your compiler does not support ```std::function``` like *avr-gcc*.

## Polling
```c++
BDButtonState_t Process(uint8_t buttonID, bool active);
```
The library does not support interrupts. Therefore it is **required** to poll each button inside your ```loop()```.
This method will also trigger callbacks, if you defined any. See examples.

The current GPIO (or port expander) input state must be provided to the library using the ```active``` parameter.
A simple ```digitalRead(GPIO_PIN)``` of the affected GPIO pin is sufficient.
*Returns*: Returns the button state of the processed button. See: [Button States](#button-states).

*Note*: If you use negative-logic (button switches to GND + pull-up resistor), you have to invert the return value, e.g.: ```!digitalRead(GPIO_PIN)```.


## Button States
Each time a button is processed by the ```Process()``` method, it will return a ```BDButtonState``` enum value.
If there was no new event detected on the button, it will return ```BDButtonState::IDLE_UNCHANGED```.

List of possible return values defined in ```BDButtonState``` enum that can be returned from the ```Process()``` method:
- ```BDButtonState::IDLE_UNCHANGED``` The button is not pressed or there is no new state to report.
- ```BDButtonState::PRESSED``` The button is pressed and debounced.
- ```BDButtonState::HOLD``` The button is pressed and hold.
- ```BDButtonState::REPEAT``` The button is pressed and hold. This is the repeat state of the button.
- ```BDButtonState::RELEASED``` The button is released and debounced.

## Callbacks
If you don't want to handle your buttons inside the main ```loop()```, you can define callbacks (and lambda expressions to define anonymous functions) inside your ```setup()```.
It is also possible to use polling and callbacks simultaneously.

List of supported callbacks:
- ```void OnButtonPress(void (*func)(void *parent, uint8_t buttonID));```
- ```void OnButtonHold(void (*func)(void *parent, uint8_t buttonID));```
- ```void OnButtonRepeat(void (*func)(void *parent, uint8_t buttonID));```
- ```void OnButtonPressDone(void (*func)(void *parent, uint8_t buttonID));```
- ```void OnButtonRepeatDone(void (*func)(void *parent, uint8_t buttonID));```
- ```void OnButtonReleased(void (*func)(void *parent, uint8_t buttonID));```

*Note:* You must call ```Debouncer.Process(...);``` inside your ```loop()``` anyway!

### Callback Example
```c++
Debouncer.OnButtonPress([](void *, uint8_t buttonID) {
	Serial.print("OnButtonPress - Button: ");
	Serial.println(buttonID);
});
```

## Settings
Depending on used buttons (hardware), it could be necessary to tweak the debounce times.
This can be done by calling the folowing methods to apply the settings to all handled buttons at once:
- ```void SetDebouncePressTime(uint8_t interval);``` Set the Debounce Press Time of all buttons in ms.
- ```void SetHoldTime(uint16_t interval);``` Set the Hold Time of all buttons in ms. This is the time after that the button repeat will take over.
- ```void SetRepeatIntervalTime(uint16_t interval);``` Set the Repeat Interval Time of all buttons in ms. This is the time that will be passed between button repeats.
- ```void SetDebounceReleaseTime(uint8_t interval); ``` Set the Debounce Release Time of all buttons.

You can also change the Hold Time and the Repeat interval.
If required, all intervals for each button can be set independently.

### Settings Example
Set the Debounce Press Time of your first button (button ID: 0) to 100ms:
```Debouncer.Settings[0].ButtonDebouncePressTime = 100;```

Set the Repeat Interval Time of your second button (button ID: 1) to 50ms:
```Debouncer.Settings[1].ButtonRepeatIntervalTime = 50;```

### Loading/Saving Button settings
You can use ```Debouncer.Settings``` to load your button settings from, or store to an EEPROM.
New settings value will take effect immediately.

---
Have fun! :)
