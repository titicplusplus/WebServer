#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <sys/socket.h>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "LogFile.hpp"

#include <netinet/in.h>
#include <vector>
#include <thread>

#include <unordered_map>

/// @brief Class to manage the Http connection with TCP socket
/// @author Thibaud VINCENT
class WebServerHttp {
	public:

		/// @brief Construct WebServer Object (init polling_js and error 404)
		WebServerHttp();
		
		/// Class Destructor
		virtual ~WebServerHttp();

		/// @brief Setup TCP connection
		/// @pre port need to be a integer
		/// @param port The port for the socket
		/// @param nbr_thread The number of thread
		/// @param file_name the file_name for the log
		/// @return void
		virtual void config_server(int port, int nbr_thread = -1, std::string file_name = "");

		/// @brief The TCP Main loop, accept connection
		/// @return void
		void start();

		/// @brief To stop the TCP Main loop when user wants
		/// @return void
		virtual void stop(int signal);

		/// @brief Method that anwsers the web browser or http protocol when a new TCP connection happens
		/// @param port the socket of the client
		/// @param ipClient the ip of the client
		/// @return void
		virtual void new_http_request(int port, const std::string ipClient);

		/// @brief Method to get the main information with the request sush as GET/POST/PUT, pathfile ( "/" ), and HTML version
		/// @param buffer The text request
		/// @param size The max-size of the buffer
		/// @return std::vector<std::string>
		std::vector<std::string> info_request(char *buffer, size_t size);

		/// @brief Get the content of the POST/PUT request
		/// @param buffer The text request
		/// @param size The max-size of the buffer
		/// 
		/// @return std::string
		std::string get_content(char *buffer, size_t size);
		

		/// @brief Get the paramaters of the URL
		/// 
		/// @param &url The url
		/// @return std::unordered_map<std::string>
		std::unordered_map<std::string, std::string> get_params(std::string &url);

		/// @brief To open or render with jinja a file
		/// @param filename  the path of the file
		/// @return std::string
		std::string open_file(std::string filename);
		std::string content_type(std::string &filenames);


		/// @brief get the classical answer
		/// @return std:string
		virtual std::string getContent(std::string &url, std::string &type, char *buffer, int bufferSize);

	protected:
		struct sockaddr_in addr;
		int server_fd;
		bool is_alive;

		std::string error404;
		//PoolRequest m_manageRequest;
		LogFile m_file;
		bool logF;

		static std::unordered_map<std::string, std::unique_ptr<std::mutex>> fileRead;
		//std::unordered_map<int, std::string> ipClient;
		std::mutex logFile;

};

#endif // WEBSERVER_HPP

