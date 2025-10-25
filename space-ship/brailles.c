#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *bits;
    int value;
    int codepoint;
    while (1) {
        puts("enter 8 bit bynray :");
        scanf("%8s", bits);
        if (strlen(bits) != 8) {
            exit(0);
        }
        value = (int) strtol(bits, NULL, 2);  // convert binary string → int
        codepoint = 0x2800 + value;           // add Braille base
        // Print as UTF-8
        if (codepoint <= 0x7F) {
            printf("%c\n", codepoint);
        } else if (codepoint <= 0x7FF) {
            printf("%c%c\n",
                   0xC0 | ((codepoint >> 6) & 0x1F),
                   0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0xFFFF) {
            printf("%c%c%c\n",
                   0xE0 | ((codepoint >> 12) & 0x0F),
                   0x80 | ((codepoint >> 6) & 0x3F),
                   0x80 | (codepoint & 0x3F));
        } else {
            printf("%c%c%c%c\n",
                   0xF0 | ((codepoint >> 18) & 0x07),
                   0x80 | ((codepoint >> 12) & 0x3F),
                   0x80 | ((codepoint >> 6) & 0x3F),
                   0x80 | (codepoint & 0x3F));
        }

    }
    return 0;
}
