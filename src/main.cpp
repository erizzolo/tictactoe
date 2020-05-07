/**
 * Purpose: base game application: guess a decimal digit
 * Author:  Emanuele Rizzolo
 * Class:   3XIN
 * Date:    2020/01/15
 * Note:
*/

/**
 * La logica del gioco è semplice e consiste nel far indovinare all'utente,
 * in un tempo e con un numero di tentativi limitati, un segreto generato
 * casualmente dal computer.
 * L'utente può eseguire delle "azioni" sul gioco stesso specificando, cioè
 * eseguendo, opportuni "comandi" dell'interfaccia utente.
 */
/**
 * Per facilitare le modifiche, si sono tenute il più possibile separate:
 *  1   la logica del gioco (game.h / game.cpp):
 *      così è possibile cambiare interfaccia utente senza modificarla
 *  2   la logica dell'applicazione (action.h / action.cpp):
 *      così è possibile "sostituire" un tipo ad un altro senza impatti su 1
 *  3   l'interfaccia utente (ui.h /ui.cpp):
 *      così è possibile "sostituire" un tipo ad un altro senza impatti su 1/2
 */

#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define DEBUG 0

// The application logic ======================================================
// application configuration
const int MAX_GUESSES = 10;     // max number of guesses: fair enough!
const double TIME_ALLOWED = 10; // ten seconds = 1 seconds / guess
struct configuration
{
    int maxGuesses{MAX_GUESSES / 2};  ///< il numero di tentativi concessi
    double timeAllowed{TIME_ALLOWED}; ///< tempo concesso per indovinare
};

// carica e restituisce la configurazione dell'applicazione
configuration loadConfiguration();
// salva la configurazione dell'applicazione
void saveConfiguration(const configuration &);

#include "game.h"
#include "action.h"
#include "ui.h"

#include "game.cpp"
#include "action.cpp"
#include "ui.cpp"

// The main logic ==============================================================
int main(int argc, char *argv[])
{
    srand(time(nullptr)); // set random seed if needed
    showWelcomeScreen();
    configuration config = loadConfiguration();
    hideWelcomeScreen();
    action a;
    game g = newGame(config);
    do
    {
        updateElapsed(g); // update game
        updateView(g);    // update UI
        a = getUserAction(g);
        processUserAction(a, g, config);
    } while (a.code != EXIT);
    saveConfiguration(config);
    showFarewellScreen();

    /// successful termination
    return 0;
}

// Implementations!!!
// application functions
configuration loadConfiguration()
{
    statusMsg("Loading configuration...");
    configuration result;
    ifstream in("config.ini");
    if (in)
    {
        in >> result.maxGuesses >> result.timeAllowed;
    }
    in.close();
    return result;
}
void saveConfiguration(const configuration &c)
{
    statusMsg("Saving configuration... ");
    ofstream out("config.ini");
    if (out)
    {
        out << c.maxGuesses << " " << c.timeAllowed;
    }
    out.close();
}
