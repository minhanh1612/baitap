#include "../inc/strutils.h"

// Hàm đảo ngược chuỗi tại chỗ
char* str_reverse(char* str) {
    if (str == NULL) {
        return NULL;
    }
    
    int len = strlen(str);
    if (len <= 1) {
        return str;
    }
    
    // Đổi chỗ các ký tự từ đầu và cuối
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
    
    return str;
}

// Hàm loại bỏ khoảng trắng đầu và cuối chuỗi
char* str_trim(char* str) {
    if (str == NULL) {
        return NULL;
    }
    
    // Bỏ khoảng trắng ở đầu
    char* start = str;
    while (isspace(*start)) {
        start++;
    }
    
    // Nếu chuỗi chỉ toàn khoảng trắng
    if (*start == '\0') {
        *str = '\0';
        return str;
    }
    
    // Bỏ khoảng trắng ở cuối
    char* end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) {
        end--;
    }
    
    // Di chuyển chuỗi đã trim về đầu buffer
    int len = end - start + 1;
    for (int i = 0; i < len; i++) {
        str[i] = start[i];
    }
    str[len] = '\0';
    
    return str;
}

// Hàm chuyển đổi chuỗi thành số nguyên an toàn
int str_to_int(const char* str, int* result) {
    if (str == NULL || result == NULL) {
        return 0;
    }
    
    // Bỏ qua khoảng trắng đầu
    while (isspace(*str)) {
        str++;
    }
    
    // Kiểm tra chuỗi rỗng
    if (*str == '\0') {
        return 0;
    }
    
    // Kiểm tra dấu âm/dương
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Kiểm tra có ký tự số không
    if (!isdigit(*str)) {
        return 0;
    }
    
    // Chuyển đổi
    long temp = 0;
    while (isdigit(*str)) {
        temp = temp * 10 + (*str - '0');
        
        // Kiểm tra overflow
        if (temp > 2147483647) { // INT_MAX
            return 0;
        }
        str++;
    }
    
    // Kiểm tra có ký tự không hợp lệ ở cuối không
    while (isspace(*str)) {
        str++;
    }
    if (*str != '\0') {
        return 0;
    }
    
    *result = (int)(temp * sign);
    return 1;
}