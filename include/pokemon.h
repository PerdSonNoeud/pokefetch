#ifndef POKEMON_H
#define POKEMON_H

#include <stddef.h>

#define POKEAPI "https://pokeapi.co/api/v2"
#define POKEMON_IMG "assets/icons/"
#define NOT_FOUND "Not Found"

// Colors
#define BG       "[48;2;"
#define FG       "[38;2;"
#define DEFAULT  "[0m"

#define WHITE "220;220;220m"
#define BLACK "0;0;0m"

#define NORMAL   "160;160;160m"
#define FIRE     "230;40;40m"
#define WATER    "40;130;240m"
#define GRASS    "60;160;40m"
#define FIGHTING "255;128;0m"
#define FLYING   "130;185;240m"
#define POISON   "145;65;200m"
#define ELECTRIC "250;190;0m"
#define GROUND   "145;80;30m"
#define ROCK     "175;170;130m"
#define PSYCHIC  "240;65;120m"
#define ICE      "60;205;240m"
#define BUG      "145;160;25m"
#define DRAGON   "80;95;225m"
#define GHOST    "110;65;110m"
#define DARK     "100;75;80m"
#define STEEL    "95;160;185m"
#define FAIRY    "240;110;240m"

/**
 * @struct Pokemon
 * @brief A structure representing a pokémon profile.
 *
 * This structure is used to store pokémon information.
 */
struct Pokemon {
  char *name;     /**< Name of the pokémon */
  char *alias;    /**< Name of the pokémon in english, needed for the image URL */
  int id;         /**< ID of the pokémon in the pokedex */
  char *types[2]; /**< Types of the pokémon */
  int height;     /**< Height of the pokémon */
  int weight;     /**< Weight of the pokémon */
  char *desc;     /**< Description of the pokémon from the pokedex */
  char *genus;    /**< Genus/Category of the pokémon */
  char *icon;     /**< Icon of the pokémon */
};

#endif // !POKEMON_H
