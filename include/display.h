#ifndef DISPLAY
#define DISPLAY


/**
 * @brief Function that display information about a pokémon.
 *
 * This function displays information about the given pokemon, it takes ASCII sprite of the
 * pokemon in 'assets/icons/' and displays it on the left and its infomation in the right.
 *
 * @param pokemon struct Pokemon where are the information about him
 * @param shiny char representing "shiny" if the pokemon is shiny, otherwise "regular"
 * @return returns 0 if everything went fine, otherwise 1
 */
int display(struct Pokemon *pokemon, char *shiny);

#endif // !DISPLAY
#define DISPLAY
