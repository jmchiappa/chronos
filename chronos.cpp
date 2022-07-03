#include "Arduino.h"
#include "clock.h"
#include "chronos.h"

static delayCallbackObj_t delayCallback_Handle[MAX_CHRONOS] = {NULL};

static uint8_t indexChronosCallback=0;

static uint8_t activeChronos = 0;
/**
 * user implementation called every 1 ms
 * */
void HAL_SYSTICK_Callback(void)
{
	if(activeChronos>0) {
		for(uint8_t i=0; i<MAX_CHRONOS;i++) {
			if(delayCallback_Handle[i].run) {
				if(delayCallback_Handle[i].delay>0) {
					if(getCurrentMillis()>delayCallback_Handle[i].delay) {
						activeChronos--;
						delayCallback_Handle[i].delay = 0; // reset ? maybe periodic usage ?
						delayCallback_Handle[i].isElapsed = true;
						delayCallback_Handle[i].run = false;
						delayCallback_Handle[i].callback();
					}
				}
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
	tmpDelay = 0;
}

void Chronos::start(bool reset) {
	if( (reset) || (run==false) ) {
		startTime = millis();
		if(reset) {
			elapsedTime = 0;
		}
		if(tmpDelay>0) {
			delayCallback_Handle[index].delay = startTime+tmpDelay+elapsedTime;
			delayCallback_Handle[index].run = true;
		}
	}
	run = true;
}

void Chronos::stop() {
	elapsedTime += millis() - startTime;
	if(index != NOT_USE) {
		delayCallback_Handle[index].run = false;
		delayCallback_Handle[index].isElapsed = false;
	}
	run = false;
}

void Chronos::reset() {
	startTime = millis();
	elapsedTime = 0;
}

uint32_t Chronos::getElapsedTime() {
	return run ? millis()-startTime+elapsedTime : elapsedTime;
}

void Chronos::attachInterrupt(uint32_t delay, callback_function_t callback) {
	// nothign to do if there's no more space available
	if(index == NO_MORE_SPACE)
		return;

	if(delay>0){
		if(index==NOT_USE) {
			// reserve a slot
			index = getIndex();
			if(index==NO_MORE_SPACE) {
				return;
			}
			delayCallback_Handle[index].isElapsed = true;
			delayCallback_Handle[index].run = false;
		}
		if(delayCallback_Handle[index].isElapsed) {
			delayCallback_Handle[index].isElapsed = false;
			tmpDelay = delay;
			if(run) {
				delayCallback_Handle[index].delay = millis()+tmpDelay;
				delayCallback_Handle[index].run = true;
			}
			delayCallback_Handle[index].callback = callback;
			activeChronos++;
		}
	}
}

bool Chronos::isRunning(void) {
	return run;
}
