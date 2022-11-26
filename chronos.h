#ifndef CHRONOS_H_
#define CHRONOS_H_

/* define the maximum number of chronometers that could run simultaneously */
#define MAX_CHRONOS	64

#define NO_MORE_SPACE MAX_CHRONOS
#define NOT_USE -1

// #define DEBUG


#ifdef DEBUG
#	define DBG_PRINTLN(a,b)	{Serial.print(a);Serial.println(b);}
#else
#	define DBG_PRINTLN(a,b)	{}
#endif

#ifdef __cplusplus

#include <functional>
using callback_function_t = std::function<void(void)>;

typedef struct  {
	// absolute number of millis from the start call
  uint32_t startTime;
	// relative time to wait before calling callback, set by user
  uint32_t userDelay;
	// absolute time to wait before calling callback
  uint32_t delay;
	// flag set when activate
	bool run;
	// set once the delay is elapsed and callback has been called
	bool isElapsed;
	// function to call when the delay is elapsed
  callback_function_t callback;
} delayCallbackObj_t;

class Chronos {
	public:
		Chronos();
		/**
		 * start chronometer. If reset is set
		 * then force start time to the current time
		 * even if it already runs
		 * */
		void start(bool reset=false);

		/**
		 * return elapsed time from start
		 * */
		uint32_t getElapsedTime();

		/**
		 * stop timer. time is reset. callback is cleared
		 * */
		void stop();

		/**
		 * pause timer. time is suspended. callback is still fired
		 * */
		void pause();

		/**
		 * reset time value
		 * */
		void reset();

		/**
		 * attach callback which be called once the delay will be elapsed
		 * */
		void attachInterrupt(uint32_t delay, callback_function_t callback);

		/**
		 * return true if timer is running
		 * false otherwise
		 * */
		bool isRunning();
	
	private:
		/* current time when start is called */ 
		uint32_t startTime = 0;

		/* cumulated elapsed time */
		uint32_t elapsedTime = 0;

		/* flag that indicates if chronometer is running (tru) or not (false) */
		bool run = false;

		int8_t index = NOT_USE;

		uint32_t tmpDelay = 0;
};

#endif /* __cplusplus */

#endif /* CHRONOS_H_ */