# Detect platform using uname (works on Linux/macOS)
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    PLATFORM = LINUX
endif
ifeq ($(UNAME_S),Darwin)
    PLATFORM = MACOS
endif
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
endif

# Compiler and flags
CXX = g++
CXXFLAGS = -c -Wall -Iinclude

# Paths
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

# Platform-specific libs and defines
ifeq ($(PLATFORM),WINDOWS)
    LIBS = -lws2_32 -lwinmm
    DEFINES = -D_WIN32
else
    LIBS = -pthread
    DEFINES = -D_POSIX_C_SOURCE=200112L
endif

# Object files
SOCKADDR_OBJ = $(BUILD_DIR)/SockAddr.o
GETERROR_OBJ = $(BUILD_DIR)/GetError.o
SERVER_OBJ = $(BUILD_DIR)/Server.o
CLIENT_OBJ = $(BUILD_DIR)/Client.o

.PHONY: all clean help server client

all: server client

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SOCKADDR_OBJ): $(SRC_DIR)/SockAddr.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEFINES) $< -o $@

$(GETERROR_OBJ): $(SRC_DIR)/GetError.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEFINES) $< -o $@

$(SERVER_OBJ): $(SRC_DIR)/Server.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEFINES) $< -o $@

$(CLIENT_OBJ): $(SRC_DIR)/Client.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEFINES) $< -o $@

server: $(SOCKADDR_OBJ) $(GETERROR_OBJ) $(SERVER_OBJ) | $(BIN_DIR)
	$(CXX) $(SERVER_OBJ) $(SOCKADDR_OBJ) $(GETERROR_OBJ) -o $(BIN_DIR)/Server $(LIBS)

client: $(SOCKADDR_OBJ) $(GETERROR_OBJ) $(CLIENT_OBJ) | $(BIN_DIR)
	$(CXX) $(CLIENT_OBJ) $(SOCKADDR_OBJ) $(GETERROR_OBJ) -o $(BIN_DIR)/Client $(LIBS)

clean:
	rm -f $(BUILD_DIR)/*.o $(BIN_DIR)/Server $(BIN_DIR)/Client

help:
	@echo "Available make commands:"
	@echo "  all      - Build both server and client (default)"
	@echo "  server   - Build the server executable"
	@echo "  client   - Build the client executable"
	@echo "  clean    - Remove object files and executables"
	@echo "  help     - Show this help message"
