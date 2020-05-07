#ifndef GAME_H
#define GAME_H

// The game logic =============================================================
/**
 * Questa sezione definisce la logica del gioco, le operazioni che si possono
 * effettuare ed i dati necessari alla gestione.
 */

// data to be guessed
using guess = char;

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
    NEW_GAME,                                                   ///< just started (no guess so far)
    RUNNING,                                                    ///< started, at least one guess made
    TIMEOUT = 2,                                                ///< time allowed finished
    NO_MORE_GUESSES = 4,                                        ///< no more guesses allowed
    REVEALED = 8,                                               ///< secret has been revealed
    RIGHT_GUESSED = 16,                                         ///< secret has been guessed
    OVER = TIMEOUT | NO_MORE_GUESSES | REVEALED | RIGHT_GUESSED ///< over, for checking purposes
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
 * @brief Get the number of available guesses
 * 
 * @return int the number of available guesses
 */
int getAvailableGuesses(const game &);

/**
 * @brief Get the number of guesses made so far
 * 
 * @return int the number of guesses made so far
 */
int getNumberOfGuesses(const game &);

/**
 * @brief Get the desired guess
 * 
 * @param p the desired guess position (0 <= p < numGuesses)
 * @param guess the retrieved guess
 * @return true if the guess was set
 * @return false otherwise
 */
bool getGuess(const game &, int, guess &);

/**
 * @brief Get the elapsed time since the start of the game
 * 
 * @return double the elapsed time (in seconds)
 */
double getElapsed(const game &);

/**
 * @brief Get the secret (and terminate the game)
 * 
 * @return guess the secret
 */
guess getSecret(game &);

/**
 * @brief Allow a guess to be made
 * 
 * @param s the guess
 * @return true if guess is right
 * @return false if guess is wrong or not allowed
 */
bool checkGuess(game &, guess);

/**
 * @brief update the time elapsed for the game
 * 
 */
void updateElapsed(game &);

#endif