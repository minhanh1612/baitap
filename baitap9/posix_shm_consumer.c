#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // O_RDONLY
#include <sys/mman.h>   // shm_open, mmap
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/my_shared_mem"

typedef struct {
    int id;
    char name[50];
    float price;
} Product;

int main() {
    int shm_fd;
    Product *shared_product;

    // 1. Mở vùng nhớ chia sẻ
    shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    // 2. Ánh xạ vùng nhớ
    shared_product = mmap(0, sizeof(Product), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shared_product == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // 3. Đọc dữ liệu và in ra
    printf("Consumer: Đã đọc thông tin từ bộ nhớ chia sẻ.\n");
    printf("  ID: %d\n  Name: %s\n  Price: %.2f\n",
           shared_product->id, shared_product->name, shared_product->price);

    // 4. Giải phóng
    munmap(shared_product, sizeof(Product));
    close(shm_fd);

    // 5. Xóa vùng nhớ chia sẻ sau khi dùng
    shm_unlink(SHM_NAME);

    return 0;
}
