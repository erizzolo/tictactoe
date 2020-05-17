#ifndef GAME_CPP
#define GAME_CPP

#include "game.h"
#include <chrono>

using namespace std;

// for timing
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<double>;
const Clock theClock;

#define MIN_DIM 3                 ///< min board dimension
#define NUM_PLAYERS 2             ///< number of players
#define PLAYER_NONE (NUM_PLAYERS) ///< none of current players

using moves = unsigned int; ///< bitmap for moves
// computed at every new game ...
moves ALL_MOVES, WINNINGS[MAX_DIM + MAX_DIM + 2];
size_t NUM_WINNINGS, NUM_CELLS;
square MIN_CELL, MAX_CELL;

// representation of the game configuration
using config = unsigned int;
// (NUM_CELLS + NUM_CELLS LSBits) = player moves
enum outcome
{
    WINNING, ///< turn player wins
    LOSING,  ///< turn player loses
    DRAW     ///< nobody wins/loses
};

// AI map for 4x4 board
#include <map>
map<config, outcome> results;

/**
 * @brief game representation
 * 
 */
struct game
{
    int DIM{BOARD_DIM};                    ///< dimensione del gioco
    moves done[NUM_PLAYERS]{0, 0};         ///< mosse effettuate
    player turn{rand() % NUM_PLAYERS};     ///< random turn
    player winner{NUM_PLAYERS};            ///< winner
    status state;                          ///< stato del gioco
    double timeAllowed{TIME_ALLOWED};      ///< tempo concesso per le mosse
    TimePoint startTime{theClock.now()};   ///< istante inizio
    Duration elapsed[NUM_PLAYERS]{0s, 0s}; ///< elapsed time
};

void initData(const game &g)
{
    // compute dimensions and moves
    NUM_CELLS = g.DIM * g.DIM;
    MIN_CELL = 0;
    MAX_CELL = MIN_CELL + NUM_CELLS - 1;
    ALL_MOVES = (1 << NUM_CELLS) - 1;
    moves firstCol = 0, mainDiagonal = 0, coDiagonal = 0;
    for (size_t i = 0; i < g.DIM; i++)
    {
        firstCol |= 1 << (i * g.DIM);
        mainDiagonal |= 1 << (i * (g.DIM + 1));
        coDiagonal |= 1 << ((i + 1) * (g.DIM - 1));
        WINNINGS[i] = ((1 << g.DIM) - 1) << (i * g.DIM);
    }
    for (size_t i = 0; i < g.DIM; i++)
    {
        WINNINGS[i + g.DIM] = firstCol << i;
    }
    WINNINGS[g.DIM + g.DIM] = mainDiagonal;
    WINNINGS[g.DIM + g.DIM + 1] = coDiagonal;
    NUM_WINNINGS = g.DIM + g.DIM + 2;
}
/**
 * @brief Get a new game based on configuration
 * 
 * @param c     the application configuration
 * @return game a new game
 */
game newGame(const configuration &c)
{
    game g;
    g.timeAllowed = c.timeAllowed;
    g.DIM = c.boardDim;
    initData(g);
    // results.clear();
    g.state = RUNNING;
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
 * @brief Get the current player
 * 
 * @return current player 
 */
player getTurn(const game &g)
{
    return (getStatus(g) & OVER) == 0 ? g.turn : PLAYER_NONE;
}

/**
 * @brief Get the winner player
 * 
 * @return winner player 
 */
player getWinner(const game &g)
{
    return g.winner;
}

/**
 * @brief Get the player of a cell
 * 
 * @return cell player 
 */
player getCellPlayer(const game &g, square c)
{
    if (MIN_CELL <= c && c <= MAX_CELL)
    {
        moves move = 1 << (c - MIN_CELL);
        for (int p = 0; p < NUM_PLAYERS; p++)
        {
            if ((move & g.done[p]) != 0)
            {
                return p;
            }
        }
    }
    return NUM_PLAYERS;
}

/**
 * @brief Get the elapsed time for the given player
 * 
 * @return double the elapsed time (in seconds)
 */
double getElapsed(const game &g, player p)
{
    if (getStatus(g) == RUNNING)
    {
        if (getTurn(g) == p)
        {
            Duration elapsed = theClock.now() - g.startTime;
            elapsed += g.elapsed[getTurn(g)];
            return elapsed.count() > g.timeAllowed ? g.timeAllowed : elapsed.count();
        }
    }
    if (p < NUM_PLAYERS)
    {
        return g.elapsed[p].count();
    }
    return 0;
}

/**
 * @brief check whether the moves are winning
 * 
 * @param m the moves
 * @return true if winning
 * @return false otherwise
 */
bool isWinning(moves m)
{
    for (size_t i = 0; i < NUM_WINNINGS; i++)
    {
        if ((m & WINNINGS[i]) == WINNINGS[i])
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief return all moves made so far
 * 
 * @param g the game
 * @return moves 
 */
moves allMoves(const game &g)
{
    moves result = 0;
    for (player p = 0; p < NUM_PLAYERS; p++)
    {
        result |= g.done[p];
    }
    return result;
}

// Reverse row tranform assuming DIM = 4
// IN:  3333222211110000 3333222211110000
// OUT: 0000111122223333 0000111122223333
inline config RR4(config c)
{
#define RR4_LOW8_MASK ((config)0x00FF00FF)
#define RR4_HIGH8_MASK ((config)0xFF00FF00)
#define RR4_LOW4_MASK ((config)0x0F0F0F0F)
#define RR4_HIGH4_MASK ((config)0xF0F0F0F0)
    // h:  1111000033332222 1111000033332222
    config h = ((c & RR4_LOW8_MASK) << 8) | ((c & RR4_HIGH8_MASK) >> 8);
    // return:  0000111122223333 0000111122223333
    return ((h & RR4_LOW4_MASK) << 4) | ((h & RR4_HIGH4_MASK) >> 4);
}

// Reverse column tranform assuming DIM = 4
// IN:  3210321032103210 3210321032103210
// OUT: 0123012301230123 0123012301230123
inline config RC4(config c)
{
#define RC4_LOW8_MASK ((config)0x33333333)
#define RC4_HIGH8_MASK ((config)0xCCCCCCCC)
#define RC4_HIGH4_MASK ((config)0xAAAAAAAA)
#define RC4_LOW4_MASK ((config)0x55555555)
    // h:  1032103210321032 1032103210321032
    config h = ((c & RC4_LOW8_MASK) << 2) | ((c & RC4_HIGH8_MASK) >> 2);
    // return:  0123012301230123 0123012301230123
    return ((h & RC4_LOW4_MASK) << 1) | ((h & RC4_HIGH4_MASK) >> 1);
}

// Exchange row/column tranform assuming DIM = 4
// IN:  FEDCBA9876543210 FEDCBA9876543210
// OUT: FB73EA62D951C840 FB73EA62D951C840
inline config X4(config c)
{
#define X4_NO_SHIFT ((config)0x84218421)
#define X4_SHIFT_P3 ((config)0x08420842)
#define X4_SHIFT_P6 ((config)0x00840084)
#define X4_SHIFT_P9 ((config)0x00080008)
#define X4_SHIFT_M3 ((config)0x42104210)
#define X4_SHIFT_M6 ((config)0x21002100)
#define X4_SHIFT_M9 ((config)0x10001000)
    config r = (c & X4_NO_SHIFT) |
               ((c & X4_SHIFT_P3) << 3) |
               ((c & X4_SHIFT_M3) >> 3) |
               ((c & X4_SHIFT_P6) << 6) |
               ((c & X4_SHIFT_M6) >> 6) |
               ((c & X4_SHIFT_P9) << 9) |
               ((c & X4_SHIFT_M9) >> 9);
    return r;
}

void setConfigResult(config c0, outcome o)
{
    results[c0] = o;
    config c1 = X4(c0);
    results[c1] = o;
    config c2 = RR4(c0);
    results[c2] = o;
    config c3 = RR4(c1);
    results[c3] = o;
    config c4 = RC4(c0);
    results[c4] = o;
    config c5 = RC4(c1);
    results[c5] = o;
    config c6 = RC4(c2);
    results[c6] = o;
    config c7 = RC4(c3);
    results[c7] = o;
}
outcome checkConfig4(config cfg, moves all)
{
    // cfg = minConfig(cfg);
    auto chk = results.find(cfg);
    if (chk != results.end())
    {
        return chk->second;
    }
    outcome result = WINNING;
    if (!isWinning(cfg))
    {
        if (all == ALL_MOVES)
        {
            result = DRAW;
        }
        else
        {
            // check other player's moves
            config other = ((cfg & ALL_MOVES) << NUM_CELLS) | (cfg >> NUM_CELLS);
            moves value = 1;
            while (value < ALL_MOVES && result != LOSING)
            {
                if ((value & all) == 0)
                {
                    outcome chk = checkConfig4(other | value, all | value);
                    if (chk == WINNING)
                    {
                        result = LOSING;
                    }
                    else if (chk == DRAW)
                    {
                        result = DRAW;
                    }
                }
                value <<= 1;
            }
        }
    }
    setConfigResult(cfg, result);
    return result;
}
outcome checkConfig(config cfg, moves all)
{
    outcome result = WINNING;
    if (!isWinning(cfg))
    {
        if (all == ALL_MOVES)
        {
            result = DRAW;
        }
        else
        {
            // check other player's moves
            cfg = ((cfg & ALL_MOVES) << NUM_CELLS) | (cfg >> NUM_CELLS);
            moves value = 1;
            while (value < ALL_MOVES && result != LOSING)
            {
                if ((value & all) == 0)
                {
                    outcome chk = checkConfig(cfg | value, all | value);
                    if (chk == WINNING)
                    {
                        result = LOSING;
                    }
                    else if (chk == DRAW)
                    {
                        result = DRAW;
                    }
                }
                value <<= 1;
            }
        }
    }
    return result;
}

square bestMove(const game &g)
{
    config cfg = g.done[getTurn(g)] | (g.done[1 - getTurn(g)] << NUM_CELLS);
    moves all = allMoves(g), value = 1;
    square result = MIN_CELL, current = MIN_CELL;
    if (all == 0 && g.DIM == 3)
    {
        // first move
        result = (rand() % 2) * (g.DIM - 1) + (rand() % 2) * g.DIM * (g.DIM - 1);
    }
    else
    {
        while (current <= MAX_CELL)
        {
            if ((value & all) == 0)
            {
                outcome chk;
                if (g.DIM == 3)
                {
                    chk = checkConfig(cfg | value, all | value);
                }
                else
                {
                    chk = checkConfig4(cfg | value, all | value);
                }
                if (chk == WINNING)
                {
                    return current;
                }
                if (chk == DRAW)
                {
                    result = current;
                }
            }
            value <<= 1;
            current++;
        }
    }
    return result;
}

/**
 * @brief Get a move from the computer
 * 
 * @return square the chosen cell
 */
square getMove(const game &g)
{
    return bestMove(g);
    square result;
    do
    {
        result = MIN_CELL + rand() % NUM_CELLS;
    } while (!isAllowedMove(g, result));
    return result;
}

/**
 * @brief Allow a move to be made
 * 
 * @return true if move has been made
 * @return false if move was not allowed
 */
bool makeMove(game &g, square c)
{
    if (isAllowedMove(g, c))
    {
        player current = getTurn(g);
        g.done[current] |= 1 << (c - MIN_CELL);
        if (isWinning(g.done[current]))
        {
            g.winner = current;
            g.state = ENDED;
            g.turn = PLAYER_NONE;
        }
        else
        {
            if (allMoves(g) == ALL_MOVES)
            {
                g.state = ENDED;
            }
            else
            {
                g.turn++;
                if (g.turn == NUM_PLAYERS)
                {
                    g.turn = 0;
                }
            }
        }
        moveMade(g, c); // notify UI
        if (getStatus(g) != RUNNING)
        {
            gameEnded(g); // notify UI
        }
        return true;
    }
    return false;
}

/**
 * @brief Check if a move is allowed
 * 
 * @return true if move is allowed
 * @return false if move is not allowed
 */
bool isAllowedMove(const game &g, square c)
{
    if (MIN_CELL <= c && c <= MAX_CELL)
    {
        if (getStatus(g) == RUNNING)
        {
            moves move = 1 << (c - MIN_CELL);
            return (allMoves(g) & move) == 0;
        }
    }
    return false;
}

/**
 * @brief update the time elapsed for the game
 * 
 */
void updateElapsed(game &g)
{
    if (getStatus(g) == RUNNING)
    {
        player current = getTurn(g);
        Duration elapsed = theClock.now() - g.startTime;
        g.startTime = theClock.now();
        g.elapsed[current] += elapsed;
        if (g.elapsed[current].count() >= g.timeAllowed)
        {
            g.state = TIMEOUT;
            g.elapsed[current] = Duration(g.timeAllowed);
            if (NUM_PLAYERS == 2)
            {
                g.winner = 1 - current;
            }
            gameEnded(g);
        }
    }
}

/**
 * @brief Initialize AI map
 */
void initAI()
{
    game g;
    g.DIM = 4;
    g.state = RUNNING;
    initData(g);
    square s = getMove(g);
}

#endif