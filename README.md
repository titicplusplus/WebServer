# WebServer

A web server for http and https requests

## Installation

```
cmake .
make
sudo make install
```

## How to use ?

To use, you need to inherit the following classes.


### For a http web server;
```cpp
// webserver.hpp
#pragma once

#include <WebServerHttp.hpp>

class WebServer : public WebServerHttp {
	public:
		virtual std::string getContent(std::string &url, std::string &type, char *buffer, int bufferSize);
};
```

### For a https web server;
```cpp
// webserver.hpp
#pragma once

#include <WebServerHttps.hpp>

class WebServer : public WebServerHttps {
	public:
		virtual std::string getContent(std::string &url, std::string &type, char *buffer, int bufferSize);
};
```

### And cpp files

```
#include "webserver.hpp"
#include <fstream>
#include <curl/curl.h>

std::string WebServer::getContent(std::string &url, std::string &type, char *buffer, int bufferSize) {
	std::string content;

	if (url == "/") {
		content = open_file("index.html");
	} else {
		content = open_file(url.substr(1));
	} else {
		content = "!:"; // For error 404
	}

	return content;
}
```

And the main.cpp to launch the web server:

```cpp
#include "webserver.hpp"
#include <csignal>
#include <fstream>

WebServer server;

void end_signal(int signal) {
	std::cout << "signal: " << signal << std::endl;
	server.stop(signal);
}


int main() {
	std::signal(SIGINT, end_signal);
	std::signal(SIGTERM, end_signal);


	// For an http web server
	server.config_server(8080);
	// For an https web server
	server.config_server(8080, "certs/testCA.crt", "certs/testCA.key");
		
	server.start();

	return 0;
}
```
