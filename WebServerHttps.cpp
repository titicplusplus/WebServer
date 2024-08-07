#include "WebServerHttps.hpp"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <mutex>

std::mutex WebServerHttps::sslSecure;

WebServerHttps::WebServerHttps() : WebServerHttp() {
	redirectUrl = "<html><head><title>404 Redirection</title></head><body><h1>Please wait for https redirection</h1></body></html>";
	ctx = NULL;
}

void WebServerHttps::config_server(int port, std::string cert_file, std::string key_file, int nbr_thread, std::string file_name) {
	WebServerHttp::config_server(port, nbr_thread, file_name);

	/* SSL */
	init_ssl();

	ctx.reset(SSL_CTX_new(TLS_server_method()));// = std::unique_ptr<SSL_CTX>(SSL_CTX_new(TLS_server_method()));

	if (ctx.get() == NULL) {
		std::cerr << "SSL error" << std::endl;
		exit(0);
	}

	load_certificate(cert_file, key_file);
}

int WebServerHttps::load_certificate(std::string certfile, std::string keyfile) {
	/* set the local certificate from CertFile */
	if ( SSL_CTX_use_certificate_file(ctx.get(), certfile.c_str(), SSL_FILETYPE_PEM) <= 0 )
	{
		ERR_print_errors_fp(stderr);
		return -1;
	}

	/* set the private key from KeyFile (may be the same as CertFile) */
	if ( SSL_CTX_use_PrivateKey_file(ctx.get(), keyfile.c_str(), SSL_FILETYPE_PEM) <= 0 )
	{
		ERR_print_errors_fp(stderr);
		return -1;
	}
	/* verify private key */
	if ( !SSL_CTX_check_private_key(ctx.get()) )
	{
		std::cerr << "Private key does not match the public certificate" << std::endl;
		return -1;
	}

	return 0;
}

void WebServerHttps::new_http_request(int port, const std::string ipClient) {
	//SSL *cSSL = SSL_new(ctx.get());
	
	//std::unique_lock<std::mutex> sslLck {sslSecure};
	auto cSSL = std::unique_ptr<SSL, SslDeleter>(SSL_new(ctx.get()));
	//sslLck.unlock();

	SSL_set_fd(cSSL.get(), port);
	
	std::string content { "!:" };

	if (SSL_accept(cSSL.get()) == 0) {
		ERR_print_errors_fp(stderr);
	} else {
		char buffer[1024] = {0};
		//int valread = read( port, buffer, 1024);

		int valread = SSL_read( cSSL.get(), buffer, 1024);
		std::vector<std::string> value;

		if (valread < 0) {
			/**content = "Please, use https request";

			std::string rep = "HTTP/1.0 200 OK\r\nContent-Length: " +  
				std::to_string(content.size()) + "\r\n\r\n" + content;

			write(port, rep.c_str(), rep.size());**/
			return;
		} else {
			value = info_request( buffer, valread );

			if (value.size() > 1) {
				std::unique_lock<std::mutex> logLck { logFile };
				if (logF) {
					m_file.setLog(ipClient, value[1]);
				}
				logLck.unlock();

				auto params = get_params( value[1] );

				//////////////////////////////////
				
				content = getContent(value[1], value[0], buffer, valread, params);//"<h1>Hello, world!</h1>";

				//////////////////////////////////
			}

			std::string rep { "" };
			
			if (content == "!:") {
				rep = "HTTP/1.0 404 Not Found\r\nContent-Length: " +  
				std::to_string(error404.size()) + "\r\n\r\n" + error404;
			} else {
				rep = "HTTP/1.0 200 OK\r\nContent-Length: " +  
				std::to_string(content.size()) + "\r\n" + content_type(value[1]) + "\r\n" + content;
			}

			//std::unique_lock<std::mutex> sslLck {sslSecure};

			SSL_write(cSSL.get(), rep.c_str(), rep.size());

			//sslLck.unlock();
		}
	}
	//SSL_free(cSSL);

	return;
}

void WebServerHttps::init_ssl() {
	SSL_load_error_strings();
	SSL_library_init();
    	OpenSSL_add_all_algorithms();
}

void WebServerHttps::destroy_ssl() {
	ERR_free_strings();
	EVP_cleanup();
}

void WebServerHttps::stop(int signal) {
	WebServerHttp::stop(signal);

	destroy_ssl();
}

std::string WebServerHttps::getContent(std::string &url, std::string &type, char *buffer, int bufferSize,
		const Parameters &parameters) {
	return "Hello World!";
}


WebServerHttps::~WebServerHttps() {
}
