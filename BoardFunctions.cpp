#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Cell.h"
#include "Board.h"

using namespace std;

typedef Cell::Content mineType;
typedef Cell::State cellState;

// Constructor
Board::Board(int rows, int cols) : rows(rows), cols(cols) {
    // Initialize the board
    for (int i = 0; i < rows; ++i) {
        vector<Cell> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(Cell());
        }
        board.push_back(row);
    }
}

// Constructor with cell size
Board::Board(int rows, int cols, int cellSize) : rows(rows), cols(cols), CELL_SIZE(cellSize) {
    // Initialize the board
    for (int i = 0; i < rows; ++i) {
        vector<Cell> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(Cell());
        }
        board.push_back(row);
    }
}

// Function to draw the board and update the window
void Board::updateBoard(
    sf::RenderWindow* window
) {
    // Draw the board
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(j * CELL_SIZE + cols * CELL_SIZE + CELL_SIZE + 1, i * CELL_SIZE + 1); // Slight padding

            if (board[i][j].getContent() == mineType::Mine) {
                cell.setFillColor(colors.mineColor); // Mine
            } else {
                cell.setFillColor(colors.emptyColor); // Empty
            }

            window->draw(cell);
        }
    }
    // Draw the user board
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(j * CELL_SIZE + 1, i * CELL_SIZE + 1); // Slight padding

            if (board[i][j].getState() == cellState::Flagged) {
                cell.setFillColor(colors.flagColor); // Flagged
            } else if (board[i][j].getState() == cellState::Revealed) {
                if (board[i][j].getContent() == mineType::Mine) {
                    cell.setFillColor(colors.mineColor); // Mine
                } else {
                    cell.setFillColor(colors.safeColor); // Safe
                }
            } else {
                cell.setFillColor(colors.emptyColor); // Not revealed
            }

            window->draw(cell);
        }
    }
    // Display the board
    window->display();
}


// Function to place mines randomly on the board
void Board::placeMines(int mineCount) {
    srand(time(0));
    while (mineCount > 0) {
        int r = rand() % rows;
        int c = rand() % cols;
        if (board[r][c].getContent() == mineType::Empty) {
            board[r][c].setContent(mineType::Mine);
            --mineCount;
        }
    }
}

void Board::handleLeftClick(const sf::Vector2i &mousePosition, sf::RenderWindow* window){
    // Convert mouse position to grid coordinates
    int col = mousePosition.x / CELL_SIZE;
    int row = mousePosition.y / CELL_SIZE;

    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        // Check the content of the clicked cell
        if (board[row][col].getState() == cellState::Hidden){
            if (board[row][col].getContent() == mineType::Mine) {
                sf::RenderWindow gameOverWindow(sf::VideoMode(200, 100), "Game Over!");
                sf::Font font;
                if (!font.loadFromFile("DejaVuSans.ttf")) {
                    cerr << "Failed to load font!" << endl;
                    return;
                }
                sf::Text text("Game Over!", font, 24);
                text.setFillColor(sf::Color::Red);
                text.setPosition(50, 30);

                while (gameOverWindow.isOpen()) {
                    sf::Event event;
                    while (gameOverWindow.pollEvent(event)) {
                        if (event.type == sf::Event::Closed) {
                            gameOverWindow.close();
                            window->close();
                        }
                    }

                    gameOverWindow.clear(sf::Color::Black);
                    gameOverWindow.draw(text);
                    gameOverWindow.display();
                }
            } else {
                // Reveal the cell
                board[row][col].setState(cellState::Revealed);
                // Continue revealing neigh
            }
        }
    }
}

void Board::handleRightClick(const sf::Vector2i &mousePosition, sf::RenderWindow* window){
    // Convert mouse position to grid coordinates
    int col = mousePosition.x / CELL_SIZE;
    int row = mousePosition.y / CELL_SIZE;

    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        // Check the content of the clicked cell
        if (board[row][col].getState() == cellState::Hidden) {
            // Flag the cell
            board[row][col].setState(cellState::Flagged);
        } else if (board[row][col].getState() == cellState::Flagged) {
            // Unflag the cell
            board[row][col].setState(cellState::Hidden);
        }
    }
}