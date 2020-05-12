#ifndef GAME_H
#define GAME_H

// The game logic =============================================================
/**
 * Questa sezione definisce la logica del gioco, le operazioni che si possono
 * effettuare ed i dati necessari alla gestione.
 */

// the cell
using square = char;
// the player type
using player = unsigned int;

#define MAX_DIM 4                 ///< max board dimension

/**
 * @brief game representation (to be defined in game.cpp)
 * 
 */
struct game;

/**
 * @brief possible game status
 * 
 */
enum status
{
    RUNNING,               ///< game is running
    TIMEOUT = 1,           ///< player timeout
    ENDED = 2,             ///< normal end
    OVER = TIMEOUT | ENDED ///< over, for checking purposes
};

// actions available on the game (known to the application and the user interface)

/**
 * @brief Get a new game based on configuration
 * 
 * @return game a new game
 */
game newGame(const configuration &);

/**
 * @brief Get the status of the game
 * 
 * @return status the status of the game
 */
status getStatus(const game &);

/**
 * @brief Get the current player
 * 
 * @return current player 
 */
player getTurn(const game &);

/**
 * @brief Get the winner player
 * 
 * @return winner player 
 */
player getWinner(const game &);

/**
 * @brief Get the player of a cell
 * 
 * @return cell player 
 */
player getCellPlayer(const game &, square);

/**
 * @brief Get the elapsed time for the given player
 * 
 * @return double the elapsed time (in seconds)
 */
double getElapsed(const game &, player);

/**
 * @brief Get a move from the computer
 * 
 * @return square the chosen cell
 */
square getMove(const game &);

/**
 * @brief Allow a move to be made
 * 
 * @return true if move has been made
 * @return false if move was not allowed
 */
bool makeMove(game &, square);

/**
 * @brief Check if a move is allowed
 * 
 * @return true if move is allowed
 * @return false if move is not allowed
 */
bool isAllowedMove(const game &, square);

/**
 * @brief update the time elapsed for the game
 * 
 */
void updateElapsed(game &);

#endif