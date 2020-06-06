# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#CFLAGS  = -g -Wall -Werror
CFLAGS  = -g -Werror
# the build target executable:
TARGET = gobang

all: $(TARGET)

$(TARGET): $(TARGET).cpp $(TARGET).h
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)