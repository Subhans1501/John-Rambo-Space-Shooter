#include<iostream>
#include<windows.h>
#include<conio.h>
#include<fstream>
#include<ctime>
#include<iomanip>
#include<string>
using namespace std;
const int SCREEN_WIDTH=50;
const int SCREEN_HEIGHT=30;
const int MAX_OBSTACLES=3;
const int MAX_TANKS_L1=3;
const int MAX_TANKS_L2=5;
const int MAX_BULLETS=50;
const int MAX_CRATES=10;
const int BULLET_LIFETIME=10;
const int PLAYABLE_ROWS=15;
const int CRATES_FOR_LIFE=5;
const int LEVEL_UP_DISTANCE=2500;
const int LEVEL_UP_TANKS=10;
const int JUMP_HEIGHT=6;
struct HighScore 
{
    string name;
    int score;
};
char gameMap[SCREEN_HEIGHT][SCREEN_WIDTH];

// Rambo properties
int ramboX, ramboY;
bool ramboJumping;
int ramboJumpCounter;
int ramboLives;
int ramboScore;
int ramboDistance;
int ramboTanksDestroyed;
int ramboCratesDestroyed;

// Obstacles
int obstacleX[MAX_OBSTACLES];
int obstacleY[MAX_OBSTACLES];
int obstacleWidth[MAX_OBSTACLES];
bool obstacleActive[MAX_OBSTACLES];

// Tanks
int tankX[MAX_TANKS_L2];
int tankY[MAX_TANKS_L2];
bool tankActive[MAX_TANKS_L2];
int tankLastShot[MAX_TANKS_L2];

// Bullets
int bulletX[MAX_BULLETS];
int bulletY[MAX_BULLETS];
bool bulletActive[MAX_BULLETS];
bool bulletIsEnemy[MAX_BULLETS];
int bulletDistance[MAX_BULLETS];

// Crates
int crateX[MAX_CRATES];
int crateY[MAX_CRATES];
bool crateActive[MAX_CRATES];

// Game state
bool gameOver;
bool level2;
int gameSpeed;
int currentSpeed;
bool onRoad;
char playerName[50];
int activeTanksCount;
int activeBulletsCount;
int activeCratesCount;
int activeObstaclesCount;
int frameCounter;

// Function declarations
void initializeGame();
void drawGame();
void updateGame();
void processInput();
void generateObstacle();
void generateCrate();
void generateTank();
void updateTanks();
void updateBullets();
void updateRambo();
void checkCollisions();
void drawRambo();
void drawObstacles();
void drawTanks();
void drawBullets();
void drawCrates();
void drawRoad();
void clearScreen();
void setColor(int color);
void drawHUD();
void saveHighScore();
void loadHighScores();
void showHighScores();
void gameOverScreen();
bool checkLevelUp();
void clearMap();
void spawnBullet(int x, int y, bool isEnemy);
void removeInactiveObjects();
void handleJump();
void gotoxy(int x, int y);
void resetCursor();
void drawBorder();
void initializeArrays();
void handleRamboDeath();
void handleTankDeath(int tankIndex);
void handleCrateDestruction(int crateIndex);
void sortHighScores(HighScore scores[], int count);
void displayControls();

int main() {
    srand(time(0));
    initializeGame();
    
    while (!gameOver) {
        processInput();
        updateGame();
        drawGame();
        drawHUD();
        
        if (currentSpeed > 0) {
            Sleep(currentSpeed);
        }
    }
    
    gameOverScreen();
    return 0;
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void resetCursor() {
    gotoxy(0, SCREEN_HEIGHT + 8);
}

void initializeArrays() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacleActive[i] = false;
    }
    for (int i = 0; i < MAX_TANKS_L2; i++) {
        tankActive[i] = false;
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        bulletActive[i] = false;
    }
    for (int i = 0; i < MAX_CRATES; i++) {
        crateActive[i] = false;
    }
}

void initializeGame() {
    system("cls");
    setColor(14);
    cout << "=== JOHN RAMBO CONSOLE GAME ===\n\n";
    setColor(7);
    
    cout << "Enter your name: ";
    cin.getline(playerName, 50);
    
    // Initialize game state
    ramboX = SCREEN_WIDTH / 2;
    ramboY = SCREEN_HEIGHT - 5;
    ramboJumping = false;
    ramboJumpCounter = 0;
    ramboLives = 3;
    ramboScore = 0;
    ramboDistance = 0;
    ramboTanksDestroyed = 0;
    ramboCratesDestroyed = 0;
    gameOver = false;
    level2 = false;
    gameSpeed = 80;
    currentSpeed = gameSpeed;
    onRoad = false;
    activeTanksCount = 0;
    activeBulletsCount = 0;
    activeCratesCount = 0;
    activeObstaclesCount = 0;
    frameCounter = 0;
    
    initializeArrays();
    clearMap();
    
    // Generate initial objects
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        generateObstacle();
    }
    for (int i = 0; i < 3; i++) {
        generateCrate();
    }
    for (int i = 0; i < MAX_TANKS_L1; i++) {
        generateTank();
    }
    
    displayControls();
}

void displayControls() {
    system("cls");
    setColor(11);
    cout << "=== CONTROLS ===\n\n";
    setColor(7);
    cout << "LEFT ARROW or 'A': Move Left\n";
    cout << "RIGHT ARROW or 'D': Move Right\n";
    cout << "SPACEBAR: Jump\n";
    cout << "'S': Shoot\n";
    cout << "'Q': Quit Game\n\n";
    setColor(10);
    cout << "Objective: Destroy tanks, avoid obstacles!\n";
    cout << "Collect crates (5 crates = 1 life)\n";
    setColor(7);
    cout << "\nPress any key to start...";
    _getch();
}

void clearMap() {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            gameMap[i][j] = ' ';
        }
    }
}

void drawBorder() {
    // Top border
    cout << char(201);
    for (int i = 0; i < SCREEN_WIDTH; i++) cout << char(205);
    cout << char(187) << endl;
}

void drawGame() {
    gotoxy(0, 0);
    drawBorder();
    
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        cout << char(186);
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            cout << gameMap[i][j];
        }
        cout << char(186) << endl;
    }
    
    // Bottom border
    cout << char(200);
    for (int i = 0; i < SCREEN_WIDTH; i++) cout << char(205);
    cout << char(188);
}

void updateGame() {
    frameCounter++;
    clearMap();
    
    // Draw road first
    drawRoad();
    
    // Update game objects
    updateRambo();
    updateTanks();
    updateBullets();
    
    // Check for level up
    if (!level2 && checkLevelUp()) {
        level2 = true;
        gameSpeed = gameSpeed * 2 / 3; // 1.5x faster
        currentSpeed = gameSpeed;
        
        // Add more tanks for level 2
        while (activeTanksCount < MAX_TANKS_L2) {
            generateTank();
        }
        
        // Show level up message
        gotoxy(SCREEN_WIDTH/2 - 5, SCREEN_HEIGHT/2);
        setColor(14);
        cout << "LEVEL 2!";
        Sleep(500);
        setColor(7);
    }
    
    // Check if Rambo is on speed road
    int roadStart = SCREEN_WIDTH / 2 - 2;
    int roadEnd = SCREEN_WIDTH / 2 + 2;
    
    if (ramboX >= roadStart && ramboX <= roadEnd) {
        if (!onRoad) {
            onRoad = true;
            currentSpeed = gameSpeed / 2; // Double speed
        }
    } else {
        if (onRoad) {
            onRoad = false;
            currentSpeed = gameSpeed;
        }
    }
    
    // Draw all game objects
    drawObstacles();
    drawCrates();
    drawTanks();
    drawBullets();
    drawRambo();
    
    // Check collisions
    checkCollisions();
    
    // Generate new objects if needed
    if (activeObstaclesCount < MAX_OBSTACLES && frameCounter % 20 == 0) {
        generateObstacle();
    }
    if (activeCratesCount < 3 && frameCounter % 30 == 0) {
        generateCrate();
    }
    
    // Update distance (2 meters per step)
    if (frameCounter % 3 == 0) {
        ramboDistance += 2;
    }
}

void processInput() {
    // Check for key press without blocking
    if (_kbhit()) {
        char ch = _getch();
        
        // Check for arrow keys (extended key)
        if (ch == -32 || ch == 0) {
            ch = _getch();
            switch (ch) {
                case 75: // Left arrow
                    if (ramboX > 1) ramboX--;
                    break;
                case 77: // Right arrow
                    if (ramboX < SCREEN_WIDTH - 4) ramboX++;
                    break;
            }
        } else {
            // Regular keys
            switch (ch) {
                case 'a':
                case 'A':
                    if (ramboX > 1) ramboX--;
                    break;
                case 'd':
                case 'D':
                    if (ramboX < SCREEN_WIDTH - 4) ramboX++;
                    break;
                case ' ':
                    if (!ramboJumping) {
                        ramboJumping = true;
                        ramboJumpCounter = 0;
                    }
                    break;
                case 's':
                case 'S':
                    spawnBullet(ramboX + 1, ramboY - 1, false);
                    break;
                case 'q':
                case 'Q':
                    gameOver = true;
                    break;
            }
        }
    }
}

void handleJump() {
    if (ramboJumping) {
        if (ramboJumpCounter < JUMP_HEIGHT / 2) {
            // Going up
            if (ramboY > SCREEN_HEIGHT - PLAYABLE_ROWS) {
                ramboY--;
            }
        } else if (ramboJumpCounter < JUMP_HEIGHT) {
            // Coming down
            if (ramboY < SCREEN_HEIGHT - 3) {
                ramboY++;
            }
        } else {
            ramboJumping = false;
        }
        ramboJumpCounter++;
    }
}

void updateRambo() {
    // Auto move upward (illusion of forward movement)
    if (frameCounter % 5 == 0 && ramboY > SCREEN_HEIGHT - PLAYABLE_ROWS) {
        ramboY--;
    }
    
    // Handle jumping
    handleJump();
    
    // Keep Rambo in bounds
    if (ramboX < 1) ramboX = 1;
    if (ramboX > SCREEN_WIDTH - 4) ramboX = SCREEN_WIDTH - 4;
    if (ramboY < SCREEN_HEIGHT - PLAYABLE_ROWS) ramboY = SCREEN_HEIGHT - PLAYABLE_ROWS;
    if (ramboY > SCREEN_HEIGHT - 3) ramboY = SCREEN_HEIGHT - 3;
}

void drawRambo() {
    if (ramboY >= 0 && ramboY < SCREEN_HEIGHT && ramboX >= 0 && ramboX < SCREEN_WIDTH - 2) {
        if (ramboJumping) {
            // Jumping Rambo
            setColor(10);
            gameMap[ramboY][ramboX] = '/';
            gameMap[ramboY][ramboX + 1] = 'O';
            gameMap[ramboY][ramboX + 2] = '\\';
        } else {
            // Normal Rambo
            setColor(12);
            gameMap[ramboY][ramboX] = '[';
            gameMap[ramboY][ramboX + 1] = 'O';
            gameMap[ramboY][ramboX + 2] = ']';
        }
        setColor(7);
    }
}

void generateObstacle() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacleActive[i]) {
            obstacleWidth[i] = rand() % 5 + 3; // Width 3-7
            obstacleX[i] = rand() % (SCREEN_WIDTH - obstacleWidth[i] - 4) + 2;
            obstacleY[i] = 0;
            obstacleActive[i] = true;
            activeObstaclesCount++;
            break;
        }
    }
}

void drawObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacleActive[i]) continue;
        
        // Move obstacle down
        obstacleY[i]++;
        
        // Remove if out of screen
        if (obstacleY[i] >= SCREEN_HEIGHT) {
            obstacleActive[i] = false;
            activeObstaclesCount--;
            continue;
        }
        
        // Draw obstacle
        if (obstacleY[i] >= 0 && obstacleY[i] < SCREEN_HEIGHT) {
            setColor(8);
            gameMap[obstacleY[i]][obstacleX[i]] = '|';
            for (int j = 1; j <= obstacleWidth[i]; j++) {
                if (obstacleX[i] + j < SCREEN_WIDTH) {
                    gameMap[obstacleY[i]][obstacleX[i] + j] = '-';
                }
            }
            if (obstacleX[i] + obstacleWidth[i] + 1 < SCREEN_WIDTH) {
                gameMap[obstacleY[i]][obstacleX[i] + obstacleWidth[i] + 1] = '|';
            }
            setColor(7);
        }
    }
}

void generateCrate() {
    for (int i = 0; i < MAX_CRATES; i++) {
        if (!crateActive[i]) {
            crateX[i] = rand() % (SCREEN_WIDTH - 4) + 2;
            crateY[i] = 0;
            crateActive[i] = true;
            activeCratesCount++;
            break;
        }
    }
}

void drawCrates() {
    for (int i = 0; i < MAX_CRATES; i++) {
        if (!crateActive[i]) continue;
        
        // Move crate down
        if (frameCounter % 2 == 0) {
            crateY[i]++;
        }
        
        // Remove if out of screen
        if (crateY[i] >= SCREEN_HEIGHT) {
            crateActive[i] = false;
            activeCratesCount--;
            continue;
        }
        
        // Draw crate
        if (crateY[i] >= 0 && crateY[i] < SCREEN_HEIGHT) {
            setColor(6);
            gameMap[crateY[i]][crateX[i]] = '[';
            gameMap[crateY[i]][crateX[i] + 1] = 'C';
            gameMap[crateY[i]][crateX[i] + 2] = ']';
            setColor(7);
        }
    }
}

void generateTank() {
    int maxTanks = level2 ? MAX_TANKS_L2 : MAX_TANKS_L1;
    
    if (activeTanksCount >= maxTanks) return;
    
    for (int i = 0; i < maxTanks; i++) {
        if (!tankActive[i]) {
            tankX[i] = rand() % (SCREEN_WIDTH - 5) + 2;
            tankY[i] = 2;
            tankActive[i] = true;
            tankLastShot[i] = 0;
            activeTanksCount++;
            break;
        }
    }
}

void updateTanks() {
    int maxTanks = level2 ? MAX_TANKS_L2 : MAX_TANKS_L1;
    
    for (int i = 0; i < maxTanks; i++) {
        if (!tankActive[i]) continue;
        
        // Follow Rambo horizontally
        if (frameCounter % 8 == 0) {
            if (tankX[i] < ramboX) {
                tankX[i]++;
            } else if (tankX[i] > ramboX) {
                tankX[i]--;
            }
        }
        
        // Keep in bounds
        if (tankX[i] < 2) tankX[i] = 2;
        if (tankX[i] > SCREEN_WIDTH - 6) tankX[i] = SCREEN_WIDTH - 6;
        
        // Avoid tank collisions
        for (int j = 0; j < maxTanks; j++) {
            if (i != j && tankActive[j]) {
                if (abs(tankX[i] - tankX[j]) < 4) {
                    if (tankX[i] < tankX[j]) tankX[i]--;
                    else tankX[i]++;
                }
            }
        }
        
        // Shoot randomly
        if (frameCounter % 15 == 0 && rand() % 100 < 20) {
            spawnBullet(tankX[i] + 2, tankY[i] + 3, true);
        }
    }
}

void drawTanks() {
    int maxTanks = level2 ? MAX_TANKS_L2 : MAX_TANKS_L1;
    
    for (int i = 0; i < maxTanks; i++) {
        if (!tankActive[i]) continue;
        
        // Draw tank (4x4)
        if (tankY[i] >= 0 && tankY[i] < SCREEN_HEIGHT - 1 &&
            tankX[i] >= 0 && tankX[i] < SCREEN_WIDTH - 3) {
            
            setColor(4);
            // Top row
            gameMap[tankY[i]][tankX[i]] = '[';
            gameMap[tankY[i]][tankX[i] + 1] = 'T';
            gameMap[tankY[i]][tankX[i] + 2] = ']';
            
            // Bottom row
            if (tankY[i] + 1 < SCREEN_HEIGHT) {
                gameMap[tankY[i] + 1][tankX[i]] = '[';
                gameMap[tankY[i] + 1][tankX[i] + 1] = '#';
                gameMap[tankY[i] + 1][tankX[i] + 2] = ']';
            }
            setColor(7);
        }
    }
}

void spawnBullet(int x, int y, bool isEnemy) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bulletActive[i]) {
            bulletX[i] = x;
            bulletY[i] = y;
            bulletActive[i] = true;
            bulletIsEnemy[i] = isEnemy;
            bulletDistance[i] = 0;
            activeBulletsCount++;
            break;
        }
    }
}

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bulletActive[i]) continue;
        
        if (bulletIsEnemy[i]) {
            // Enemy bullet moves down
            bulletY[i]++;
            bulletDistance[i]++;
            
            // Remove after 10 rows
            if (bulletDistance[i] >= BULLET_LIFETIME) {
                bulletActive[i] = false;
                activeBulletsCount--;
            }
        } else {
            // Rambo bullet moves up
            bulletY[i]--;
        }
        
        // Remove if out of bounds
        if (bulletY[i] < 0 || bulletY[i] >= SCREEN_HEIGHT) {
            bulletActive[i] = false;
            activeBulletsCount--;
        }
    }
}

void drawBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bulletActive[i]) continue;
        
        if (bulletY[i] >= 0 && bulletY[i] < SCREEN_HEIGHT &&
            bulletX[i] >= 0 && bulletX[i] < SCREEN_WIDTH) {
            
            if (bulletIsEnemy[i]) {
                setColor(12); // Red for enemy bullets
                gameMap[bulletY[i]][bulletX[i]] = 'v';
            } else {
                setColor(10); // Green for Rambo bullets
                gameMap[bulletY[i]][bulletX[i]] = '^';
            }
            setColor(7);
        }
    }
}

void drawRoad() {
    int roadStart = SCREEN_WIDTH / 2 - 2;
    int roadEnd = SCREEN_WIDTH / 2 + 2;
    
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = roadStart; j <= roadEnd; j++) {
            if (j >= 0 && j < SCREEN_WIDTH && gameMap[i][j] == ' ') {
                gameMap[i][j] = '=';
            }
        }
    }
}

void checkCollisions() {
    // Check Rambo bullets vs Tanks
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bulletActive[i] || bulletIsEnemy[i]) continue;
        
        int maxTanks = level2 ? MAX_TANKS_L2 : MAX_TANKS_L1;
        for (int j = 0; j < maxTanks; j++) {
            if (!tankActive[j]) continue;
            
            if (bulletX[i] >= tankX[j] && bulletX[i] <= tankX[j] + 3 &&
                bulletY[i] >= tankY[j] && bulletY[i] <= tankY[j] + 3) {
                handleTankDeath(j);
                bulletActive[i] = false;
                activeBulletsCount--;
                break;
            }
        }
    }
    
    // Check Rambo bullets vs Crates
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bulletActive[i] || bulletIsEnemy[i]) continue;
        
        for (int j = 0; j < MAX_CRATES; j++) {
            if (!crateActive[j]) continue;
            
            if (bulletX[i] >= crateX[j] && bulletX[i] <= crateX[j] + 2 &&
                bulletY[i] >= crateY[j] && bulletY[i] <= crateY[j] + 1) {
                handleCrateDestruction(j);
                bulletActive[i] = false;
                activeBulletsCount--;
                break;
            }
        }
    }
    
    // Check Enemy bullets vs Rambo
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bulletActive[i] || !bulletIsEnemy[i]) continue;
        
        if ((bulletX[i] >= ramboX && bulletX[i] <= ramboX + 2) &&
            (bulletY[i] >= ramboY && bulletY[i] <= ramboY + 1)) {
            handleRamboDeath();
            bulletActive[i] = false;
            activeBulletsCount--;
            break;
        }
    }
    
    // Check Rambo vs Obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacleActive[i]) continue;
        
        // Check if Rambo collides with obstacle
        if (ramboY == obstacleY[i]) {
            for (int w = 0; w <= obstacleWidth[i] + 1; w++) {
                int obsX = obstacleX[i] + w;
                if (obsX >= ramboX && obsX <= ramboX + 2) {
                    handleRamboDeath();
                    return;
                }
            }
        }
    }
    
    // Check Rambo vs Crates
    for (int i = 0; i < MAX_CRATES; i++) {
        if (!crateActive[i]) continue;
        
        if ((ramboX >= crateX[i] && ramboX <= crateX[i] + 2) &&
            (ramboY >= crateY[i] && ramboY <= crateY[i] + 1)) {
            handleRamboDeath();
            break;
        }
    }
    
    // Check Rambo vs Tanks
    int maxTanks = level2 ? MAX_TANKS_L2 : MAX_TANKS_L1;
    for (int i = 0; i < maxTanks; i++) {
        if (!tankActive[i]) continue;
        
        if ((ramboX >= tankX[i] - 2 && ramboX <= tankX[i] + 4) &&
            (ramboY >= tankY[i] - 1 && ramboY <= tankY[i] + 3)) {
            handleRamboDeath();
            break;
        }
    }
}

void handleRamboDeath() {
    ramboLives--;
    
    // Visual feedback
    gotoxy(SCREEN_WIDTH/2 - 3, SCREEN_HEIGHT/2);
    setColor(12);
    cout << "OUCH!";
    Sleep(200);
    
    if (ramboLives <= 0) {
        gameOver = true;
    } else {
        // Reset position with brief invincibility
        ramboX = SCREEN_WIDTH / 2;
        ramboY = SCREEN_HEIGHT - 5;
        ramboJumping = false;
    }
}

void handleTankDeath(int tankIndex) {
    tankActive[tankIndex] = false;
    activeTanksCount--;
    ramboScore += 100;
    ramboTanksDestroyed++;
    
    // Visual feedback
    gotoxy(SCREEN_WIDTH/2 - 4, SCREEN_HEIGHT/2);
    setColor(10);
    cout << "+100 PTS";
    Sleep(100);
}

void handleCrateDestruction(int crateIndex) {
    crateActive[crateIndex] = false;
    activeCratesCount--;
    ramboScore += 50;
    ramboCratesDestroyed++;
    
    // Visual feedback
    gotoxy(SCREEN_WIDTH/2 - 4, SCREEN_HEIGHT/2 + 1);
    setColor(14);
    cout << "+50 PTS";
    Sleep(100);
    
    // Give extra life for every 5 crates
    if (ramboCratesDestroyed % CRATES_FOR_LIFE == 0) {
        ramboLives++;
        gotoxy(SCREEN_WIDTH/2 - 5, SCREEN_HEIGHT/2 + 2);
        setColor(10);
        cout << "EXTRA LIFE!";
        Sleep(300);
    }
}

bool checkLevelUp() {
    return (ramboDistance >= LEVEL_UP_DISTANCE || ramboTanksDestroyed >= LEVEL_UP_TANKS);
}

void drawHUD() {
    gotoxy(0, SCREEN_HEIGHT + 2);
    
    setColor(11);
    cout << "JOHN RAMBO";
    setColor(7);
    cout << " | Player: " << playerName;
    
    cout << "\nScore: ";
    setColor(10);
    cout << ramboScore;
    setColor(7);
    cout << " | Lives: ";
    setColor(12);
    cout << ramboLives;
    setColor(7);
    cout << " | Distance: ";
    setColor(14);
    cout << ramboDistance << "m/" << LEVEL_UP_DISTANCE << "m";
    
    cout << "\nTanks: ";
    setColor(12);
    cout << ramboTanksDestroyed << "/" << LEVEL_UP_TANKS;
    setColor(7);
    cout << " | Crates: ";
    setColor(6);
    cout << ramboCratesDestroyed;
    setColor(7);
    cout << " | Level: ";
    setColor(level2 ? 12 : 10);
    cout << (level2 ? "2" : "1");
    
    cout << "\nSpeed: ";
    setColor(onRoad ? 10 : 7);
    cout << (onRoad ? "FAST" : "NORMAL");
    setColor(7);
    cout << " | Bullets: " << activeBulletsCount;
    
    // Clear any leftover text
    cout << "                        ";
}

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clearScreen() {
    system("cls");
}

void sortHighScores(HighScore scores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                HighScore temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}

void saveHighScore() {
    // Read existing scores
    ifstream inFile("highscores.txt");
    HighScore scores[20];
    int count = 0;
    
    if (inFile.is_open()) {
        while (count < 20 && inFile >> scores[count].name >> scores[count].score) {
            count++;
        }
        inFile.close();
    }
    
    // Add new score
    if (count < 20) {
        scores[count].name = playerName;
        scores[count].score = ramboScore;
        count++;
    } else {
        // Replace lowest score if new score is higher
        int minIndex = 0;
        for (int i = 1; i < count; i++) {
            if (scores[i].score < scores[minIndex].score) {
                minIndex = i;
            }
        }
        if (ramboScore > scores[minIndex].score) {
            scores[minIndex].name = playerName;
            scores[minIndex].score = ramboScore;
        }
    }
    
    // Sort scores
    sortHighScores(scores, count);
    
    // Write back to file (keep top 10)
    ofstream outFile("highscores.txt");
    if (outFile.is_open()) {
        for (int i = 0; i < min(10, count); i++) {
            outFile << scores[i].name << " " << scores[i].score << endl;
        }
        outFile.close();
    }
}

void loadHighScores() {
    ifstream file("highscores.txt");
    if (file.is_open()) {
        HighScore scores[10];
        int count = 0;
        
        cout << "\n=== TOP 10 HIGH SCORES ===\n";
        cout << "--------------------------\n";
        
        while (count < 10 && file >> scores[count].name >> scores[count].score) {
            count++;
        }
        
        for (int i = 0; i < count; i++) {
            cout << setw(2) << i + 1 << ". " << setw(20) << left << scores[i].name 
                 << " " << setw(10) << right << scores[i].score << endl;
        }
        file.close();
    } else {
        cout << "\nNo high scores yet!\n";
    }
}

void gameOverScreen() {
    clearScreen();
    
    // Save score
    saveHighScore();
    
    setColor(12);
    cout << "\n  #############################\n";
    cout << "  #                           #\n";
    cout << "  #       GAME OVER!          #\n";
    cout << "  #                           #\n";
    cout << "  #############################\n\n";
    setColor(7);
    
    cout << "Player: " << playerName << endl;
    cout << "Final Score: " << ramboScore << endl;
    cout << "Distance: " << ramboDistance << "m" << endl;
    cout << "Tanks Destroyed: " << ramboTanksDestroyed << endl;
    cout << "Crates Destroyed: " << ramboCratesDestroyed << endl;
    cout << "Level Reached: " << (level2 ? "2" : "1") << endl;
    
    cout << "\n1. View High Scores\n";
    cout << "2. Exit\n";
    cout << "Choice: ";
    
    char choice;
    cin >> choice;
    
    if (choice == '1') {
        clearScreen();
        loadHighScores();
        cout << "\nPress any key to exit...";
        _getch();
    }
}