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
        return true;
    case MOVE:
    case TRY:
        return getStatus(g) == RUNNING;
    case SIZE:
        return getStatus(g) != RUNNING;
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
void processUserAction(action a, game &g, configuration &c)
{
    if (isEnabled(a.code, g))
    {
        switch (a.code)
        {
        case EXIT:
            break;
        case NEW:
            g = newGame(c);
            break;
        case TRY:
            makeMove(g, a.param); // ignore result
            break;
        case MOVE:
            changeSelection(g, a.param);
            break;
        case SIZE:
            c.boardDim += a.param;
            if (c.boardDim > MAX_DIM)
            {
                c.boardDim = MIN_DIM;
            }
            if (c.boardDim < MIN_DIM)
            {
                c.boardDim = MAX_DIM;
            }
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