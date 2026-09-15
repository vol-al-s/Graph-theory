# Компилятор и флаги
CXX = g++
# Флаг -Iheader указывает компилятору искать заголовочные файлы в папке header
CXXFLAGS = -Wall -Wextra -std=c++11 -Iheader

# Директории
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
HEADER_DIR = header

# Исходные и объектные файлы
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/lab_1/graph.cpp $(SRC_DIR)/lab_1/distribution.cpp \
       $(SRC_DIR)/lab_1/shimbel.cpp $(SRC_DIR)/lab_1/road.cpp $(SRC_DIR)/lab_1/quantities.cpp \
       $(SRC_DIR)/lab_2/depth.cpp $(SRC_DIR)/lab_2/bellmanford.cpp \
       $(SRC_DIR)/lab_3/maxflow.cpp $(SRC_DIR)/lab_3/mincost.cpp \
	   $(SRC_DIR)/lab_4/kirchhoff.cpp $(SRC_DIR)/lab_4/kruskal.cpp \
       $(SRC_DIR)/lab_4/prufer.cpp $(SRC_DIR)/lab_4/vertexcover.cpp
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Исполняемый файл
EXEC = $(BIN_DIR)/lab1

# Цель по умолчанию
all: directories $(EXEC)

# Создание папок obj и bin, если их еще нет
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Линковка (сборка финального файла в папке bin)
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Компиляция .cpp файлов из src/ в .o файлы в папке obj/
# Компиляция .cpp файлов из src/ в .o файлы в папке obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === БЫСТРЫЙ ЗАПУСК ===
# Команда "make run" сначала проверит изменения (вызовет all), а затем запустит программу
run: all
	./$(EXEC)

# Очистка сгенерированных файлов
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*
