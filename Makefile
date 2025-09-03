
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Đường dẫn thư mục
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# Tên file
STATIC_LIB = $(LIB_DIR)/libstrutils.a
SHARED_LIB = $(LIB_DIR)/libstrutils.so
OBJ_FILE = $(OBJ_DIR)/strutils.o

# Executable files
MAIN_STATIC = $(BIN_DIR)/main_static
MAIN_SHARED = $(BIN_DIR)/main_shared

# Target mặc định - build cả static và shared
all: $(MAIN_STATIC) $(MAIN_SHARED)
	@echo "=== BUILD HOAN THANH ==="
	@echo "Da tao thanh cong:"
	@echo "  - $(STATIC_LIB) (static library)"
	@echo "  - $(SHARED_LIB) (shared library)"
	@echo "  - $(MAIN_STATIC) (su dung static library)"
	@echo "  - $(MAIN_SHARED) (su dung shared library)"

# Build chỉ static library và executable
static: $(MAIN_STATIC)
	@echo "=== BUILD STATIC HOAN THANH ==="

# Build chỉ shared library và executable  
shared: $(MAIN_SHARED)
	@echo "=== BUILD SHARED HOAN THANH ==="

# Tạo các thư mục cần thiết
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

# Biên dịch file object từ source
$(OBJ_FILE): $(SRC_DIR)/strutils.c $(INC_DIR)/strutils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $(SRC_DIR)/strutils.c -o $(OBJ_FILE)

# Tạo static library (.a file)
$(STATIC_LIB): $(OBJ_FILE) | $(LIB_DIR)
	ar rcs $(STATIC_LIB) $(OBJ_FILE)
	@echo "Da tao static library: $(STATIC_LIB)"

# Tạo shared library (.so file) 
$(SHARED_LIB): $(SRC_DIR)/strutils.c $(INC_DIR)/strutils.h | $(LIB_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -fPIC -shared $(SRC_DIR)/strutils.c -o $(SHARED_LIB)
	@echo "Da tao shared library: $(SHARED_LIB)"

# Build executable sử dụng static library
$(MAIN_STATIC): $(SRC_DIR)/main.c $(STATIC_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) $(SRC_DIR)/main.c -L$(LIB_DIR) -lstrutils -o $(MAIN_STATIC)
	@echo "Da tao $(MAIN_STATIC) (su dung static library)"

# Build executable sử dụng shared library
$(MAIN_SHARED): $(SRC_DIR)/main.c $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) $(SRC_DIR)/main.c -L$(LIB_DIR) -lstrutils -o $(MAIN_SHARED)
	@echo "Da tao $(MAIN_SHARED) (su dung shared library)"

# Target để test các chương trình
test: all
	@echo "=== CHAY TEST STATIC VERSION ==="
	$(MAIN_STATIC)
	@echo ""
	@echo "=== CHAY TEST SHARED VERSION ==="
	LD_LIBRARY_PATH=$(LIB_DIR) $(MAIN_SHARED)

# Dọn dẹp các file được tạo ra
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)
	@echo "Da xoa tat ca file build va thu muc"

# Hiển thị thông tin về các file đã tạo
info:
	@echo "=== THONG TIN FILE BUILD ==="
	@echo "Thu muc obj:"
	@ls -la $(OBJ_DIR)/ 2>/dev/null || echo "  Chua co file object nao"
	@echo "Thu muc lib:"
	@ls -la $(LIB_DIR)/ 2>/dev/null || echo "  Chua co library nao"
	@echo "Thu muc bin:"
	@ls -la $(BIN_DIR)/ 2>/dev/null || echo "  Chua co executable nao"
	@echo ""
	@echo "Kich thuoc executable:"
	@wc -c $(BIN_DIR)/* 2>/dev/null || echo "Chua co executable nao"
