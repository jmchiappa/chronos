#include "Arduino.h"
#include "clock.h"
#include "chronos.h"

#ifdef DEBUG
# include "core_debug.h"
# define COREDBG(a) {core_debug(a);}
#else
# define COREDBG(a)
#endif
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
		delta = 0xFFFFFFFFUL - previousTime + time;
	} else {
		delta = time - previousTime;
	}
	return delta;
}

void HAL_SYSTICK_Callback(void)
{
	if(activeChronos>0) {
		for(uint8_t i=0; i<MAX_CHRONOS;i++) {
			// only check time if callback has been attached, otherwise use as free run timer
			if(delayCallback_Handle[i].run && delayCallback_Handle[i].callback)
			{
				if(getDeltaTime(delayCallback_Handle[i].startTime) > delayCallback_Handle[i].userDelay) {
					delayCallback_Handle[i].startTime = getCurrentMillis(); // prepare next tme slot
					delayCallback_Handle[i].isElapsed = true;
					delayCallback_Handle[i].callback();
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
}

void Chronos::start(bool reset) {
	//this chrono should not be used
	if( index == NO_MORE_SPACE)
		return;
	// if the chrono already runs, do nothing
	if( run==false ) {
		startTime = getCurrentMillis();
		// if no index has been already allocated, assign a slot
		if( index == NOT_USE ) {
			index= getIndex();
			// sorry, no more chrono available
			if( index == NO_MORE_SPACE)
				return;
		}
		// this chrono was stopped before, so add it to active chrono
		if(delayCallback_Handle[index].run == false) {
			activeChronos++;
		}
		// initialize start up time
		delayCallback_Handle[index].startTime = startTime-elapsedTime;
		// activate the chrono

		delayCallback_Handle[index].run = true;
		run = true;
	}
	DBG_PRINTLN("Chronos actifs: ",activeChronos)
	DBG_PRINTLN("run: ",run)
	DBG_PRINTLN("delayCallback_Handle[index].run: ",delayCallback_Handle[index].run)
	DBG_PRINTLN("startTime: ",startTime)
	DBG_PRINTLN("delayCallback_Handle[index].startTime: ",delayCallback_Handle[index].startTime)
	DBG_PRINTLN("delayCallback_Handle[index].userDelay: ",delayCallback_Handle[index].userDelay)
}

void Chronos::pause() {
	if(!run) return;
	elapsedTime += getDeltaTime(startTime);
	if(index != NOT_USE) {
		delayCallback_Handle[index].run = false;
		delayCallback_Handle[index].isElapsed = false;
	}
	run = false;
}

void Chronos::stop() {
	elapsedTime = 0;
	if(index != NOT_USE && delayCallback_Handle[index].startTime>0) {
		delayCallback_Handle[index].run = false;
		delayCallback_Handle[index].isElapsed = false;
		delayCallback_Handle[index].userDelay = 0;
		delayCallback_Handle[index].startTime = 0;
		activeChronos--;		
	}
	run = false;
}

void Chronos::reset() {
	startTime = getCurrentMillis();
	elapsedTime = 0;
}

uint32_t Chronos::getElapsedTime() {
	return run ? getDeltaTime(startTime) + elapsedTime : elapsedTime;
}

void Chronos::attachInterrupt(int32_t delay, callback_function_t callback) {
	// nothign to do if there's no more space available
	DBG_PRINTLN("delai: ",delay)
	DBG_PRINTLN("delai: ",delay)
	DBG_PRINTLN("index: ",index)
	if(index == NO_MORE_SPACE)
		return;
	if(delay<0) delay=0;
	
	if(index==NOT_USE) {
		// reserve a slot if available
		index = getIndex();
		if(index==NO_MORE_SPACE) {
			return;
		}
		DBG_PRINTLN("nouvel index: ",index)
		delayCallback_Handle[index].run = false;
	}
	delayCallback_Handle[index].isElapsed = false;
	delayCallback_Handle[index].userDelay = delay;
	DBG_PRINTLN("index: ",index)
	DBG_PRINTLN("delai enregistre: ",delayCallback_Handle[index].userDelay)
	if(run) {
		delayCallback_Handle[index].run = true;
	}
	DBG_PRINTLN("run : ",delayCallback_Handle[index].run)
	delayCallback_Handle[index].callback = callback;
}

bool Chronos::isRunning(void) {
	return run;
}
