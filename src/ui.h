#ifndef UI_H
#define UI_H

// The UI logic ================================================================

// functions of the UI, called by the application logic

/**
 * @brief Mostra schermata di benvenuto
 * 
 */
void showWelcomeScreen();

/**
 * @brief nasconde schermata di benvenuto
 * 
 */
void hideWelcomeScreen();

/**
 * @brief Mostra schermata di addio
 * 
 */
void showFarewellScreen();

/**
 * @brief Get the User command
 * 
 * @return action the action chosen by the user
 */
action getUserAction(const game &);

// possible alternatives
// 1. everything is updated always:
//    in update view (called by the application)

/**
 * @brief aggiorna schermata (completa)
 * 
 */
void updateView(const game &);
// 2. just when something new happens:
//    in the following functions (called by the game logic)

/**
 * @brief called when a guess is checked
 * 
 */
void guessChecked(const game &);

/**
 * @brief called when a game is started
 * 
 */
void gameStarted(const game &);

/**
 * @brief called when a game is over
 * 
 */
void gameEnded(const game &);

/**
 * @brief called when the secret is shown
 * 
 */
void secretShown(const game &);

// 3. a mix of the two alternatives, of course

/**
 * @brief called when selected guess changes
 * 
 * @param g 
 * @param amount 
 */
void changeSelection(game &g, int amount);

/**
 * @brief utility function to show a message from application
 * 
 * @param msg 
 */
void statusMsg(const char msg[]);

#endif
