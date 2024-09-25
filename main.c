#include <8051.h>  // Включаем заголовочный файл для 8051 микроконтроллеров

void delay(unsigned int ms) {
    while (ms--) {
        unsigned int i = 120;
        while (i--);
    }
}

void main() {
    while (1) {
        P1_0 = 0;     // Включаем светодиод
        delay(500);   // Задержка 500 мс
        P1_0 = 1;     // Выключаем светодиод
        delay(500);   // Задержка 500 мс
    }
}