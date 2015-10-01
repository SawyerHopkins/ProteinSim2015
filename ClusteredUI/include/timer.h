#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <math.h>

namespace debugging
{
	/**
	 * @class timer
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file timer.h
	 * @brief Timer for diagnostics.
	 */
	class timer
	{
		private:

			//System time objects.
			std::chrono::system_clock::time_point start_time;
			std::chrono::system_clock::time_point stop_time;

		public:

			//Header Version.
			static const int version = 1;

			/**
			 * @brief Create a new timer.
			 */
			timer();
			~timer();

			/**
			 * @brief Start the timer.
			 */
			void start() { start_time = std::chrono::system_clock::now(); }
			/**
			 * @brief End the timer.
			 */
			void stop() { stop_time = std::chrono::system_clock::now(); }

			/**
			 * @brief The number of seconds elapsed between start() and stop()
			 * @return The difference between stop_time and start_time. 
			 */
			float getElapsedSeconds() 
			{
				std::chrono::system_clock::duration diff = stop_time-start_time;
				return (diff.count() / pow(10,9));
			}
	};
}

#endif // TIMER_H
