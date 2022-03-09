#pragma once

#include "WebServerHttp.hpp"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct SslDeleter {
    void operator()(SSL *_p)
    {
        //SSL_shutdown(_p);
        SSL_free(_p);
    }

    void operator()(SSL_CTX *_p)
    {
        SSL_CTX_free(_p);
    }
};

class WebServerHttps : public WebServerHttp {
	public:
		/// @brief Construct WebServer Object (init polling_js and error 404)
		WebServerHttps();
		
		/// Class Destructor
		virtual ~WebServerHttps();

		/// @brief Setup TCP connection
		/// @pre port need to be a integer
		/// @param port The port for the socket
		/// @param cert_file the certificate file
		/// @parma key_file the key file
		/// @return void
		virtual void config_server(int port, std::string cert_file, std::string key_file);

		/// @brief Setup the ssl files
		/// @param cert_file the certificate file
		/// @parma key_file the key file
		/// @return int
		int load_certificate(std::string certfile, std::string keyfile);

		/// @brief a classic http request answer
		/// @param the port of user
		/// @return void
		virtual void new_http_request(int port);

		/// @brief get classic answer
		/// @param url the url
		/// @param type the request type (get, post, ..)
		/// @param buffer the request
		/// @param bufferSize the size of buffer
		/// @param content the content to change
		/// @param setContentType the content type of the answer
		/// @return void
		virtual std::string getContent(std::string &url, std::string &type, char *buffer, int bufferSize);

		/// @brief init ssl
		void init_ssl();

		/// @brief destroy ssl 
		void destroy_ssl();

		/// @brief To stop the TCP Main loop when user wants
		/// @return void
		virtual void stop(int signal);
	private:
		std::unique_ptr<SSL_CTX, SslDeleter> ctx;
		std::string redirectUrl;
};
