# Define the compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Werror

# Define the source and object files
SRCS = fs.c cot.c
OBJS = $(SRCS:.c=.o)

# Define the name of the executable
TARGET = cot.out

# Define the build rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Define the clean rule
clean:
	rm -f $(OBJS) $(TARGET)
