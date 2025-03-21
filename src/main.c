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
  srand(time(NULL));
  return min + rand() % (max - min + 1); 
}

int main(int argc, char **argv) {
  // Number of pokemon
  int count = pokemon_count();
  // Version of the pokedex
  char *version = "omega-ruby";
  // Language of the text
  char *lang = "fr";
  // ID of the pokemon to print
  int id = randrange(1, count);

  // Checks for parameters
  for (int i = 1; i < argc; i++) {
    // Select a specific ID
    if (strcmp(argv[i], "-id") == 0) {
      if (isdigit(argv[i + 1])) {
        id = atoi(argv[i + 1]);
        i++;
      } else {
        fprintf(stderr, "Invalid argument, %s must be an integer between 0 and %d.", argv[i + 1], count);
        return EXIT_FAILURE;
      }
    }
  }

  const char *json_str = fetch_pokemon("pokemon", id);
  const char *json_spe_str = fetch_pokemon("pokemon-species", id);
  struct Pokemon pokemon = {strdup(NOT_FOUND), 0, {strdup(NOT_FOUND), strdup(NOT_FOUND)}, 0, 0, strdup(NOT_FOUND), strdup(NOT_FOUND)};

  if (parse_pokemon_json(&pokemon, json_str, json_spe_str, version, lang)) {
    fprintf(stderr, "parse_pokemon_json() failed.\n");
    return EXIT_FAILURE;
  }

  display(&pokemon);

  free_pokemon(&pokemon);
  return EXIT_SUCCESS;
}
