# Makefile для лабораторной работы по теории графов

# Компилятор
CXX = g++

# Флаги компиляции
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
DEBUG_FLAGS = -g -DDEBUG

# Директории
SRC_DIR = src
HDR_DIR = header
OBJ_DIR = obj
BIN_DIR = bin

# Исходные файлы (исправлена опечатка: raph.cpp -> graph.cpp)
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/graph.cpp $(SRC_DIR)/random_generator.cpp $(SRC_DIR)/algorithms.cpp
HEADERS = $(HDR_DIR)/graph.h $(HDR_DIR)/random_generator.h $(HDR_DIR)/algorithms.h

# Объектные файлы (будут в obj/, а не в obj/src/)
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/graph.o $(OBJ_DIR)/random_generator.o $(OBJ_DIR)/algorithms.o

# Имя исполняемого файла
TARGET = $(BIN_DIR)/graph_lab

# Правила
.PHONY: all clean debug run

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Правило для компиляции (добавлен -I для поиска заголовков)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) -I$(HDR_DIR) -c -o $@ $<

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean all

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Зависимости (исправлены пути к заголовкам)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(HEADERS)
$(OBJ_DIR)/graph.o: $(SRC_DIR)/graph.cpp $(HDR_DIR)/graph.h
$(OBJ_DIR)/random_generator.o: $(SRC_DIR)/random_generator.cpp $(HDR_DIR)/random_generator.h
$(OBJ_DIR)/algorithms.o: $(SRC_DIR)/algorithms.cpp $(HDR_DIR)/algorithms.h $(HDR_DIR)/graph.h

# Помощь
help:
	@echo "Доступные цели:"
	@echo "  all     - Сборка проекта (по умолчанию)"
	@echo "  debug   - Сборка с отладочной информацией"
	@echo "  run     - Сборка и запуск программы"
	@echo "  clean   - Очистка скомпилированных файлов"
	@echo "  help    - Показать эту справку"