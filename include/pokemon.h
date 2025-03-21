#ifndef POKEMON_H
#define POKEMON_H

#include <stddef.h>

#define NOT_FOUND "Not Found"

/**
 * @struct Pokemon
 * @brief A structure representing a pokémon profile.
 *
 * This structure is used to store pokémon information.
 */
struct Pokemon {
  char *name;     /**< Name of the pokémon */
  int id;         /**< ID of the pokémon in the pokedex */
  char *types[2]; /**< Types of the pokémon */
  int height;     /**< Height of the pokémon */
  int weight;     /**< Weight of the pokémon */
  char *desc;     /**< Description of the pokémon from the pokedex */
  char *genus;    /**< Genus/Category of the pokémon */
};

#endif // !POKEMON_H
