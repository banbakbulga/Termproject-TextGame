#include <iostream>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include <string>

using namespace std;

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const int NUM_COLUMNS = 4;
const int COLUMN_WIDTH = SCREEN_WIDTH / NUM_COLUMNS;
const int NUM_ROWS = 10;
const int STRING_SPEED_INTERVAL = 500;
const int MAX_STRING_LENGTH = 8;
const int SCORE_TO_WIN = 10;

class StringComponent {
public:
    virtual string generateString() = 0;
};

class BaseString : public StringComponent {
public:
    string generateString() override {
        string randomString;
        const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < MAX_STRING_LENGTH; ++i) {
            randomString += characters[rand() % characters.length()];
        }
        return randomString;
    }
};

class AlphabetDecorator : public StringComponent {
    StringComponent* baseString;

public:
    AlphabetDecorator(StringComponent* base) : baseString(base) {}

    string generateString() override {
        return baseString->generateString() + "abcdefghijklmnopqrstuvwxyz";
    }
};

class NumberDecorator : public StringComponent {
    StringComponent* baseString;

public:
    NumberDecorator(StringComponent* base) : baseString(base) {}

    string generateString() override {
        string numberString;
        for (int i = 0; i < MAX_STRING_LENGTH; ++i) {
            numberString += to_string(rand() % 10); // 숫자로만 이루어진 문자열 생성
        }
        return numberString;
    }
};

class SpecialCharacterDecorator : public StringComponent {
    StringComponent* baseString;

public:
    SpecialCharacterDecorator(StringComponent* base) : baseString(base) {}

    string generateString() override {
        string specialCharString;
        const string characters = "!@#$%^&*()"; // 특수 문자로만 구성된 문자열
        for (int i = 0; i < MAX_STRING_LENGTH; ++i) {
            specialCharString += characters[rand() % characters.length()];
        }
        return specialCharString;
    }
};

class AlphabetNumberString : public StringComponent { //알파벳과 숫자로만 구성된 문자열
public:
    string generateString() override {
        string mixedString;
        const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        for (int i = 0; i < MAX_STRING_LENGTH; ++i) {
            mixedString += characters[rand() % characters.length()];
        }
        return mixedString;
    }
};

class AlphabetSpecialCharString : public StringComponent { // 알파벳과 특수문자로만 구성된 문자열
public:
    string generateString() override {
        string mixedString;
        const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()";
        for (int i = 0; i < MAX_STRING_LENGTH; ++i) {
            mixedString += characters[rand() % characters.length()];
        }
        return mixedString;
    }
};

class NumberSpecialCharString : public StringComponent { // 숫자와 특수문자로만 구성된 문자열
public:
    string generateString() override {
        string mixedString;
        const string characters = "0123456789!@#$%^&*()";
        for (int i = 0; i < MAX_STRING_LENGTH; ++i) {
            mixedString += characters[rand() % characters.length()];
        }
        return mixedString;
    }
};


class FallingString {
public:
    int x;
    int y;
    string value;
    int speed;

    FallingString(StringComponent* stringComponent) {
        value = stringComponent->generateString();
        value = value.substr(0, MAX_STRING_LENGTH); // 제한된 길이로 자르기
    }
};

bool isGameOver = false;
int playerScore = 0;

void drawFallingString(const FallingString& fs) {
    cout << "\033[" << fs.y << ";" << fs.x << "H"; 
    cout << fs.value;
}

void clearFallingString(const FallingString& fs) {
    cout << "\033[" << fs.y << ";" << fs.x << "H";
    for (int i = 0; i < fs.value.length(); ++i) {
        cout << ' ';
    }
}

void updateFallingString(FallingString& fs, StringComponent* selectedStringComponent) { 
    clearFallingString(fs);

    fs.y++;
    if (fs.y >= SCREEN_HEIGHT) {
        fs.y = 1;
        fs.x = rand() % SCREEN_WIDTH;
        fs.speed = rand() % 3 + 1; // 1~3의 랜덤한 속도 설정
        fs.value = selectedStringComponent->generateString(); // 새로운 문자열 생성
        fs.value = fs.value.substr(0, MAX_STRING_LENGTH); // 제한된 길이로 자르기
    }

    drawFallingString(fs);
}

void displayScore(int score) {
    cout << "\033[" << SCREEN_HEIGHT + 1 << ";0H"; 
    cout << "Score: " << score << "   ";
}

void displayUserInput(const string& userInput) {
    cout << "\033[" << SCREEN_HEIGHT << ";0H"; 
    cout << "Your Input: " << userInput << "   ";
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    StringComponent* baseString = new BaseString();
    StringComponent* alphabetString = new AlphabetDecorator(baseString);
    StringComponent* numberString = new NumberDecorator(baseString);
    StringComponent* specialCharString = new SpecialCharacterDecorator(baseString);
    StringComponent* alphabetNumberString = new AlphabetNumberString(); 
    StringComponent* alphabetSpecialCharString = new AlphabetSpecialCharString(); 
    StringComponent* numberSpecialCharString = new NumberSpecialCharString(); 

    cout << "Select an option please :\n";
    cout << "1. Alphabet\n";
    cout << "2. Number\n";
    cout << "3. Special Characters\n";
    cout << "12. Alphabet + Number\n";
    cout << "13. Alphabet + Special Characters\n";
    cout << "23. Number + Special Characters\n";

    int option;
    cin >> option;

    StringComponent* selectedStringComponent = nullptr;

    switch (option) {
    case 1:
        selectedStringComponent = alphabetString;
        break;
    case 2:
        selectedStringComponent = numberString;
        break;
    case 3:
        selectedStringComponent = specialCharString;
        break;
    case 12:
        selectedStringComponent = alphabetNumberString;
        break;
    case 13:
        selectedStringComponent = alphabetSpecialCharString;
        break;
    case 23:
        selectedStringComponent = numberSpecialCharString;
        break;
    default:
        cerr << "Invalid option. Exiting.";
        delete baseString;
        delete alphabetString;
        delete numberString;
        delete specialCharString;
        delete alphabetNumberString;
        delete alphabetSpecialCharString;
        delete numberSpecialCharString;
        return 1;
    }

    // 옵션 선택 후, 옵션 메시지를 지우기
    system("cls");

    FallingString fallingString(selectedStringComponent);
    fallingString.y = 1;
    fallingString.x = rand() % SCREEN_WIDTH;
    fallingString.speed = rand() % 3 + 1; // 문자열이 내려오는 속도 설정

    string userInput;

    while (!isGameOver && playerScore < SCORE_TO_WIN) {
        updateFallingString(fallingString, selectedStringComponent); 

        if (_kbhit()) {
            char inputChar = _getch();
            if (inputChar == '\b' && !userInput.empty()) {
                userInput.pop_back(); // Backspace 처리
            }
            else {
                userInput += inputChar;
            }
            displayUserInput(userInput);

            // quit를 입력하면 게임이 종료됨
            if (userInput == "quit") {
                isGameOver = true;
                break;
            }
        }

        if (userInput == fallingString.value) {
            clearFallingString(fallingString);
            fallingString.y = 1;
            fallingString.x = rand() % SCREEN_WIDTH;
            fallingString.speed = rand() % 3 + 1;
            fallingString.value = selectedStringComponent->generateString();
            fallingString.value = fallingString.value.substr(0, MAX_STRING_LENGTH);
            playerScore++;
            displayScore(playerScore);
            userInput.clear();
        }

        if (playerScore == SCORE_TO_WIN) {
            isGameOver = true;
            cout << "\033[" << SCREEN_HEIGHT + 2 << ";0H";
            cout << "Congratulations! You've reached the target score of " << SCORE_TO_WIN << " points!" << endl;
        }

        Sleep(STRING_SPEED_INTERVAL);
    }

    userInput.clear();

    delete baseString;
    delete alphabetString;
    delete numberString;
    delete specialCharString;

    return 0;
}