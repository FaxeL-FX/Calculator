



add_library(BMPWriter BMPWriter.cc BMPWriter.h)

list(APPEND ConsoleCalculator_SOURCES ConsoleCalculator.cpp)

add_executable(ConsoleCalculator ${ConsoleCalculator_SOURCES})

target_link_libraries(ConsoleCalculator BMPWriter)