cmake_minimum_required(VERSION 3.16)

project(Space_Shooter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)

set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(SFML_BUILD_DOCS OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML)

add_executable(Space_Shooter
    main.cpp
    Asteroid.cpp Asteroid.h
    Bullet.cpp Bullet.h
    Menu.cpp Menu.h
    Player.cpp Player.h
    ScoreManager.cpp ScoreManager.h
    TimeBonus.cpp TimeBonus.h
    Explosion.cpp Explosion.h
)

target_link_libraries(Space_Shooter PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/config.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/font.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/bullet.png
    ${CMAKE_CURRENT_SOURCE_DIR}/asteroid.png
    ${CMAKE_CURRENT_SOURCE_DIR}/background.png
    ${CMAKE_CURRENT_SOURCE_DIR}/ship.png
    ${CMAKE_CURRENT_SOURCE_DIR}/ship_thrust.png
    ${CMAKE_CURRENT_SOURCE_DIR}/time_bonus.png
    ${CMAKE_CURRENT_SOURCE_DIR}/explosion.png
    ${CMAKE_CURRENT_SOURCE_DIR}/explosion.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/shoot.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/crash.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/thrust.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/reload.wav
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
if(TARGET sfml-audio)
    add_custom_command(TARGET Space_Shooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll"
        "$<TARGET_FILE_DIR:Space_Shooter>"
    )
endif()

if(WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(Space_Shooter PROPERTIES WIN32_EXECUTABLE TRUE)
endif()