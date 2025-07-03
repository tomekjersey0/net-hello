# Compiler and flags
CXX = g++
CXXFLAGS = -c -Wall -Iinclude

# Paths
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

# Libraries
LIBS = -lwinmm -lws2_32

# Object files
SOCKADDR_OBJ = $(BUILD_DIR)/SockAddr.o
GETERROR_OBJ = $(BUILD_DIR)/GetError.o
SERVER_OBJ = $(BUILD_DIR)/Server.o
CLIENT_OBJ = $(BUILD_DIR)/Client.o

.PHONY: all clean help server client

all: server client

# Create build directories if they don't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile source files to objects
$(SOCKADDR_OBJ): $(SRC_DIR)/SockAddr.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(GETERROR_OBJ): $(SRC_DIR)/GetError.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(SERVER_OBJ): $(SRC_DIR)/Server.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(CLIENT_OBJ): $(SRC_DIR)/Client.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# Link server executable
server: $(SOCKADDR_OBJ) $(GETERROR_OBJ) $(SERVER_OBJ) | $(BIN_DIR)
	$(CXX) $(SERVER_OBJ) $(SOCKADDR_OBJ) $(GETERROR_OBJ) -o $(BIN_DIR)/Server $(LIBS)

# Link client executable
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
