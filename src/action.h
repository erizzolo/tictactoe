#ifndef ACTION_H
#define ACTION_H

// The action logic ============================================================
/**
 * Definizione interna delle operazioni che si possono effettuare.
 * Le azioni sono in numero finito ed identificate internamente da un codice,
 * indipendentemente dalla modalità con cui l'utente le indica; possono
 * essere abilitate o disabilitate a seconda della situazione del gioco.
 * L'abilitazione e la disabilitazione di un'azione dovrebbero essere
 * notificate in qualche modo all'utente tramite l'interfaccia.
 * Per gestire più agevolmente azioni "simili" esse possono avere
 * in alcuni casi un dato (parametro) aggiuntivo.
 * Ad es. anzichè gestire quattro azioni di spostamento (Nord, Sud, Est, Ovest)
 * ne prevedo una sola (Move) con parametro direzione.
 * In questo caso viene usata per l'azione TRY.
 */

/**
 * @brief possible action codes
 * 
 */
enum action_code
{
    NONE,       ///< fake code to indicate no action at all
    EXIT,       ///< exit the application
    NEW,        ///< new game
    TRY,        ///< make a guess (with guess as parameter)
    MOVE,       ///< move selection (with amount as parameter)
    SIZE,       ///< resize board (with amount as parameter)
    NUM_ACTIONS ///< fake code: total number of actions (including NONE)
};

// action parameter [optional] data type
using action_param = int;
// definition of no parameter specified
#define PARAM_NONE 0

/**
 * @brief the action representation (to be defined in action.cpp)
 * 
 */
struct action;

/**
 * @brief Returns whether an action is enabled or not
 * 
 * @return true if the action is enabled
 * @return false  otherwise
 */
bool isEnabled(action_code, const game &);

/**
 * @brief process the given action on the given game
 * 
 */
void processUserAction(action, game &, configuration &);

#endif