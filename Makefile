# 컴파일러 및 플래그 설정
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++98 -g

# 디렉토리 설정
SRC_DIR = src
INC_DIR = $(shell find include -type d -exec echo -I{} \;)
BUILD_DIR = build

# 소스 파일 및 객체 파일 설정
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# 실행 파일 이름
TARGET = webserv

# 기본 규칙
all: $(TARGET)

# 실행 파일 생성 규칙
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INC_DIR)

# 개별 객체 파일 생성 규칙
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INC_DIR)

# 빌드 디렉토리 생성 규칙
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 정리 규칙
clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
