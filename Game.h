#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <windows.h>  // Include for SetConsoleTextAttribute

using namespace std;

// Define the color constants for text output
#define FOREGROUND_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define FOREGROUND_MAGENTA (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FOREGROUND_CYAN (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)

void SetColor(int color);

// Struct Definitions
struct User {
    string name;
    string rollNo;
};

struct HighScore {
    string name;
    string rollNo;
    int score;
};

// Class Declarations
class ScoreTracker {
private:
    int score;

public:
    ScoreTracker();
    void updateScore(int points);
    int getScore() const;
    void resetScore();
    friend ostream& operator<<(ostream& os, const ScoreTracker& tracker);
    ScoreTracker& operator+=(int points);
};

class WordValidator {
public:
    bool isValidWord(string word, string wordList[], int wordCount);
};

class BaseGrid {
protected:
    int size;

public:
    BaseGrid(int gridSize);
    virtual void displayGrid() = 0;
    virtual ~BaseGrid() {}
};

class Grid : public BaseGrid {
private:
    char** grid;

public:
    Grid(int gridSize);
    Grid(const Grid& other);
    ~Grid() override;

    void populateGrid(string words[], int wordCount);
    void displayGrid() override;
    bool validateWord(string word, int row, int col);
    void replaceWordWithStars(string word, int row, int col);
    int getSize() const;
};

class Game {
private:
    Grid* grid;
    ScoreTracker scoreTracker;
    WordValidator validator;
    int lives;
    string wordList[50];
    int wordCount;
    User currentUser;

    void initializeGame(const string& mode, int level);
    void loadWords(const string& fileName);
    void saveHighScore();
    bool playLevel(int level, const string& mode);

public:
    Game();
    ~Game();

    void displayMenu();
    void start();
    void displayInstructions();
    void displayHighestScores();
    void aboutUs();
};

#endif
