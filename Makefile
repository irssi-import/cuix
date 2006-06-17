CC = gcc
SOURCES = cuix.c
OBJ = $(SOURCES:.c=.o)
CFLAGS = -g -Wall -W
LDFLAGS = -lncurses -lform
TARGET = cuix-mockup

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $< $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f $(TARGET) $(OBJ)
