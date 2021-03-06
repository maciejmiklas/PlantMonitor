#include "Sos.h"

static uint8_t state;
static uint32_t switchMs = 0;
static uint8_t lastPinVal = -1;
static boolean sosOn = false;

void sos_setup() {
	pinMode(SOS_PIN, OUTPUT);
	sos_off();
}

void sos_on() {
	sosOn = true;
}
void sos_off() {
	if (sosOn) {
		state = 1;
		digitalWrite(SOS_PIN, LOW);
		switchMs = util_millis();
	}
	sosOn = false;
}

static void doSwitch(uint16_t duration) {
	if (util_millis() - switchMs >= duration) {
		state++;
		switchMs = util_millis();
	}
}
// States:
// 1 - LED ON for long time
// 2 - LED OFF
// 3 - LED ON for long time
// 4 - LED OFF
// 5 - LED ON for long time
// 6 - LED OFF
// 7 - LED ON for short time
// 8 - LED OFF
// 9 - LED ON for short time
// 10 - LED OFF
// 11 - LED ON for short time
// 12 - LED OFF
void sos_cycle() {
	if (!sosOn) {
		return;
	}
	uint8_t pinVal = HIGH;

	switch (state) {

	// led off long duration
	case 2:
	case 4:
		doSwitch(SOS_OFF_LONG_DURATION);
		pinVal = LOW;
		break;

		// pause between 3x long and 3x short
	case 6:
		doSwitch(SOS_PAUSE_MIDLE);
		pinVal = LOW;
		break;

		// led off short duration
	case 8:
	case 10:
		doSwitch(SOS_OFF_SHORT_DURATION);
		pinVal = LOW;
		break;

		// led on for long time
	case 1:
	case 3:
	case 5:
		doSwitch(SOS_ON_LONG_DURATION);
		pinVal = HIGH;
		break;

		// led on for short time
	case 7:
	case 9:
	case 11:
		doSwitch(SOS_ON_SHORT_DURATION);
		pinVal = HIGH;
		break;

		// pause after whole SOS
	case 12:
		doSwitch(SOS_PAUSE_DONE);
		pinVal = LOW;
		break;

	default:
		state = 1;
		break;
	}

	if (lastPinVal != pinVal) {
		digitalWrite(SOS_PIN, pinVal);
	}
	lastPinVal = pinVal;
}

