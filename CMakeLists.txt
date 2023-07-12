cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(async_server)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set (BOOST_ROOT /boost_1_82_0) # Путь к библиотеке Boost

#set (CMAKE_CXX_STANDARD 19) 
set (Boost_USE_STATIC_LIBS ON)
  
# Подключаем необходимые модули
find_package (Boost REQUIRED) 
  
# Если библиотека найдена, то
if (Boost_FOUND)
    include_directories (${Boost_INCLUDE_DIR}) # подключаем заголовочные файлы
    add_executable(Client src/client.cpp)
    add_executable(Server src/server.cpp) 
    target_link_libraries (Client ${Boost_LIBRARIES}) # подключаем библиотеку
    target_link_libraries (Server ${Boost_LIBRARIES})
endif ()