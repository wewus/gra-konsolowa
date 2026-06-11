#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <limits>
using namespace std;

const int MAX_MAP_WIDTH = 42;
const int MAX_MAP_HEIGHT = 22;

// A true C-style 2D array for the map display.
char mapGrid[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];

// Characters used for rendering the game world.
const char PLAYER_CHAR = '+';
const char FALLING_CHAR = 'v';
const char HORIZONTAL_CHAR = '<';
const char WALL_CHAR = '#';
const char EMPTY_CHAR = '.';

// Difficulty settings control how fast walls move and how often they appear.
enum Difficulty { EASY = 1, MEDIUM = 2, HARD = 3, IMPOSIBLE = 4 };
int spawnInterval = 10;
int wallSpeedFall = 4;
int wallSpeedHoriz = 3;
int MAP_WIDTH = 32;
int MAP_HEIGHT = 17;
Difficulty currentDifficulty = MEDIUM;

// Configure global game parameters based on the selected difficulty.
void applyDifficulty(Difficulty diff) {
    currentDifficulty = diff;
    if (diff == EASY) {
        spawnInterval = 20;
        wallSpeedFall = 5;
        wallSpeedHoriz = 4;
        MAP_WIDTH = 42;
        MAP_HEIGHT = 22;
    } else if (diff == MEDIUM) {
        spawnInterval = 10;
        wallSpeedFall = 4;
        wallSpeedHoriz = 3;
        MAP_WIDTH = 32;
        MAP_HEIGHT = 17;
    } else if (diff == HARD) {
        spawnInterval = 5;
        wallSpeedFall = 2;
        wallSpeedHoriz = 2;
        MAP_WIDTH = 22;
        MAP_HEIGHT = 12;
    } else if (diff == IMPOSIBLE) {
        spawnInterval = 2;
        wallSpeedFall = 1;
        wallSpeedHoriz = 1;
        MAP_WIDTH = 12;
        MAP_HEIGHT = 7;
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

// Returns true when the wall occupies the given cell coordinates.
bool isWallCell(const Wall &wall, int x, int y) {
    if (!wall.active) return false;
    if (wall.falling) {
        return y == wall.position && x != wall.gap;
    }
    return x == wall.position && y != wall.gap;
}

// Fill the 2D map array for rendering the full frame.
// This includes borders, the current wall, and the player.
void updateMapGrid(int playerX, int playerY, const Wall &wall) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                mapGrid[y][x] = WALL_CHAR;
            } else if (isWallCell(wall, x, y)) {
                mapGrid[y][x] = wall.falling ? FALLING_CHAR : HORIZONTAL_CHAR;
            } else {
                mapGrid[y][x] = EMPTY_CHAR;
            }
        }
    }
    if (playerX > 0 && playerX < MAP_WIDTH - 1 && playerY > 0 && playerY < MAP_HEIGHT - 1) {
        mapGrid[playerY][playerX] = PLAYER_CHAR;
    }
}

// Draw the entire map array on the console.
void drawMap() {
    system("cls");
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            cout << mapGrid[y][x];
        }
        cout << '\n';
    }
    cout << "\nWASD to move, Q to quit to menu." << endl;
    cout << "One wall with a single gap appears at a time." << endl;
}

// Spawn a new wall if none is currently active.
// The wall can either fall from the top or move horizontally from the right.
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

// Move the active wall one step and deactivate it when it leaves the map.
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

// Detect collision between the player and any wall cell.
bool checkCollision(int playerX, int playerY, const Wall &wall) {
    return isWallCell(wall, playerX, playerY);
}

// Main game loop: spawn walls, update the map, read input, and check collisions.
void startGame() {
    srand(static_cast<unsigned>(time(NULL)));
    int playerX = MAP_WIDTH / 2;
    int playerY = MAP_HEIGHT / 2;
    Wall wall = { false, false, 0, 0 };
    int tick = 0;
    bool playing = true;

    while (playing) {
        // Spawn a new wall occasionally, depending on difficulty.
        if (!wall.active && tick % spawnInterval == 0) {
            spawnWall(wall);
        }

        // Move the wall at a speed determined by difficulty.
        if (tick % (wall.falling ? wallSpeedFall : wallSpeedHoriz) == 0) updateWall(wall);

        // Check whether the player is on a wall cell.
        if (checkCollision(playerX, playerY, wall)) {
            updateMapGrid(playerX, playerY, wall);
            drawMap();
            cout << "\nYou were hit! Game over. Press Enter to return to menu." << endl;
            cin.ignore(32767, '\n');
            cin.get();
            break;
        }

        // Update the visual map and display it.
        updateMapGrid(playerX, playerY, wall);
        drawMap();

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

// Helper to pause until the player presses Enter.
void waitForEnter() {
    cin.ignore(32767, '\n');
    cin.get();
}

// Difficulty selection screen shown from the main menu.
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
        if (choice >= 1 && choice <= 4) {
            applyDifficulty(static_cast<Difficulty>(choice));
            cout << "Difficulty set to " << difficultyName(currentDifficulty) << "." << endl;
            cout << "Press Enter to return to the menu.";
            waitForEnter();
            break;
        } else if (choice == 5) {
            break;
        } else {
            cout << "Invalid choice. Press Enter to try again.";
            waitForEnter();
        }
    }
}

int main() {
    // Main menu loop: allow the player to start the game or change settings.
    while (true) {
        cout << "Krzysztof Malecki 1.a" << endl;
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
