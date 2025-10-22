#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // O_CREAT, O_RDWR
#include <sys/mman.h>   // shm_open, mmap
#include <sys/stat.h>   // mode constants
#include <unistd.h>     // ftruncate, close
#include <string.h>     // strcpy

#define SHM_NAME "/my_shared_mem"

typedef struct {
    int id;
    char name[50];
    float price;
} Product;

int main() {
    int shm_fd;
    Product *shared_product;

    // 1. Tạo hoặc mở vùng nhớ chia sẻ
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    // 2. Đặt kích thước cho vùng nhớ
    if (ftruncate(shm_fd, sizeof(Product)) == -1) {
        perror("ftruncate failed");
        exit(1);
    }

    // 3. Ánh xạ vùng nhớ vào không gian địa chỉ tiến trình
    shared_product = mmap(0, sizeof(Product), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_product == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // 4. Ghi dữ liệu sản phẩm vào vùng nhớ
    shared_product->id = 101;
    strcpy(shared_product->name, "Embedded Linux Board");
    shared_product->price = 59.99;

    printf("Producer: Đã ghi sản phẩm vào bộ nhớ chia sẻ.\n");
    printf("  ID: %d\n  Name: %s\n  Price: %.2f\n",
           shared_product->id, shared_product->name, shared_product->price);

    // 5. Giải phóng
    munmap(shared_product, sizeof(Product));
    close(shm_fd);

    return 0;
}
