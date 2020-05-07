#ifndef GAME_CPP
#define GAME_CPP

#include <chrono>

using namespace std;

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<double>;

const Clock theClock;

/**
 * @brief game representation
 * 
 */
struct game
{
    guess secret;                        ///< il segreto da indovinare
    int maxGuesses{MAX_GUESSES};         ///< il numero di tentativi concessi
    int numGuesses{0};                   ///< il numero di tentativi effettuati
    guess guesses[MAX_GUESSES];          ///< i tentativi effettuati
    status state{NEW_GAME};              ///< stato del gioco
    double timeAllowed{TIME_ALLOWED};    ///< tempo concesso per indovinare
    TimePoint startTime{theClock.now()}; ///< istante inizio
    Duration elapsed{0};                 ///< elapsed time ( 0 = running )
};

/**
 * @brief Get a new game based on configuration
 * 
 * @param c     the application configuration
 * @return game a new game
 */
game newGame(const configuration &c)
{
    game g;
    g.maxGuesses = c.maxGuesses;
    g.timeAllowed = c.timeAllowed;
    g.secret = '0' + rand() % ('9' - '0' + 1);
    gameStarted(g); // notify UI
    return g;
}

/**
 * @brief Get the status of the game
 * 
 * @return status the status of the game
 */
status getStatus(const game &g)
{
    return g.state;
}

/**
 * @brief Get the number of available guesses
 * 
 * @return int the number of available guesses
 */
int getAvailableGuesses(const game &g)
{
    return (g.state & OVER) == 0 ? g.maxGuesses - g.numGuesses : 0;
}

/**
 * @brief Get the number of guesses made so far
 * 
 * @return int the number of guesses made so far
 */
int getNumberOfGuesses(const game &g)
{
    return g.numGuesses;
}

/**
 * @brief Get the desired guess
 * 
 * @param p the desired guess position (0 <= p < numGuesses)
 * @param guess the retrieved guess
 * @return true if the guess was set
 * @return false otherwise
 */
bool getGuess(const game &g, int p, guess &guess)
{
    if (p >= 0 && p < g.numGuesses)
    {
        guess = g.guesses[p];
        return true;
    }
    return false;
}

/**
 * @brief Get the elapsed time since the start of the game
 * 
 * @return double the elapsed time (in seconds)
 */
double getElapsed(const game &g)
{
    if ((g.state & OVER) == 0)
    {
        Duration elapsed = theClock.now() - g.startTime;
        return elapsed.count() > g.timeAllowed ? g.timeAllowed : elapsed.count();
    }
    return g.elapsed.count();
}

/**
 * @brief Get the secret (and terminate the game)
 * 
 * @return guess the secret
 */
guess getSecret(game &g)
{
    if ((getStatus(g) & (REVEALED | RIGHT_GUESSED)) == 0)
    {
        if ((getStatus(g) & OVER) == 0)
        {
            g.state = REVEALED;
            g.elapsed = theClock.now() - g.startTime;
            gameEnded(g); // notify UI
        }
        secretShown(g); // notify UI
    }
    return g.secret;
}

/**
 * @brief Allow a guess to be made
 * 
 * @param s the guess
 * @return true if guess is right
 * @return false if guess is wrong or not allowed
 */
bool checkGuess(game &g, guess s)
{
    bool result = false;
    if ((g.state & OVER) == 0)
    {
        g.state = RUNNING;
        g.guesses[g.numGuesses] = s;
        g.numGuesses++;
        result = s == g.secret;
        if (result)
        {
            g.state = RIGHT_GUESSED;
        }
        else if (getAvailableGuesses(g) == 0)
        {
            g.state = NO_MORE_GUESSES;
        }
        guessChecked(g);
        if ((g.state & OVER) != 0)
        {
            g.elapsed = theClock.now() - g.startTime;
            gameEnded(g); // notify UI
        }
    }
    return result;
}

/**
 * @brief update the time elapsed for the game
 * 
 */
void updateElapsed(game &g)
{
    if ((g.state & OVER) == 0)
    {
        g.elapsed = theClock.now() - g.startTime;
        if (g.elapsed.count() >= g.timeAllowed)
        {
            g.state = TIMEOUT;
            g.elapsed = Duration(g.timeAllowed);
            gameEnded(g);
        }
    }
}

#endif