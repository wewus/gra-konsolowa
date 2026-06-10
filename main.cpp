#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <limits>
using namespace std;

const int MAP_WIDTH = 40;
const int MAP_HEIGHT = 16;
const char PLAYER_CHAR = '+';
const char FALLING_CHAR = 'v';
const char HORIZONTAL_CHAR = '<';
const char WALL_CHAR = '#';
const char EMPTY_CHAR = '.';

enum Difficulty { EASY = 1, MEDIUM = 2, HARD = 3, IMPOSIBLE = 4 };
int spawnInterval = 10;
int wallSpeedFall = 3;
int wallSpeedHoriz = 2;
Difficulty currentDifficulty = MEDIUM;

void applyDifficulty(Difficulty diff) {
    currentDifficulty = diff;
    if (diff == EASY) {
        spawnInterval = 20;
        wallSpeedFall = 5;
        wallSpeedHoriz = 4;
    } else if (diff == MEDIUM) {
        spawnInterval = 10;
        wallSpeedFall = 4;
        wallSpeedHoriz = 3;
    } else if (diff == HARD) {
        spawnInterval = 5;
        wallSpeedFall = 2;
        wallSpeedHoriz = 2;
    } else if (diff == IMPOSIBLE) {
        spawnInterval = 2;
        wallSpeedFall = 1;
        wallSpeedHoriz = 1;
    }
}

const char *difficultyName(Difficulty diff) {
    if (diff == EASY) return "Easy";
    if (diff == MEDIUM) return "Medium";
    if (diff == HARD) return "Hard";
    if (diff == IMPOSIBLE) return "Imposible";
    return "Unknown";
}

struct Wall {
    bool active;
    bool falling;
    int position;
    int gap;
};

bool isWallCell(const Wall &wall, int x, int y) {
    if (!wall.active) return false;
    if (wall.falling) {
        return y == wall.position && x != wall.gap;
    }
    return x == wall.position && y != wall.gap;
}

void drawMap(int playerX, int playerY, const Wall &wall) {
    system("cls");
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                cout << WALL_CHAR;
            } else if (x == playerX && y == playerY) {
                cout << PLAYER_CHAR;
            } else if (isWallCell(wall, x, y)) {
                cout << (wall.falling ? FALLING_CHAR : HORIZONTAL_CHAR);
            } else {
                cout << EMPTY_CHAR;
            }
        }
        cout << '\n';
    }
    cout << "\nWASD to move, Q to quit to menu." << endl;
    cout << "One wall with a single gap appears at a time." << endl;
}

void spawnWall(Wall &wall) {
    if (wall.active) return;
    wall.active = true;
    wall.falling = (rand() % 2 == 0);
    if (wall.falling) {
        wall.position = 1;
        wall.gap = 1 + rand() % (MAP_WIDTH - 2);
    } else {
        wall.position = MAP_WIDTH - 2;
        wall.gap = 1 + rand() % (MAP_HEIGHT - 2);
    }
}

void updateWall(Wall &wall) {
    if (!wall.active) return;
    if (wall.falling) {
        wall.position++;
        if (wall.position >= MAP_HEIGHT - 1) wall.active = false;
    } else {
        wall.position--;
        if (wall.position <= 0) wall.active = false;
    }
}

bool checkCollision(int playerX, int playerY, const Wall &wall) {
    return isWallCell(wall, playerX, playerY);
}

void startGame() {
    srand(static_cast<unsigned>(time(NULL)));
    int playerX = MAP_WIDTH / 2;
    int playerY = MAP_HEIGHT / 2;
    Wall wall = { false, false, 0, 0 };
    int tick = 0;
    bool playing = true;

    while (playing) {
        if (!wall.active && tick % spawnInterval == 0) {
            spawnWall(wall);
        }

        if (tick % (wall.falling ? wallSpeedFall : wallSpeedHoriz) == 0) updateWall(wall);

        if (checkCollision(playerX, playerY, wall)) {
            drawMap(playerX, playerY, wall);
            cout << "\nYou were hit! Game over. Press Enter to return to menu." << endl;
            cin.ignore(32767, '\n');
            cin.get();
            break;
        }

        drawMap(playerX, playerY, wall);

        int key = 0;
        int wait = 0;
        while (wait < 10) {
            if (_kbhit()) {
                key = _getch();
                break;
            }
            Sleep(20);
            wait++;
        }

        if (key == 'w' || key == 'W') {
            if (playerY > 1) playerY--;
        } else if (key == 's' || key == 'S') {
            if (playerY < MAP_HEIGHT - 2) playerY++;
        } else if (key == 'a' || key == 'A') {
            if (playerX > 1) playerX--;
        } else if (key == 'd' || key == 'D') {
            if (playerX < MAP_WIDTH - 2) playerX++;
        } else if (key == 'q' || key == 'Q') {
            playing = false;
        }

        tick++;
    }
}

void waitForEnter() {
    cin.ignore(32767, '\n');
    cin.get();
}

void chooseDifficulty() {
    while (true) {
        system("cls");
        cout << "|====================================|" << endl;
        cout << "|           Select difficulty        |" << endl;
        cout << "|                                    |" << endl;
        cout << "|             1. Easy                |" << endl;
        cout << "|             2. Medium              |" << endl;
        cout << "|             3. Hard                |" << endl;
        cout << "|             4. Imposible           |" << endl;
        cout << "|             5. Back                |" << endl;
        cout << "|====================================|" << endl;
        cout << "Choose difficulty: ";

        int choice;
        cin >> choice;
        if (choice >= 1 && choice <= 3) {
            applyDifficulty(static_cast<Difficulty>(choice));
            cout << "Difficulty set to " << difficultyName(currentDifficulty) << "." << endl;
            cout << "Press Enter to return to the menu.";
            waitForEnter();
            break;
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice. Press Enter to try again.";
            waitForEnter();
        }
    }
}

int main() {
    while (true) {
        cout << "|====================================|" << endl;
        cout << "|              main menu             |" << endl;
        cout << "|                                    |" << endl;
        cout << "|             1. Start Game          |" << endl;
        cout << "|             2. Difficulty          |" << endl;
        cout << "|             3. Exit                |" << endl;
        cout << "|                                    |" << endl;
        cout << "|====================================|" << endl;
        cout << " Current difficulty: " << difficultyName(currentDifficulty) << endl;
        cout << "Please select an option: ";

        int option;
        cin >> option;

        if (option == 1) {
            startGame();
        } else if (option == 2) {
            chooseDifficulty();
        } else if (option == 3) {
            cout << "Exiting..." << endl;
            cout << "Goodbye!" << endl;
            break;
        } else {
            cout << "Invalid option. Please try again." << endl;
            cout << "Press Enter to continue.";
            waitForEnter();
        }
        system("cls");
    }

    return 0;
}
