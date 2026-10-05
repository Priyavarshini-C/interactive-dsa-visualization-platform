# ==========================================================================
#  Interactive Algorithm Simulation and Visualization Platform
#  Data Structures and Algorithms (21CSC201J) - B.Tech CSE, SRMIST
#
#  make          build the program
#  make run      build and run it
#  make auto     build and run without step-by-step pauses
#  make clean    remove the build output
# ==========================================================================

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Iinclude
TARGET  = dsaviz
SOURCES = $(wildcard src/*.c)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

auto: $(TARGET)
	./$(TARGET) --auto

clean:
	rm -f $(TARGET)

.PHONY: run auto clean
