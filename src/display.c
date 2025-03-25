#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/pokemon.h"
#include "../include/display.h"
#include "../include/parser.h"

char *format_title(int id, char *name, char *genus, char *shiny) {
  char *result = strdup(NOT_FOUND);

  char *bg;
  char *color;
  char p_id[5];
  char *text;

  int size;
   
  // Background color for ID
  size = strlen(BG) + strlen(WHITE) + 1;
  bg = malloc(sizeof(char) * size);
  snprintf(bg, size, "%s%s", BG, WHITE);

  // Color for shiny
  if (strcmp(shiny, "shiny") == 0) {
    size = strlen(FG) + strlen(ELECTRIC) + 1;
    color = malloc(sizeof(char) * size);
    snprintf(color, size, "%s%s", FG, ELECTRIC);
  }

  // Create text for ID
  snprintf(p_id, sizeof(p_id), "%04d", id);

  // Create text for name and genus
  size = 1 + strlen(name) + 3 + strlen(genus) + 1;  // +5 for the spaces and the '\0'
  text = malloc(sizeof(char) * size);
  snprintf(text, size, " %s - %s", name, genus);

  // Store result
  size = strlen(p_id) + strlen(text) + strlen(bg) + 2 * (strlen(DEFAULT) + strlen(color)) + 1;
  result = malloc(sizeof(char) * size);
  snprintf(result, size, "%s%s%s%s%s%s%s", bg, color, p_id, DEFAULT, color, text, DEFAULT);

  // Free everything
  free(bg);
  free(color);
  free(text);
  
  return result;
}

char *type_color(char *type);

int format_types(char *types[2]);

int format_desc(char *desc);

int display(struct Pokemon *pokemon, char *shiny) {
  char imagePath[512];
  snprintf(imagePath, sizeof(imagePath), "%s/%s/%s", "assets/icons", shiny, pokemon->alias);

  // Image of the pokémon
  // char *image = fetch_icon(imagePath, pokemon->alias, 0);
  
  char *title = format_title(pokemon->id, pokemon->name, pokemon->genus, shiny);
  printf("%s\n", imagePath);

  return 0;
}
