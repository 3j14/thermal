CC := c++

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/thermal

SRC_EXT := cpp
SOURCES := $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SRC_EXT)=.o))
CFLAGS := -g -Wall $(shell python3-config --cflags)
LIB := $(shell python3-config --ldflags)
INC := -Iinclude $(shell python3-config --includes)


$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@echo "Building..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning..." 
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)

test:
	@echo "Testing ..."  # TODO add tests

.PHONY: clean
