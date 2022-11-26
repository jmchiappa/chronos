#include "Arduino.h"
#include "clock.h"
#include "chronos.h"

static delayCallbackObj_t delayCallback_Handle[MAX_CHRONOS] = {NULL};

static uint8_t indexChronosCallback=0;

static uint8_t activeChronos = 0;
/**
 * user implementation called every 1 ms
 * */

/**
 * Manage the roll over of millis timer
 * */
uint32_t getDeltaTime(uint32_t previousTime) {
	uint32_t time = getCurrentMillis();
	uint32_t delta;
	if(time<previousTime) {
		delta = 0xFFFFFFFF - previousTime + time;
	} else {
		delta = time - previousTime;
	}
	return delta;
}

void HAL_SYSTICK_Callback(void)
{
	if(activeChronos>0) {
		for(uint8_t i=0; i<MAX_CHRONOS;i++) {
			if(delayCallback_Handle[i].run) {
				// if(delayCallback_Handle[i].delay>0) {
					if(getDeltaTime(delayCallback_Handle[i].startTime) > delayCallback_Handle[i].userDelay) {
						delayCallback_Handle[i].startTime = getCurrentMillis(); // prepare next tme slot
						delayCallback_Handle[i].isElapsed = true;
						delayCallback_Handle[i].callback();
					}
				// }
			}
		}
	}
}

static uint8_t getIndex(void) {
	uint8_t ret;
	if(indexChronosCallback<MAX_CHRONOS) {
		ret = indexChronosCallback;
		indexChronosCallback++;

	} else {
		ret = NO_MORE_SPACE;
	}
	return ret;
}

Chronos::Chronos() {
	startTime = 0;
	elapsedTime = 0;
	run = false;
	// tmpDelay = 0;
}

void Chronos::start(bool reset) {
	if( run==false ) {
		DBG_PRINTLN("run: ",run)
		startTime = getCurrentMillis();
		// elapsedTime = 0;
		// if(tmpDelay>0) {
		
		if(delayCallback_Handle[index].run == false) {
			activeChronos++;
		}

		delayCallback_Handle[index].startTime = startTime-elapsedTime;
		delayCallback_Handle[index].run = true;
		// }
		run = true;
	}
}

void Chronos::pause() {
	elapsedTime += getDeltaTime(startTime);
	if(index != NOT_USE) {
		delayCallback_Handle[index].run = false;
		delayCallback_Handle[index].isElapsed = false;
	}
	run = false;
}

void Chronos::stop() {
	elapsedTime = 0;
	if(index != NOT_USE) {
		delayCallback_Handle[index].run = false;
		delayCallback_Handle[index].isElapsed = false;
		// delayCallback_Handle[index].delay = 0;
		delayCallback_Handle[index].userDelay = 0;
		delayCallback_Handle[index].startTime = 0;
		// tmpDelay=0;
		activeChronos--;		
	}
	run = false;
}

void Chronos::reset() {
	pinMode(LED_BUILTIN,OUTPUT);
	startTime = getCurrentMillis();
	elapsedTime = 0;
}

uint32_t Chronos::getElapsedTime() {
	return run ? getCurrentMillis()-elapsedTime : elapsedTime;
}

void Chronos::attachInterrupt(uint32_t delay, callback_function_t callback) {
	// nothign to do if there's no more space available
	DBG_PRINTLN("delai: ",delay)
	DBG_PRINTLN("delai: ",delay)
	DBG_PRINTLN("index: ",index)
	if(index == NO_MORE_SPACE)
		return;
	
	digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));

	// if(delay>=0){
	if(index==NOT_USE) {
		// reserve a slot
		index = getIndex();
		if(index==NO_MORE_SPACE) {
			return;
		}
		DBG_PRINTLN("nouvel index: ",index)
		delayCallback_Handle[index].isElapsed = true;
		delayCallback_Handle[index].run = false;
	}
	if(delayCallback_Handle[index].isElapsed) {
		delayCallback_Handle[index].isElapsed = false;
		// tmpDelay = delay;
	}
	delayCallback_Handle[index].userDelay = delay;
	DBG_PRINTLN("nouvel index: ",index)
	DBG_PRINTLN("delai enregistre: ",delayCallback_Handle[index].userDelay)
	

	if(run) {
	// 	delayCallback_Handle[index].delay = tmpDelay;
		delayCallback_Handle[index].run = true;
	}


	delayCallback_Handle[index].callback = callback;
	// }
}

bool Chronos::isRunning(void) {
	return run;
}
