cmake_minimum_required(VERSION 3.23)
project(cuoiki C)

set(CMAKE_C_STANDARD 99)
include_directories(/usr/include/openssl/)
link_libraries(ssl)
link_libraries(crypto)
add_executable(cuoiki main.c smtp.c pop3.c connect.c base64.c)
