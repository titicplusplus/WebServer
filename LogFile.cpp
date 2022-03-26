#include "LogFile.hpp"
#include <sstream>
#include <ctime>

//std::mutex LogFile::m;

std::string numberToStringFormat(int number) {
	if (number < 10) {
		return "0" + std::to_string(number);
	}

	return std::to_string(number);
}

LogFile::LogFile() {}

LogFile::LogFile(std::string file) {
	openFile(file);
}

void LogFile::openFile(std::string file) {
	m_file = file;

	if (m_file == "") {
		std::cerr << "ERROR couldn't open the file" << std::endl;
	}

	std::ofstream flux {m_file};
	if (!flux) {
		m_file = "";
		std::cerr << "ERROR couldn't open the file" << std::endl;
	} 

	flux.close();
}

void LogFile::setLog(const std::string_view ip, std::string_view url) {
	if (m_file != "") {
		//tasks.emplace_back(std::async([text] {
		
		//std::lock_guard<std::mutex> l(m);

		time_t now = time(0);
		std::tm ltm;

		localtime_r(&now, &ltm);

		std::ofstream flux {m_file, std::ios_base::app};

		if (flux)
			flux << "[" << numberToStringFormat(ltm.tm_hour) << ":"
 			<< numberToStringFormat(ltm.tm_min) << ":" << numberToStringFormat(ltm.tm_sec) << " - "
 			<< numberToStringFormat(ltm.tm_mday) << "/" << numberToStringFormat(ltm.tm_mon)<< "] " << ip 
			<< ':' << url << "\n";
		else {
			std::cerr << "Error coundn't not open the file" << std::endl;
		}



		flux.close();

		//}));
	} else {
		std::cerr << "ERROR file not open" << std::endl;
	}
}

LogFile::~LogFile() {
	//m_file.close();
}
