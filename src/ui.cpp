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
    {TRY, "Make a move"},
    {MOVE, "Select/confirm cell"},
    {SIZE, "Change board dimension"}};

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

// the array of actual translations (to be filled at runtime)
translation *input_action = nullptr;
size_t NUM_TRANSLATIONS = 0;

// prompt for user input
const char INPUT_PROMPT[] = "Enter your choice: ";

// currently selected cell
square selected = 0;
// player names
#define MAX_NAME_LENGTH 25
char names[NUM_PLAYERS][MAX_NAME_LENGTH];

// the windows
/**
 * @brief text and background components of a colour
 * 
 */
struct colour
{
    int text, back; ///< text and background components
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
    int vertical, horizontal; ///< vertical (rows) and horizontal (cols) dimension
};

// max window title length
const int MAX_TITLE_LENGTH = 80;
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

#define MIN_SCREEN_ROWS 22
#define MIN_SCREEN_COLUMNS 90

// the main window
const window mainWindow{{1, 1},
                        {22, 90},
                        {1, 1},
                        {WHITE, BLACK},
                        {BLACK, GREY},
                        "Welcome to the game!!!"};
// the board window (just a border, actually)
const window board{{2, 52},
                   {20, 38},
                   {0, 1},
                   {WHITE, BLACK},
                   {BLACK, GREY},
                   ""};
// the "menu" bar
const window menuBar{{2, 2},
                     {NUM_ACTIONS + 1, 50},
                     {1, 0},
                     {GREEN, BLACK},
                     {BLACK, GREEN},
                     "Available commands"};
// the game info window
const window gameInfo{{10, 2},
                      {3, 50},
                      {0, 0},
                      {YELLOW, BLACK},
                      {YELLOW, BLACK},
                      ""};
// the user input
const window userInput{{13, 2},
                       {1, 50},
                       {0, 0},
                       {YELLOW, BLACK},
                       {YELLOW, BLACK},
                       ""};
// the status bar
const window statusBar{{20, 2},
                       {3, 50},
                       {1, 0},
                       {WHITE, BLACK},
                       {BLACK, GREY},
                       "Status messages"};
// the time elapsed window (MM:SS,mmm)
#define TIME_TOP 14
#define TIME_LEFT 2
#define TIME_DELTA_ROW 2
#define TIME_DELTA_COL 0
const window timeElapsed = {{0, 0}, // corner to be specified
                            {3, 10},
                            {1, 0},
                            {WHITE, BLACK},
                            {BLACK, GREY},
                            "Elapsed"};

const window playerTimeElapsed(player p)
{
    window w = timeElapsed;
    w.corner.horizontal = TIME_LEFT + p * TIME_DELTA_COL;
    w.corner.vertical = TIME_TOP + p * TIME_DELTA_ROW;
    return w;
}
// the progress bar
#define PROGRESS_TOP 14
#define PROGRESS_LEFT 12
#define PROGRESS_DELTA_ROW 2
#define PROGRESS_DELTA_COL 0
const window progressBar = {{0, 0}, // corner to be specified
                            {3, 40},
                            {1, 0},
                            {WHITE, GREEN},
                            {BLACK, GREY},
                            "Time remaining"};

const window playerProgressBar(player p)
{
    window w = progressBar;
    w.corner.horizontal = PROGRESS_LEFT + p * PROGRESS_DELTA_COL;
    w.corner.vertical = PROGRESS_TOP + p * PROGRESS_DELTA_ROW;
    sprintf(w.title, "Time remaining for %c: %s", "XO"[p], names[p]);
    return w;
}
// a specific "window" to show a square
#define CELL_TOP 2
#define CELL_LEFT 53
#define CELL_ROWS 3
#define CELL_COLUMNS 5
#define CELL_ROW_DIM 5
#define CELL_COLUMN_DIM 9
window cell{{0, 0}, // corner to be specified
            {CELL_ROW_DIM, CELL_COLUMN_DIM},
            {(CELL_ROW_DIM - CELL_ROWS) / 2, (CELL_COLUMN_DIM - CELL_COLUMNS) / 2},
            {WHITE, BLACK}, // to be changed
            {BLACK, GREY},  // to be changed
            ""};

// assumes NUM_PLAYERS = 2 !!!
const char PLAYERS[NUM_PLAYERS][CELL_ROWS][CELL_COLUMNS + 1] = {{"\\\\ //",
                                                                 " XXX ",
                                                                 "// \\\\"},
                                                                {" OOO ",
                                                                 "O   O",
                                                                 " OOO "}};

const colour cellSelected{BLACK, BROWN};
const colour cellNormal{BLACK, BLUE};

const char CELL_SYMBOLS[] = "123456789ABCDEFG";
char symbolForCell(square c)
{
    return CELL_SYMBOLS[c];
}
square cellWithSymbol(char c)
{
    if ('a' <= c && c <= 'z')
    {
        c += 'A' - 'a';
    }
    for (square i = 0; i < NUM_CELLS; i++)
    {
        if (CELL_SYMBOLS[i] == c)
        {
            return i;
        }
    }
    return NUM_CELLS;
}

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

bool checkScreenSize()
{
    return tcols() >= MIN_SCREEN_COLUMNS && trows() >= MIN_SCREEN_ROWS;
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
}

/**
 * @brief print a square cell
 * 
 * @param g the game
 * @param what the colour
 * @param which the square to show
 */
void printCell(const game &g, colour what, square which)
{
    player p = getCellPlayer(g, which);
    which -= MIN_CELL;
    cell.corner.horizontal = CELL_LEFT + cell.size.horizontal * (which % g.DIM);
    cell.corner.vertical = CELL_TOP + cell.size.vertical * (which / g.DIM);
    cell.frame = what;
    paint(cell);
    if (p == PLAYER_NONE)
    {
        string space(CELL_COLUMNS / 2, ' ');
        printText(cell, space.c_str(), 1, false);
        cout << symbolForCell(which + MIN_CELL);
    }
    else
    {
        for (size_t i = 0; i < CELL_ROWS; i++)
        {
            printText(cell, PLAYERS[p][i], i, false);
        }
    }
    cout.flush();
}

/**
 * @brief nasconde schermata di benvenuto
 * 
 */
void hideWelcomeScreen()
{
    if (!checkScreenSize())
    {
        printText(mainWindow, "", 2, false);
        cout << "Please ensure screen size is at least " << MIN_SCREEN_ROWS << " x " << MIN_SCREEN_COLUMNS << "!";
        printText(mainWindow, "", 4, false);
        cout << "Press any key when ready...";
        cout.flush();
        while (!kbhit())
        {
            msleep(100);
        }
        getkey();
        if (!checkScreenSize())
        {
            printText(mainWindow, "", 2, true);
            cout << "Sorry... exiting!";
            cout.flush();
            msleep(100);
            exit(1);
        }
    }
    paint(mainWindow);
    paint(statusBar);
    paint(board);
    printText(statusBar, "Let's start!!!");
    msleep(500);
    paint(menuBar);
    paint(gameInfo);
    paint(userInput);
    for (player p = 0; p < NUM_PLAYERS; p++)
    {
        printText(userInput, "Name of player ");
        cout << (p == 0 ? "X [" : "O [") << (1 + p) << "] (blank = AI):";
        cin.getline(names[p], MAX_NAME_LENGTH);
        // cin.ignore();
    }
    printText(userInput, INPUT_PROMPT);
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
    for (player p = 0; p < NUM_PLAYERS; p++)
    {
        double time = getElapsed(g, (player)p);
        int minutes = time / 60;
        int seconds = time - minutes * 60;
        int millis = (time - minutes * 60 - seconds) * 1000;
        printText(playerTimeElapsed(p), "");
        cout << (minutes / 10) << (minutes % 10) << ":";
        cout << (seconds / 10) << (seconds % 10) << ",";
        cout << (millis / 100) << ((millis / 10) % 10) << (millis % 10);
        double fractionElapsed = time / g.timeAllowed;
        int length = ((progressBar.size.horizontal - 2 * progressBar.border.horizontal) * fractionElapsed) + 0.5;
        printText(playerProgressBar(p), "", 0, false);
        setBackgroundColor(RED);
        for (int i = 0; i < length; ++i)
        {
            cout << " ";
        }
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
    input what = 0;
    if (getStatus(g) == RUNNING && strlen(names[getTurn(g)]) == 0)
    {
        // computer moves
        what = symbolForCell(getMove(g));
    }
    else
    {
        locate(userInput.corner.horizontal + userInput.border.horizontal + strlen(INPUT_PROMPT), userInput.corner.vertical + userInput.border.vertical);
        if (kbhit())
        {
            what = getkey(); // non blocking
        }
        // allow lowercase commands
        if ('a' <= what && what <= 'z')
        {
            what += 'A' - 'a';
        }
    }
    // translation
    return translateInputToAction(what);
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
        if (getStatus(g) == RUNNING)
        {
            updateTime(g);
        }
        nextExec = theClock.now();
    }
}

// 2. just when something new happens: in the following functions

void setUpTranslations(const game &g)
{
    static const size_t FIXED_ACTIONS = 9;
    delete[] input_action;
    input_action = new translation[FIXED_ACTIONS + NUM_CELLS];
    if (input_action)
    {
        input_action[0] = translation{'N', "N", {NEW, PARAM_NONE}};
        input_action[1] = translation{'X', "X", {EXIT, PARAM_NONE}};
        input_action[2] = translation{KEY_ENTER, "Enter", {MOVE, 0}};
        input_action[3] = translation{KEY_LEFT, "Left", {MOVE, -1}};
        input_action[4] = translation{KEY_RIGHT, "Right", {MOVE, +1}};
        input_action[5] = translation{KEY_UP, "Up", {MOVE, -g.DIM}};
        input_action[6] = translation{KEY_DOWN, "Down", {MOVE, +g.DIM}};
        input_action[7] = translation{'+', "+", {SIZE, +1}};
        input_action[8] = translation{'-', "-", {SIZE, -1}};
        NUM_TRANSLATIONS = FIXED_ACTIONS;
        for (square c = MIN_CELL; c <= MAX_CELL; c++)
        {
            input_action[NUM_TRANSLATIONS] = translation{symbolForCell(c), {symbolForCell(c), '\0'}, {TRY, c}};
            NUM_TRANSLATIONS++;
        }
    }
    else
    {
        NUM_TRANSLATIONS = 0;
    }
}
/**
 * @brief called when a game is started
 * 
 */
void gameStarted(const game &g)
{
    statusMsg("New game ...");
    setUpTranslations(g);
    showAvailableCommands(g);
    for (player p = 0; p < NUM_PLAYERS; p++)
    {
        paint(playerTimeElapsed(p));
        paint(playerProgressBar(p));
    }
    paint(board);
    for (square c = MIN_CELL; c <= MAX_CELL; ++c)
    {
        printCell(g, cellNormal, c);
    }
    selected = MIN_CELL;
    printCell(g, cellSelected, selected);
    printText(gameInfo, "Turn of ");
    cout << (getTurn(g) == 0 ? "X: " : "O: ") << names[getTurn(g)];
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
    if (getStatus(g) == ENDED)
    {
        if (getWinner(g) != PLAYER_NONE)
        {
            printText(gameInfo, names[getWinner(g)]);
        }
        else
        {
            printText(gameInfo, "Nobody ");
        }
        cout << " wins!";
    }
    else
    {
        printText(gameInfo, names[1 - getWinner(g)]);
        cout << " runned out of time!";
    }
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
 * @brief called when selected square changes
 * 
 * @param prev previous selection
 * @param current current selection
 */
void cellaChanged(const game &g, square prev, square current)
{
    printCell(g, cellNormal, prev);
    printCell(g, cellSelected, current);
}

/**
 * @brief called when selected square changes
 * 
 * @param g 
 * @param amount 
 */
void changeSelection(game &g, int amount)
{
    if (amount == 0)
    {
        makeMove(g, selected);
    }
    else
    {
        square prevSelected = selected;
        selected += amount;
        if (selected < MIN_CELL)
        {
            selected += NUM_CELLS;
        }
        else if (selected > MAX_CELL)
        {
            selected -= NUM_CELLS;
        }
        if (prevSelected != selected)
        {
            cellaChanged(g, prevSelected, selected);
        }
    }
}
/**
 * @brief called when a square is checked
 * 
 */
void moveMade(const game &g, square c)
{
    cellaChanged(g, selected, c);
    selected = c;
    // showAvailableCommands(g);
    printText(gameInfo, "Turn of ");
    cout << (getTurn(g) == 0 ? "X: " : "O: ") << names[getTurn(g)];
    cout.flush();
}

#endif