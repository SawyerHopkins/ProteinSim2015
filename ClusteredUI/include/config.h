#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

namespace configReader
{

	using namespace std;

	class config
	{

		private:

			map<string, string> options;
			bool suppressOutput;

		public:

			//Header Version.
			static const int version = 1;

			config(string fileName);
			~config();

			/**
			 * @brief Get a option from the configuration file.
			 * @param key The keyword to option the setting for.
			 * @param def The default value if the keyword is missing.
			 * @return Returns the associated value of the given key.
			 */
			template<typename T> T getParam(string key, T def);

			/**
			 * @brief Check that the key is in the configuration file.
			 * @param key The keyword to search.
			 * @return True if exists, false if missing.
			 */
			bool containsKey(string key) { return options.count(key); }

			/**
			 * @brief Show configuration output.
			 */
			void showOutput() { suppressOutput = false; }

			/**
			 * @brief Hide configuration output.
			 */
			void hideOutput() { suppressOutput = true; }
 
	};

}

#endif // CONFIG_H
