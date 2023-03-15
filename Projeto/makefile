# Define the compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Werror

# Define the source and object files
SRCS = main.c UDP.c TCP.c
OBJS = $(SRCS:.c=.o)

# Define the name of the executable
TARGET = cot

# Define the build rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Define the clean rule
clean:
	rm -f $(OBJS) $(TARGET)
