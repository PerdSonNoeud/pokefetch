#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// personal files
#include "../include/pokemon.h"
#include "../include/display.h"
#include "../include/parser.h"

int randrange(int min, int max) {
  return min + rand() % (max - min + 1); 
}

int is_shiny(int shiny_rate) {
  if ((rand() % shiny_rate) == 0) return 1;
  return 0;
}

int is_number(const char *str) {
    // Check each character in the string
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;  // Return 0 (false) if any character is not a digit
        }
    }
    return 1;  // Return 1 (true) if all characters are digits
}

int main(int argc, char **argv) {
  // Init random
  srand(time(NULL));
  // Number of pokemon
  int count = pokemon_count();
  // Version of the pokedex
  char *version = "omega-ruby";
  // Language of the text
  char *lang = "fr";
  // ID of the pokemon to print
  int id = randrange(1, count);
  // Shiny rate for the pokemon
  int shiny_rate = 4;

  // Checks for parameters
  for (int i = 1; i < argc; i++) {
    // Select a specific ID
    if (strcmp(argv[i], "-id") == 0) {
      i++;
      if (is_number(argv[i])) {
        id = atoi(argv[i]);
      } else fprintf(stderr, "Invalid argument, %s must be an integer between 0 and %d.\n", argv[i], count); 
    // Select a shiny rate
    } else if (strcmp(argv[i], "-s") == 0) {
      i++;
      if (is_number(argv[i]) && (atoi(argv[i]) < 0)) {
        shiny_rate = atoi(argv[i]);
      } else fprintf(stderr, "Invalid argument, %s must be an integer between 1 and %d.\n", argv[i], INT_MAX);
    }
  }

  printf("ID: %d\n", id);

  const char *json_str = fetch_pokemon(POKEAPI, "pokemon", id);
  const char *json_spe_str = fetch_pokemon(POKEAPI, "pokemon-species", id);
  struct Pokemon pokemon = {strdup(NOT_FOUND), strdup(NOT_FOUND), 0, {strdup(NOT_FOUND), strdup(NOT_FOUND)}, 
    0, 0, strdup(NOT_FOUND), strdup(NOT_FOUND)};

  if (parse_pokemon_json(&pokemon, json_str, json_spe_str, version, lang)) {
    fprintf(stderr, "parse_pokemon_json() failed.\n");
    return EXIT_FAILURE;
  }

  char *shiny;
  if (is_shiny(shiny_rate)) {
    shiny = "shiny";
  } else {
    shiny = "regular";
  }

  if (display(&pokemon, shiny) != 0) {
    fprintf(stderr, "Error displaying pokemon.\n");
  }

  free_pokemon(&pokemon);
  return EXIT_SUCCESS;
}
