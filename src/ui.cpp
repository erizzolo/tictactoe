#ifndef UI_CPP
#define UI_CPP

#include "rlutil.h"
using namespace rlutil;
#include <cstring>

// user input data type (in this case, an int such as from getkey())
using input = int;
// max command description length
const int MAX_CMD_LENGTH = 25;

/**
 * @brief the UI command data type
 * 
 * command action code and action description
 */
struct UIcommand
{
    action_code code;                     // associated action
    char description[MAX_CMD_LENGTH + 1]; // description
};

// the (file global) array of all commands, filled at compile time
const UIcommand UIcommands[NUM_ACTIONS] = {
    {NONE, ""},
    {EXIT, "Exit"},
    {NEW, "New game"},
    {TRY, "Make a guess"},
    {MOVE, "Select/confirm guess"},
    {SHOW, "Show the secret"}};
/**
 * @brief translation between user input and actions data type
 * 
 */
struct translation
{
    input what;     // what the user "types"
    char desc[10];  // description such as "Esc"
    action meaning; // what it means
};

// the array of actual translations (might be filled at runtime)
const translation input_action[] = {
    {'N', "N", {NEW, PARAM_NONE}},
    {'n', "n", {NEW, PARAM_NONE}},
    {'X', "X", {EXIT, PARAM_NONE}},
    {'x', "x", {EXIT, PARAM_NONE}},
    {'0', "0", {TRY, '0'}},
    {'1', "1", {TRY, '1'}},
    {'2', "2", {TRY, '2'}},
    {'3', "3", {TRY, '3'}},
    {'4', "4", {TRY, '4'}},
    {'5', "5", {TRY, '5'}},
    {'6', "6", {TRY, '6'}},
    {'7', "7", {TRY, '7'}},
    {'8', "8", {TRY, '8'}},
    {'9', "9", {TRY, '9'}},
    {'S', "S", {SHOW, PARAM_NONE}},
    {'s', "s", {SHOW, PARAM_NONE}},
    {KEY_ENTER, "Enter", {MOVE, 0}},
    {KEY_LEFT, "Left", {MOVE, -1}},
    {KEY_RIGHT, "Right", {MOVE, +1}},
    {KEY_UP, "Up", {MOVE, -5}},
    {KEY_DOWN, "Down", {MOVE, +5}}};

const int NUM_TRANSLATIONS = sizeof(input_action) / sizeof(input_action[0]);

// prompt for user input
const char INPUT_PROMPT[] = "Enter your choice or choose from below: ";

// the windows
/**
 * @brief text and background components of a colour
 * 
 */
struct colour
{
    unsigned char text, back; ///< text and background components
};

void setColour(const colour c) ///< sets colour
{
    setColor(c.text);
    setBackgroundColor(c.back);
}

/**
 * @brief rectangular dimensions/positions on the screen
 * 
 */
struct dimension
{
    unsigned char vertical, horizontal; ///< vertical (rows) and horizontal (cols) dimension
};

// max window title length
const int MAX_TITLE_LENGTH = 30;
/**
 * @brief Representation of a generic "window" on the screen
 * 
 */
struct window
{
    dimension corner;                 ///< corner top (row) & left (col) coordinates
    dimension size;                   ///< total size (border included)
    dimension border;                 ///< border size
    colour content, frame;            ///< colours of content and border area
    char title[MAX_TITLE_LENGTH + 1]; ///< title
};

// functions to paint a window, clear content, ...

// the main window
const window mainWindow{{2, 2},
                        {trows() - 4, tcols() - 2},
                        {1, 1},
                        {WHITE, BLACK},
                        {BLACK, GREY},
                        "Welcome to the game!!!"};
// the "menu" bar
const window menuBar{{3, 3},
                     {NUM_ACTIONS + 1, tcols() - 4},
                     {1, 0},
                     {GREEN, BLACK},
                     {BLACK, GREEN},
                     "Available commands"};
// the game info window
const window gameInfo{{10, 3},
                      {3, tcols() - 4},
                      {0, 0},
                      {YELLOW, BLACK},
                      {YELLOW, BLACK},
                      ""};
// the user input
const window userInput{{13, 5},
                       {1, 20},
                       {0, 0},
                       {YELLOW, BLACK},
                       {YELLOW, BLACK},
                       ""};
// the status bar
const window statusBar{{trows() - 2, 2},
                       {3, tcols() - 2},
                       {1, 1},
                       {WHITE, BLACK},
                       {BLACK, GREY},
                       "Status messages"};
// the time elapsed window (MM:SS,mmm)
const window timeElapsed{{19, 3},
                         {3, 12},
                         {1, 1},
                         {WHITE, BLACK},
                         {BLACK, GREY},
                         "Time Elapsed"};
// the progress bar
const window progressBar{{19, 15},
                         {3, tcols() - 16},
                         {1, 1},
                         {WHITE, GREEN},
                         {BLACK, GREY},
                         "Time remaining"};
// a specific "window" to show a guess
window cell{{14, 0}, // column to be specified
            {5, 9},
            {1, 2},
            {WHITE, BLACK}, // to be changed
            {BLACK, GREY},  // to be changed
            ""};

const colour cellSelected{BLACK, BROWN};
const colour cellNormal{BLACK, BLUE};
const colour cellWrong{BLACK, RED};
const colour cellRight{BLACK, RED};

// currently selected cell
int selected = 0;

// UI functions
// functions to paint a window, clear content, ...

/**
 * @brief clear the content area of a window
 * 
 */
void clear(const window &w)
{
    string line(w.size.horizontal - 2 * w.border.horizontal, ' ');
    setColour(w.content);
    for (int row = w.border.vertical; row < w.size.vertical - w.border.vertical; ++row)
    {
        locate(w.corner.horizontal + w.border.horizontal, row + w.corner.vertical);
        cout << line;
    }
    cout.flush();
}

/**
 * @brief paint a window (border, title ...)
 * 
 */
void paint(const window &w)
{
    string top(w.size.horizontal, ' ');
    string left(w.border.horizontal, ' ');
    setColour(w.frame);
    for (int row = 0; row < w.size.vertical; ++row)
    {
        locate(w.corner.horizontal, row + w.corner.vertical);
        if (row < w.border.vertical || row + w.border.vertical >= w.size.vertical)
        {
            cout << top;
        }
        else
        {
            cout << left;
            locate(w.corner.horizontal + w.size.horizontal - w.border.horizontal, row + w.corner.vertical);
            cout << left;
        }
    }
    locate(w.corner.horizontal + (w.size.horizontal - string(w.title).length()) / 2, w.corner.vertical);
    cout << w.title;
    clear(w);
}

/**
 * @brief print a message in a window
 * 
 * @param msg   the message to be printed
 * @param row   the initial row (relative to the content area)
 * @param cls   whether to clear the window before printing
 */
void printText(const window &w, const char msg[], int row = 0, bool cls = true)
{
    if (cls)
    {
        clear(w);
    }
    locate(w.corner.horizontal + w.border.horizontal, w.corner.vertical + w.border.vertical + row);
    setColour(w.content);
    cout << msg;
    cout.flush();
}

/**
 * @brief Mostra schermata di benvenuto
 * 
 */
void showWelcomeScreen()
{
    cls();
    paint(mainWindow);
    paint(statusBar);
    // printText(mainWindow, "This is the main window", 15);
}

/**
 * @brief print a guess cell
 * 
 * @param what the colour
 * @param which which guess to show
 */
void printCell(colour what, int which)
{
    cell.corner.horizontal = 6 + cell.size.horizontal * which;
    cell.frame = what;
    paint(cell);
    printText(cell, "  ", 1, false);
    cout << (guess)('0' + which);
    cout.flush();
}

/**
 * @brief nasconde schermata di benvenuto
 * 
 */
void hideWelcomeScreen()
{
    msleep(500);
    printText(statusBar, "Let's start!!!");
    msleep(500);
    paint(menuBar);
    paint(gameInfo);
    paint(userInput);
    printText(userInput, INPUT_PROMPT);
    paint(timeElapsed);
    for (int selected = 0; selected <= '9' - '0'; ++selected)
    {
        printCell(cellNormal, selected);
    }
    printCell(cellSelected, selected);
    hidecursor();
}

/**
 * @brief Mostra schermata di addio
 * 
 */
void showFarewellScreen()
{
    msleep(500);
    printText(statusBar, "Bye Bye!!!");
    msleep(500);
    cls();
    showcursor();
}

/**
 * @brief updates time elapsed and progress bar
 * 
 * @param g 
 */
void updateTime(const game &g)
{
    double time = getElapsed(g);
    int minutes = time / 60;
    int seconds = time - minutes * 60;
    int millis = (time - minutes * 60 - seconds) * 1000;
    printText(timeElapsed, "");
    cout << (minutes / 10) << (minutes % 10) << ":";
    cout << (seconds / 10) << (seconds % 10) << ",";
    cout << (millis / 100) << ((millis / 10) % 10) << (millis % 10) << " ";
    cout.flush();
    double fractionElapsed = time / g.timeAllowed;
    int length = ((progressBar.size.horizontal - 2 * progressBar.border.horizontal) * fractionElapsed) + 0.5;
    printText(progressBar, "", 0, false);
    setBackgroundColor(RED);
    for (int i = 0; i < length; ++i)
    {
        cout << " ";
    }
    cout.flush();
}

/**
 * @brief show available commands
 * 
 * @param g 
 */
void showAvailableCommands(const game &g)
{
    for (int a = NONE + 1, row = 0; a < NUM_ACTIONS; a++, row++)
    {
        printText(menuBar, isEnabled((action_code)a, g) ? " " : "[", row, row == 0);
        for (int i = 0; i < NUM_TRANSLATIONS; ++i)
        {
            if (input_action[i].meaning.code == a)
            {
                cout << " " << input_action[i].desc;
            }
        }
        cout << ": " << UIcommands[a].description << (isEnabled((action_code)a, g) ? "" : " ]");
    }
    cout.flush();
}

/**
 * @brief translates the given input to an action
 * 
 * @param what the input to be translated
 * @return action the translation (NONE if unknown input)
 */
action translateInputToAction(const input what)
{
    for (int i = 0; i < NUM_TRANSLATIONS; ++i)
    {
        if (what == input_action[i].what)
        {
            return input_action[i].meaning;
        }
    }
    return {NONE, PARAM_NONE};
}

/**
 * @brief Get the User command
 * 
 * @return action the action chosen by the user
 */
action getUserAction(const game &g)
{
    locate(userInput.corner.horizontal + userInput.border.horizontal + strlen(INPUT_PROMPT), userInput.corner.vertical + userInput.border.vertical);
    input what = 0;
    if (kbhit())
    {
        what = getkey(); // non blocking
    }
    // translation
    return translateInputToAction(what);
}

/**
 * @brief Show guesses
 * 
 * @param g 
 */
void showGuesses(const game &g)
{
    printText(gameInfo, "Remaining guesses: ");
    cout << getAvailableGuesses(g);
    printText(gameInfo, "Previous guesses: ", 1, false);
    for (int i = 0; i < getNumberOfGuesses(g); ++i)
    {
        guess what;
        getGuess(g, i, what);
        cout << what << " ";
    }
}

/**
 * @brief aggiorna schermata (completa)
 * 
 */
void updateView(const game &g)
{
    static const Duration execInterval = 50ms; // 1/20 s
    static TimePoint nextExec = theClock.now();
    Duration interval = theClock.now() - nextExec;
    if (interval > execInterval)
    {
        if ((getStatus(g) & OVER) == 0)
        {
            updateTime(g);
        }
        nextExec = theClock.now();
    }
}

// 2. just when something new happens: in the following functions

/**
 * @brief called when a game is started
 * 
 */
void gameStarted(const game &g)
{
    showGuesses(g);
    cout << "New game!!! You haven't guessed yet.";
    statusMsg("New game ...");
    showAvailableCommands(g);
    paint(progressBar);
}
/**
 * @brief called when a game is over
 * 
 */
void gameEnded(const game &g)
{
    statusMsg("Game over!!!");
    updateTime(g);
    showAvailableCommands(g);
}
/**
 * @brief called when the secret is shown
 * 
 */
void secretShown(const game &g)
{
    printText(gameInfo, "The secret to be guessed was ", 2, false);
    cout << g.secret << "." << endl;
    showAvailableCommands(g);
}

/**
 * @brief utility function to show a message from application
 * 
 * @param msg 
 */
void statusMsg(const char msg[])
{
    printText(statusBar, msg);
}

/**
 * @brief called when selected guess changes
 * 
 * @param prev previous selection
 * @param current current selection
 */
void guessChanged(const game &, guess prev, guess current)
{
    printCell(cellNormal, prev);
    printCell(cellSelected, current);
}

/**
 * @brief called when selected guess changes
 * 
 * @param g 
 * @param amount 
 */
void changeSelection(game &g, int amount)
{
    if (amount == 0)
    {
        checkGuess(g, '0' + selected);
    }
    else
    {
        int prevSelected = selected;
        selected += amount;
        if (selected < 0)
        {
            selected += '9' - '0' + 1;
        }
        else if (selected > '9' - '0')
        {
            selected -= '9' - '0' + 1;
        }
        if (prevSelected != selected)
        {
            guessChanged(g, prevSelected, selected);
        }
    }
}
/**
 * @brief called when a guess is checked
 * 
 */
void guessChecked(const game &g)
{
    showGuesses(g);
    guess last;
    getGuess(g, getNumberOfGuesses(g) - 1, last);
    guessChanged(g, selected, last - '0');
    selected = last - '0';
    if (getStatus(g) == RIGHT_GUESSED)
    {
        printText(gameInfo, "You guessed right at ", 2, false);
        cout << getNumberOfGuesses(g) << "° guess.";
    }
    else
    {
        printText(gameInfo, "Your ", 2, false);
        cout << getNumberOfGuesses(g) << "° guess is wrong.";
    }
    showAvailableCommands(g);
}

#endif