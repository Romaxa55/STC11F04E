#include <8051.h>

unsigned char clapCount = 0;  // Счётчик хлопков
unsigned int lastClapTime = 0;  // Время последнего хлопка
unsigned char lightState = 0;   // Состояние света (включён/выключен)

// Прототипы функций
void UART_Init(void);
void UART_SendByte(unsigned char data);
void UART_SendString(const char* str);
void Timer0_Init(void);
void Timer0_ISR(void) __interrupt(1);
void toggleLight(void);
void delay_ms(unsigned int ms);  // Добавляем объявление функции задержки
void checkButton(void);

// Глобальные переменные
volatile unsigned int ms_counter = 0;  // Счётчик миллисекунд

// Инициализация UART для передачи данных
void UART_Init(void) {
    SCON = 0x50;  // UART: режим 1, 8-битный
    TMOD |= 0x20; // Таймер 1 в режиме 2
    TH1 = 0xFD;   // 9600 бод
    TR1 = 1;      // Старт таймера
}

// Инициализация таймера 0
void Timer0_Init(void) {
    TMOD |= 0x01;  // Таймер 0, режим 1
    TH0 = 0xFC;    // Генерация прерывания каждые 1 мс
    TL0 = 0x66;
    ET0 = 1;       // Разрешение прерываний таймера 0
    EA = 1;        // Глобальные прерывания
    TR0 = 1;       // Запуск таймера
}

// Прерывание таймера 0
void Timer0_ISR(void) __interrupt(1) {
TH0 = 0xFC;  // Перезагрузка таймера
TL0 = 0x66;
ms_counter++;
}

// Функция отправки байта по UART
void UART_SendByte(unsigned char data) {
    SBUF = data;
    while (TI == 0);
    TI = 0;
}

// Функция для отправки строки по UART
void UART_SendString(const char* str) {
    while (*str) {
        UART_SendByte(*str++);
    }
}

// Функция для переключения состояния света
void toggleLight(void) {
    lightState = !lightState;
    P1_0 = lightState;  // Например, на порту P1.0 подключен свет
    UART_SendString("Light state changed to: ");
    UART_SendByte(lightState ? '1' : '0');
    UART_SendString("\r\n");
}

// Функция задержки в миллисекундах
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 240; j++);  // Примерная задержка для 1 мс
    }
}

// Функция для проверки состояния кнопки на порте P3_7
void checkButton(void) {
    if (P3_7 == 0) {  // Проверка нажатия кнопки (активный низкий уровень)
        delay_ms(100);  // Антидребезг
        if (P3_7 == 0) {  // Если кнопка все еще нажата
            UART_SendString("Button pressed, toggling light...\r\n");
            toggleLight();  // Переключаем состояние света
            while (P3_7 == 0);  // Ожидаем отпускания кнопки
        }
    }
}

// Главная программа
void main(void) {
    unsigned char previousState = 0;
    unsigned char currentState;
    unsigned int currentTime;

    UART_Init();  // Инициализация UART
    Timer0_Init();  // Инициализация таймера

    // Устанавливаем начальное состояние света (выключен)
    P1_0 = 0;
    lightState = 0;

    UART_SendString("Clap control initialized. Light is OFF.\r\n");

    while (1) {
        checkButton();  // Проверка состояния кнопки

        currentState = P3_3;  // Считывание сигнала с микрофона

        if (currentState != previousState && currentState == 1) {  // Изменение с 0 на 1
            currentTime = ms_counter;
            if (currentTime - lastClapTime < 1000) {  // Если второй хлопок в течение 1 секунды
                clapCount++;
            } else {
                clapCount = 1;  // Если хлопки разделены длительным промежутком
            }

            lastClapTime = currentTime;

            if (clapCount == 2) {
                toggleLight();  // Переключаем свет после двух хлопков
                clapCount = 0;  // Сбрасываем счётчик хлопков
            }

            UART_SendString("Clap detected!\r\n");
        }

        previousState = currentState;  // Обновляем предыдущее состояние
    }
}