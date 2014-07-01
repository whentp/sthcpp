CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(bible) # project name

AUX_SOURCE_DIRECTORY(
	src
	SRC_LIST) # add src to SRC_LIST

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    -Wall
    -O2
)

ADD_EXECUTABLE(bible ${SRC_LIST})