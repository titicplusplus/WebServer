#pragma once
#include <iostream>
//#include "PoolRequest.hpp"
#include <fstream>
#include <vector>
#include <future>
#include <string_view>
#include <string>

/// @brief the goal of this class to log information about connection
/// @author Thibaud VINCENT
class LogFile {
	public:
		/// @brief the default constructer
		LogFile();

		/// @brief a constructer who open a file
		/// @param file the path to open
		LogFile(std::string file);

		/// @brief to open a file for the log
		/// @param file the path to open
		/// @return void
		void openFile(std::string file);

		/// @brief to set the log
		/// @param text the text to log
		/// @return void
		void setLog(const std::string_view ip, std::string_view url);//std::string text);

		/// @brief the destructor
		~LogFile();

	private:
		//std::mutex m;
		std::string m_file;
		std::vector<std::future<void>> tasks;
};
