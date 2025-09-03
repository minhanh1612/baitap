#include <stdio.h>
#include <string.h>
#include "../inc/strutils.h"

int main() {
    printf("=== DEMO THU VIEN STRUTILS ===\n\n");
    
    // Test 1: str_reverse
    printf("1. Test ham str_reverse:\n");
    char test1[] = "Hello World";
    printf("   Truoc khi reverse: '%s'\n", test1);
    str_reverse(test1);
    printf("   Sau khi reverse:   '%s'\n\n", test1);
    
    char test2[] = "12345";
    printf("   Truoc khi reverse: '%s'\n", test2);
    str_reverse(test2);
    printf("   Sau khi reverse:   '%s'\n\n", test2);
    
    // Test 2: str_trim
    printf("2. Test ham str_trim:\n");
    char test3[] = "   Hello World   ";
    printf("   Truoc khi trim: '%s' (do dai: %lu)\n", test3, strlen(test3));
    str_trim(test3);
    printf("   Sau khi trim:   '%s' (do dai: %lu)\n\n", test3, strlen(test3));
    
    char test4[] = "\t\n  Embedded C  \t\n ";
    printf("   Truoc khi trim: '%s' (do dai: %lu)\n", test4, strlen(test4));
    str_trim(test4);
    printf("   Sau khi trim:   '%s' (do dai: %lu)\n\n", test4, strlen(test4));
    
    // Test 3: str_to_int
    printf("3. Test ham str_to_int:\n");
    int result;
    
    // Test với số hợp lệ
    if (str_to_int("12345", &result)) {
        printf("   '12345' -> %d (thanh cong)\n", result);
    } else {
        printf("   '12345' -> khong chuyen doi duoc\n");
    }
    
    // Test với số âm
    if (str_to_int("-987", &result)) {
        printf("   '-987' -> %d (thanh cong)\n", result);
    } else {
        printf("   '-987' -> khong chuyen doi duoc\n");
    }
    
    // Test với chuỗi có khoảng trắng
    if (str_to_int("  456  ", &result)) {
        printf("   '  456  ' -> %d (thanh cong)\n", result);
    } else {
        printf("   '  456  ' -> khong chuyen doi duoc\n");
    }
    
    // Test với chuỗi không hợp lệ
    if (str_to_int("abc123", &result)) {
        printf("   'abc123' -> %d (thanh cong)\n", result);
    } else {
        printf("   'abc123' -> khong chuyen doi duoc (that bai - dung roi!)\n");
    }
    
    if (str_to_int("123abc", &result)) {
        printf("   '123abc' -> %d (thanh cong)\n", result);
    } else {
        printf("   '123abc' -> khong chuyen doi duoc (that bai - dung roi!)\n");
    }
    
    printf("\n=== KET THUC DEMO ===\n");
    return 0;
}