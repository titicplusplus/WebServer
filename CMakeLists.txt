cmake_minimum_required(VERSION 3.9)
project(TWebServer VERSION 1.1.1 DESCRIPTION "A web server class")

add_library(TWebServer SHARED
	WebServerHttp.cpp
	WebServerHttp.hpp
	WebServerHttps.cpp
	WebServerHttps.hpp
	LogFile.cpp
	LogFile.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

target_link_libraries(
	${PROJECT_NAME}
	ssl
	crypto
)

set(CMAKE_CXX_STANDARD 17)
target_compile_features(TWebServer PRIVATE cxx_std_20)

set(public_headers
	WebServerHttp.hpp
	WebServerHttps.hpp
	LogFile.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")

# For access to standard installation directory variables (CMAKE_INSTALL_xDIR).
include(GNUInstallDirs)

# Set library shared object and API header file to install.
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TWebServer/
)
