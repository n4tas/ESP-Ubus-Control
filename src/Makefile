SRC:=$(wildcard *.c)
BIN:=esp
LDFLAGS:=-lserialport -lblobmsg_json -lubox -lubus
CFLAGS:=-Wall -Werror
.PHONY:all clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $^ $(LDFLAGS)

clean:
	$(RM) $(BIN)
