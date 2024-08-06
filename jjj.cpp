#include <graphics.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAXIMIZE 1
#define MINIMIZE 2
#define CLOSE 3
#define KEYPAD 4

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Coordinates for button positions
#define CLOSE_X1 590
#define CLOSE_Y1 10
#define CLOSE_X2 630
#define CLOSE_Y2 50

#define MINIMIZE_X1 550
#define MINIMIZE_Y1 10
#define MINIMIZE_X2 590
#define MINIMIZE_Y2 50

#define MAXIMIZE_X1 510
#define MAXIMIZE_Y1 10
#define MAXIMIZE_X2 550
#define MAXIMIZE_Y2 50

#define KEYPAD_X 50
#define KEYPAD_Y 70
#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 60
#define BUTTON_PADDING_X 10
#define BUTTON_PADDING_Y 10
#define BUTTON_ROWS 4
#define BUTTON_COLS 3

// Adjust these values if you want different sizes for minimized and maximized states
#define MINIMIZED_KEYPAD_WIDTH 180
#define MINIMIZED_KEYPAD_HEIGHT 240
#define MAXIMIZED_KEYPAD_WIDTH 240
#define MAXIMIZED_KEYPAD_HEIGHT 300

char currentNumber[40] = "";  // Buffer to store the entered number
int isKeypadVisible = 1;      // Flag to control keypad visibility
int keypadWidth = MAXIMIZED_KEYPAD_WIDTH;
int keypadHeight = MAXIMIZED_KEYPAD_HEIGHT;

void draw_buttons();
void draw_keypad();
int check_button_click(int x, int y);
int check_keypad_click(int x, int y);
void clear_button_area(int button);
void maximize_window();
void minimize_window();
void close_window();
void handle_keypad_click(int x, int y);

// Euclid's Algorithm function
int gcd(int a, int b);
void display_gcd_result(int result);

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    draw_buttons();
    draw_keypad();

    while (1) {
        if (kbhit()) {
            int ch = getch();
            if (ch == 27) { // ESC key
                close_window();
                break;
            }
        }

        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);
            int button = check_button_click(x, y);

            switch (button) {
                case MAXIMIZE:
                    clear_button_area(MAXIMIZE);
                    maximize_window();
                    break;
                case MINIMIZE:
                    clear_button_area(MINIMIZE);
                    minimize_window();
                    break;
                case CLOSE:
                    clear_button_area(CLOSE);
                    close_window();
                    return 0;
            }

            // Check keypad buttons
            if (isKeypadVisible) {
                int keypadButton = check_keypad_click(x, y);
                if (keypadButton != 0) {
                    handle_keypad_click(x, y);
                }
            }
        }
    }

    closegraph();
    return 0;
}

void draw_buttons() {
    setcolor(WHITE);

    // Draw close button
    rectangle(CLOSE_X1, CLOSE_Y1, CLOSE_X2, CLOSE_Y2);
    outtextxy((CLOSE_X1 + CLOSE_X2) / 2 - 10, (CLOSE_Y1 + CLOSE_Y2) / 2 - 10, "X");

    // Draw minimize button
    rectangle(MINIMIZE_X1, MINIMIZE_Y1, MINIMIZE_X2, MINIMIZE_Y2);
    outtextxy((MINIMIZE_X1 + MINIMIZE_X2) / 2 - 10, (MINIMIZE_Y1 + MINIMIZE_Y2) / 2 - 10, "-");

    // Draw maximize button
    rectangle(MAXIMIZE_X1, MAXIMIZE_Y1, MAXIMIZE_X2, MAXIMIZE_Y2);
    outtextxy((MAXIMIZE_X1 + MAXIMIZE_X2) / 2 - 10, (MAXIMIZE_Y1 + MAXIMIZE_Y2) / 2 - 10, "[]");
}

void draw_keypad() {
    if (!isKeypadVisible) return;  // Don't draw keypad if not visible

    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);

    int keypadX = KEYPAD_X;
    int keypadY = KEYPAD_Y;
    int buttonWidth = (keypadWidth - (BUTTON_COLS - 1) * BUTTON_PADDING_X) / BUTTON_COLS;
    int buttonHeight = (keypadHeight - (BUTTON_ROWS - 1) * BUTTON_PADDING_Y) / BUTTON_ROWS;

    // Draw keypad buttons
    for (int row = 0; row < BUTTON_ROWS; row++) {
        for (int col = 0; col < BUTTON_COLS; col++) {
            int buttonX = keypadX + col * (buttonWidth + BUTTON_PADDING_X);
            int buttonY = keypadY + row * (buttonHeight + BUTTON_PADDING_Y);
            rectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);

            // Draw numbers on buttons
            char buttonText[2] = {0};
            int buttonNumber = row * BUTTON_COLS + col + 1;
            if (buttonNumber <= 9) { // Number buttons (1-9)
                buttonText[0] = buttonNumber + '0';
            } else if (buttonNumber == 10) { // 0 button
                strcpy(buttonText, "0");
            } else if (buttonNumber == 11) { // '*' button for GCD
                strcpy(buttonText, "*");
            }
            outtextxy(buttonX + buttonWidth / 2 - 10, buttonY + buttonHeight / 2 - 10, buttonText);
        }
    }
}

int check_button_click(int x, int y) {
    if (x >= CLOSE_X1 && x <= CLOSE_X2 && y >= CLOSE_Y1 && y <= CLOSE_Y2) {
        return CLOSE;
    } else if (x >= MINIMIZE_X1 && x <= MINIMIZE_X2 && y >= MINIMIZE_Y1 && y <= MINIMIZE_Y2) {
        return MINIMIZE;
    } else if (x >= MAXIMIZE_X1 && x <= MAXIMIZE_X2 && y >= MAXIMIZE_Y1 && y <= MAXIMIZE_Y2) {
        return MAXIMIZE;
    }
    return 0;
}

int check_keypad_click(int x, int y) {
    if (x >= KEYPAD_X && x <= KEYPAD_X + keypadWidth &&
        y >= KEYPAD_Y && y <= KEYPAD_Y + keypadHeight) {

        int buttonCol = (x - KEYPAD_X) / ((keypadWidth / BUTTON_COLS) + BUTTON_PADDING_X);
        int buttonRow = (y - KEYPAD_Y) / ((keypadHeight / BUTTON_ROWS) + BUTTON_PADDING_Y);
        int buttonNumber = buttonRow * BUTTON_COLS + buttonCol + 1;

        if (buttonNumber <= 10 || buttonNumber == 11) { // Check if it's a valid button
            return buttonNumber;
        }
    }
    return 0;
}

void clear_button_area(int button) {
    setfillstyle(SOLID_FILL, BLACK);
    switch (button) {
        case MAXIMIZE:
            bar(MAXIMIZE_X1, MAXIMIZE_Y1, MAXIMIZE_X2, MAXIMIZE_Y2);
            break;
        case MINIMIZE:
            bar(MINIMIZE_X1, MINIMIZE_Y1, MINIMIZE_X2, MINIMIZE_Y2);
            break;
        case CLOSE:
            bar(CLOSE_X1, CLOSE_Y1, CLOSE_X2, CLOSE_Y2);
            break;
    }
}

void handle_keypad_click(int x, int y) {
    int numberPressed = check_keypad_click(x, y);
    if (numberPressed != 0) {
        if (numberPressed == 11) { // '*' button pressed
            // Calculate and display GCD
            int num1, num2;
            sscanf(currentNumber, "%d,%d", &num1, &num2);
            int result = gcd(num1, num2);
            display_gcd_result(result);
            return;
        }

        char numberString[2] = {0};
        if (numberPressed <= 9) {
            numberString[0] = numberPressed + '0';
        } else if (numberPressed == 10) {
            strcpy(numberString, "0");
        }
        strcat(currentNumber, numberString);  // Append pressed number to current number

        cleardevice();
        draw_buttons();
        draw_keypad();
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
        outtextxy(50, 400, currentNumber);  // Display current entered number
    }
}

void maximize_window() {
    cleardevice();
    draw_buttons();
    keypadWidth = MAXIMIZED_KEYPAD_WIDTH;
    keypadHeight = MAXIMIZED_KEYPAD_HEIGHT;
    draw_keypad();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(200, 200, "Window Maximized!");
}

void minimize_window() {
    cleardevice();
    draw_buttons();
    keypadWidth = MINIMIZED_KEYPAD_WIDTH;
    keypadHeight = MINIMIZED_KEYPAD_HEIGHT;
    draw_keypad();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(200, 200, "Window Minimized!");
}

void close_window() {
    cleardevice();
    isKeypadVisible = 0;  // Hide the keypad
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(200, 200, "Window Closed!");
    delay(1000); // Display message for a while
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void display_gcd_result(int result) {
    cleardevice();
    draw_buttons();
    draw_keypad();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    char resultText[50];
    sprintf(resultText, "GCD: %d", result);
    outtextxy(50, 400, resultText);  // Display GCD result
    delay(2000); // Display result for a while
}