cmake_minimum_required(VERSION 3.10)
project(ResourceManager)

set(CMAKE_CXX_STANDARD 17)




include(FetchContent)
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
)
FetchContent_MakeAvailable(json)


find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(YourProject PRIVATE nlohmann_json::nlohmann_json)
set(SOURCES
    src/main.cpp
    src/Dispatcher.cpp
    src/Game.cpp
    src/Process.cpp
    src/Resource.cpp
    src/ResourceGenerator.cpp
    src/Utils.cpp
)

set(HEADERS
    include/Dispatcher.h
    include/Game.h
    include/Process.h
    include/Resource.h
    include/ResourceGenerator.h
    include/Utils.h
    include/Data.h
)

add_executable(ResourceManager ${SOURCES} ${HEADERS})

target_link_libraries(ResourceManager PRIVATE nlohmann_json::nlohmann_json)

if(MSVC)
    target_compile_options(ResourceManager PRIVATE /W4 /WX)
else()
    target_compile_options(ResourceManager PRIVATE -Wall -Wextra -pedantic -Werror)
endif()