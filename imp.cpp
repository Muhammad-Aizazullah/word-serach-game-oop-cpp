#include "Game.h"

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

ScoreTracker::ScoreTracker() : score(0) {}

void ScoreTracker::updateScore(int points) {
    score += points;
}

int ScoreTracker::getScore() const {
    return score;
}

void ScoreTracker::resetScore() {
    score = 0;
}

ostream& operator<<(ostream& os, const ScoreTracker& tracker) {
    os << "Score: " << tracker.score;
    return os;
}

ScoreTracker& ScoreTracker::operator+=(int points) {
    score += points;
    return *this;
}

WordValidator::WordValidator() {}

bool WordValidator::isValidWord(string word, string wordList[], int wordCount) {
    for (int i = 0; i < wordCount; i++) {
        if (wordList[i] == word) {
            return true;
        }
    }
    return false;
}

BaseGrid::BaseGrid(int gridSize) : size(gridSize) {}

BaseGrid::~BaseGrid() {}

Grid::Grid(int gridSize) : BaseGrid(gridSize) {
    grid = new char* [size];
    for (int i = 0; i < size; i++) {
        grid[i] = new char[size];
        for (int j = 0; j < size; j++) {
            grid[i][j] = '.';
        }
    }
}

Grid::Grid(const Grid& other) : BaseGrid(other.size) {
    grid = new char* [size];
    for (int i = 0; i < size; i++) {
        grid[i] = new char[size];
        for (int j = 0; j < size; j++) {
            grid[i][j] = other.grid[i][j];
        }
    }
}

Grid::~Grid() {
    for (int i = 0; i < size; i++) {
        delete[] grid[i];
    }
    delete[] grid;
}

void Grid::populateGrid(string words[], int wordCount) {
    srand(time(0));

    int wordIndex = 0;

    while (wordIndex < wordCount) {
        string word = words[wordIndex];
        int row = rand() % size;
        int col = rand() % size;

        try {
            if (wordIndex % 3 == 0) { // Horizontal placement
                if (col + word.length() > size) throw out_of_range("Word doesn't fit horizontally.");
                for (int i = 0; i < word.length(); i++) {
                    grid[row][col + i] = word[i];
                }
            }
            else if (wordIndex % 3 == 1) { // Vertical placement
                if (row + word.length() > size) throw out_of_range("Word doesn't fit vertically.");
                for (int i = 0; i < word.length(); i++) {
                    grid[row + i][col] = word[i];
                }
            }
            else { // Diagonal placement
                if (row + word.length() > size || col + word.length() > size) {
                    throw out_of_range("Word doesn't fit diagonally.");
                }
                for (int i = 0; i < word.length(); i++) {
                    grid[row + i][col + i] = word[i];
                }
            }

            wordIndex++;
        }
        catch (const out_of_range& e) {
            cout << "Error: " << e.what() << endl;
            continue; // Skip the current placement if it fails and try again
        }
    }

    // Fill remaining empty spaces with random letters
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == '.') {
                grid[i][j] = 'a' + rand() % 26;
            }
        }
    }
}

void Grid::displayGrid() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}

bool Grid::validateWord(string word, int row, int col) {
    for (int i = 0; i < word.length(); i++) {
        if (grid[row][col + i] != word[i]) {
            return false;
        }
    }
    return true;
}

void Grid::replaceWordWithStars(string word, int row, int col) {
    for (int i = 0; i < word.length(); i++) {
        grid[row][col + i] = '*';
    }
}

int Grid::getSize() const {
    return size;
}

Game::Game() : lives(5), grid(nullptr) {}

Game::~Game() {
    delete grid;
}

void Game::initializeGame(const string& mode, int level) {
    int gridSize;
    if (level == 1) {
        gridSize = 10;
    }
    else if (level == 2) {
        gridSize = 15;
    }
    else {
        gridSize = 20;
    }

    grid = new Grid(gridSize);

    string fileName;
    if (mode == "Easy" || mode == "easy" || mode == "EASY" || mode == "1") {
        fileName = "easy_words.txt";
    }
    else if (mode == "Hard" || mode == "hard" || mode == "HARD" || mode == "2") {
        fileName = "hard_words.txt";
    }
    else {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "Invalid input. Kindly input wisely!!" << endl;
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        start();
    }

    loadWords(fileName);
    grid->populateGrid(wordList, wordCount);
}

void Game::loadWords(const string& fileName) {
    ifstream file(fileName);
    wordCount = 0;
    while (file >> wordList[wordCount] && wordCount < 50) {
        wordCount++;
    }
    file.close();
}

void Game::saveHighScore() {
    HighScore highScores[3];
    ifstream inFile("highscore.txt");

    int count = 0;
    while (inFile >> highScores[count].name >> highScores[count].rollNo >> highScores[count].score) {
        count++;
    }
    inFile.close();

    bool scoreAdded = false;
    if (count < 3) {
        highScores[count] = { currentUser.name, currentUser.rollNo, scoreTracker.getScore() };
        scoreAdded = true;
        count++;
    }
    else {
        if (scoreTracker.getScore() > highScores[2].score) {
            highScores[2] = { currentUser.name, currentUser.rollNo, scoreTracker.getScore() };
            scoreAdded = true;
        }
    }

    if (scoreAdded) {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (highScores[j].score < highScores[j + 1].score) {
                    HighScore temp = highScores[j];
                    highScores[j] = highScores[j + 1];
                    highScores[j + 1] = temp;
                }
            }
        }
        ofstream outFile("highscore.txt");
        for (int i = 0; i < count; i++) {
            outFile << highScores[i].name << " " << highScores[i].rollNo << " " << highScores[i].score << endl;
        }
        outFile.close();
    }
}

bool Game::playLevel(int level, const string& mode) {
    initializeGame(mode, level);
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "Level " << level << ": Find the hidden words!" << endl;

    int wordsToFind = (level == 1) ? 3 : (level == 2) ? 5 : 7;

    while (wordsToFind > 0 && lives > 0) {
        system("cls");
        SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        cout << "Player: " << currentUser.name << " | Roll No: " << currentUser.rollNo << endl;
        grid->displayGrid();

        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "Lives left: " << lives << endl;
        cout << scoreTracker << endl;
        cout << "Words remaining to find: " << wordsToFind << endl;
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << "Enter your word: ";
        string word;
        cin >> word;

        bool wordFound = false;
        for (int i = 0; i < wordCount; i++) {
            if (wordList[i] == word) {
                for (int row = 0; row < grid->getSize(); ++row) {
                    for (int col = 0; col < grid->getSize() - word.length(); ++col) {
                        if (grid->validateWord(word, row, col)) {
                            grid->replaceWordWithStars(word, row, col);
                            wordFound = true;
                            break;
                        }
                    }
                    if (wordFound) break;
                }
                break;
            }
        }

        if (wordFound) {
            SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            cout << "Correct! Word found." << endl;
            scoreTracker += 10;
            wordsToFind--;
        }
        else {
            SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            cout << "Incorrect! Try again." << endl;
            lives--;
        }
    }

    if (wordsToFind == 0) {
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << "Level " << level << " Completed!" << endl;
        return true;
    }
    else {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "Game Over. You ran out of lives." << endl;
        return false;
    }
}

void Game::displayMenu() {
    SetColor(FOREGROUND_YELLOW | FOREGROUND_INTENSITY);
    cout << "Kindly select an option" << endl;
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "1. Start Game" << endl;
    cout << "2. Instructions" << endl;
    cout << "3. Highest Scores" << endl;
    cout << "4. About Us" << endl;
    cout << "5. Exit" << endl;
}

void Game::start() {
    int choice;
    do {
        SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        cout << "__________________________________________________" << endl;
        cout << "|                NEW GAME STARTED                |" << endl;
        cout << "__________________________________________________" << endl;
        displayMenu();
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            cout << "Enter your name: ";
            cin.ignore();
            getline(cin, currentUser.name);
            cout << "Enter your roll number: ";
            getline(cin, currentUser.rollNo);

            scoreTracker.resetScore();
            lives = 5;

            SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            cout << "Choose Difficulty level " << endl;
            cout << "1. Easy" << endl;
            cout << "2. Hard" << endl;
            string mode;
            cin >> mode;
            int level = 1;
            while (level <= 3) {
                if (!playLevel(level, mode)) break;
                level++;
            }
            if (level > 3) {
                SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                cout << "Congratulations! You completed all levels!" << endl;
            }
            saveHighScore();
        }
        else if (choice == 2) {
            displayInstructions();
        }
        else if (choice == 3) {
            displayHighestScores();
        }
        else if (choice == 4) {
            aboutUs();
        }
        else if (choice == 5) {
            cout << "Thanks for playing! Goodbye!" << endl;
        }
        else {
            cout << "Invalid choice. Please select again." << endl;
        }

    } while (choice != 5);
}

void Game::displayInstructions() {
    cout << "Instructions: You will be given a grid with some hidden words. Find them!" << endl;
}

void Game::displayHighestScores() {
    cout << "High Scores:" << endl;
    ifstream inFile("highscore.txt");
    HighScore highScores[3];
    int count = 0;
    while (inFile >> highScores[count].name >> highScores[count].rollNo >> highScores[count].score) {
        count++;
    }
    inFile.close();
    for (int i = 0; i < count; i++) {
        cout << highScores[i].name << " | " << highScores[i].rollNo << " | " << highScores[i].score << endl;
    }
}

void Game::aboutUs() {
    cout << "This is a word search game developed by Team XYZ." << endl;
    cout << "We hope you enjoy playing!" << endl;
}
