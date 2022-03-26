#include "WebServerHttp.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>
#include <future>
#include <thread>
#include <fstream>

#ifndef __has_include
	static_assert(false, "__has_include not supported");
#else
#  if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
#  elif __has_include(<filesystem>)
#    include <filesystem>
	namespace fs = std::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp> 
	namespace fs = boost::filesystem;
#  endif
#endif


std::unordered_map<std::string, std::unique_ptr<std::mutex>> WebServerHttp::fileRead;

WebServerHttp::WebServerHttp() {
	error404 = "<html><head><title>Error: 404</title></head><body><h1>Error: 404</h1></body></html>";
	logF = false;

}

void WebServerHttp::config_server(int port, int nbr_thread, std::string file_name) {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		std::cerr << "socket error" << std::endl;
	}

	int enable {1};

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		std::cerr << "Can't configure socket SO_REUSEADDRE" << std::endl;
	}

	std::clog << "Server listening on http://127.0.0.1:" << port << std::endl;

	/**if (nbr_thread != -1)
		m_manageRequest.setThread(nbr_thread);**/

	if (file_name != "") {
		logF = true;
		m_file.openFile(file_name);
	}

	fileRead.insert(std::make_pair("locker", std::make_unique<std::mutex>()));
}

void WebServerHttp::start() {
	int new_socket;
	is_alive = true;

	int res = bind(server_fd, (sockaddr*)&addr, sizeof(addr));

	if (res < 0) {
		std::cerr << "bind failed: " << res << std::endl;
		return;
	}


	res = listen(server_fd, -1);
	int addrlen = sizeof(addr);

	if (res < 0) {
		std::cerr << "listen error" << std::endl;
		return;
	}

	//thread_ptr = std::unique_ptr<std::thread>(new std::thread(&WebSocket::wait_client, &webSocket));

	std::vector<std::future<void>> tasks;
	while (is_alive && (new_socket = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen))) {
		if (new_socket > 0) {
			struct in_addr ipAddr = addr.sin_addr;
			char str[INET_ADDRSTRLEN];
		
			//std::string ipClient = inet_ntoa(addr.sin_addr);
			const std::string ipClient = inet_ntoa(addr.sin_addr);
			std::async([this, new_socket, ipClient] {
				try {
					new_http_request(new_socket, ipClient);
				} catch (const std::exception& e) {
					std::cerr << "Error during answer process: " << e.what() << std::endl;
				}

				/**std::this_thread::sleep_for(1000ms);


				std::string rep = "HTTP/1.0 200 OK\r\nContent-Length: 0\r\n\r\n";
				send( new_socket, rep.c_str(), rep.size(), 0 );**/
			});
		}
	}
}


void WebServerHttp::new_http_request(int port, const std::string ipClient) {
	char buffer[1024] = {0};
	int valread = read(port, buffer, 1024);

	if (valread == -1)
		return;

	std::vector<std::string> value = info_request( buffer, valread );
	auto params = get_params( value[1] );

	//if (value[1] != "/new")
		//std::cout << buffer << std::endl; 

	/**if (value[0] == "POST") {
		//std::cout << get_content(buffer, valread) << std::endl;
	}**/

	std::string content { "" };

	//////////////////////////////////
	
	if (value.size() > 0) {
		std::unique_lock<std::mutex> logLck { logFile };
		if (logF) {
			m_file.setLog(ipClient, value[1]);
		}
		logLck.unlock();

		content = getContent(value[1], value[0], buffer, valread );//"<h1>Hello, world!</h1>";

	}

	//////////////////////////////////

	if (content == "!:") {
		content = "No file found!";
		std::string rep = "HTTP/1.0 404 Not Found\r\nContent-Length:" + 
			std::to_string(error404.size()) + "\r\n\r\n" + error404;

		send(port, rep.c_str(), rep.size(), 0);
	} else {
		std::string rep = "HTTP/1.0 200 OK\r\nContent-Length: " +  
			std::to_string(content.size()) + "\r\n" + content_type(value[1]) + "\r\n" + content;

		send(port, rep.c_str(), rep.size(), 0);
	}
}



std::string WebServerHttp::open_file(std::string filename) {
	if (fs::exists(filename)) {

		if (fileRead.find(filename) == fileRead.end()) {
			std::unique_lock<std::mutex> fileReadLck { *fileRead["locker"].get() };
			//fileRead[filename] = std::make_unique<std::mutex>();
			fileRead.insert(std::make_pair(filename, std::make_unique<std::mutex>()));
		}

		std::lock_guard<std::mutex> fileLck { *fileRead[filename].get() };

		std::ifstream file { filename };

		if (file) {

			std::string html { (std::istreambuf_iterator<char>(file) ),
					(std::istreambuf_iterator<char>()) };
			file.close();

			return html;
		} else {
			return "!:";
		}
	} else {
		return "!:";
	}
}

std::string WebServerHttp::content_type(std::string &filename) {
	std::string ext { "" };
	
	if (filename.size() < 2) {
		return "content-type: text/html; charset=utf-8\r\n";
	}

	std::size_t i { filename.size() -2};

	for (; i >= 0; i--) {
		if (filename[i] == '.') {
			break;	
		} else if (filename[i] == '/') {
			return "content-type: text/plain; charset=utf-8\r\n";
		}
	}

	ext = filename.substr(i+1);

	if (ext == "html") {
		return "content-type: text/html; charset=utf-8\r\n";
	} else if (ext == "js") {
		return "content-type: text/javascript; charset=utf-8\r\n";
	} else if (ext == "css") {
		return "content-type: text/css; charset=utf-8\r\n";
	} else if (ext == "png") {
		return "content-type image/png;\r\n";
	}

	return "content-type: text/plain; charset=utf-8\r\n";
}

std::vector<std::string> WebServerHttp::info_request(char *buffer, size_t size) {
	size_t i = 0;
	std::string word = "";
	std::vector<std::string> response;
	int detecd_attack = 0;

	while (size > i+2 && buffer[i] != '\r' && buffer[i+1] != '\n') {

		if (buffer[i] == ' ') {
			response.push_back(word);
			word = "";
		}
		else {
			if (buffer[i] == '.') {
				if ((detecd_attack++) < 2) {
					word += buffer[i];
				}
			} else {
				word += buffer[i];
				detecd_attack = 0;
			}
		}

		i++;
	}

	if (word != "") {
		response.push_back(word);
	}

	return response;
}

std::string WebServerHttp::get_content(char *buffer, size_t size) {
	
	for (size_t i = 0; i < size-4; i ++) {
		if (buffer[i] == '\r' && buffer[i+1] == '\n' && buffer[i+2] == '\r' && buffer[i+3] == '\n') {
			std::string content { buffer };
			content = content.substr(i+4);
			return content;
		}
	}

	return "";
}


std::unordered_map<std::string, std::string> WebServerHttp::get_params(std::string &url) {

	std::cout << "Url " << url << std::endl;
	auto it = url.find("?");

	if (it == std::string::npos) {
		std::cout << "End" << std::endl;
		return {};
	}

	std::unordered_map<std::string, std::string> params;

	std::string para { "" };
	std::string value { "" };
	int status = 0;

	auto it_r = it;
	++it;
	for (; it != url.size(); ++it) {
		char &v = url[it];
		if (status == 0) {
			if (v != '=') {
				para += v;
			}
			else {
				status = 1;
			}
		}
		else if (status == 1) {
			if (v != '&') {
				value += v;
			}
			else {
				status = 0;
				params[para] = value;

				para = "";
				value = "";
			}
		}
	}
	if (para != "") {
		params[para] = value;
	}

	url = url.substr(0, it_r);
	return params;
}

std::string WebServerHttp::getContent(std::string &url, std::string &type, char *buffer, int bufferSize) {
	return "Hello World!";
}

void WebServerHttp::stop(int /*signal*/) {
	//webSocket.stop();

	shutdown(server_fd, SHUT_RD);
	is_alive = false;

	std::cerr << "end server" << std::endl;
	//thread_ptr->join();
}

WebServerHttp::~WebServerHttp() {}

