#ifndef ACTION_CPP
#define ACTION_CPP

/**
 * @brief the action representation
 * 
 */
struct action
{
    action_code code;
    action_param param;
};

/**
 * @brief Returns whether an action is enabled or not in the given game
 * 
 * @param code  the action code
 * @param g     the game
 * @return true if the action is enabled
 * @return false  otherwise
 */
bool isEnabled(action_code code, const game &g)
{
    switch (code)
    {
    case EXIT:
        return true;
    case NEW:
        return getStatus(g) != NEW_GAME;
    case MOVE:
    case TRY:
        return (getStatus(g) & OVER) == 0;
    case SHOW:
        return (getStatus(g) & (REVEALED | RIGHT_GUESSED)) == 0;
    case NONE:
        return true;
    default:
        return false;
    }
}

/**
 * @brief process the given action on the given game
 * 
 * @param a     the action
 * @param g     the game
 * @param config    the application configuration 
 */
void processUserAction(action a, game &g, configuration &config)
{
    if (isEnabled(a.code, g))
    {
        switch (a.code)
        {
        case EXIT:
            break;
        case NEW:
            g = newGame(config);
            break;
        case TRY:
            checkGuess(g, a.param); // ignore result
            break;
        case MOVE:
            changeSelection(g, a.param);
            break;
        case SHOW:
            getSecret(g); // ignore result
            break;
        case NONE:
            break;
        default:
        {
            // cout << "Should never be here !!!! " << endl;
        }
        }
    }
    else
    {
        statusMsg("command not available");
    }
}

#endif