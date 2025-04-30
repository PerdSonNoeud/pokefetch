#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/pokemon.h"
#include "../include/display.h"

size_t raw_text_size(const char *text) {
    size_t size = 0;
    while (*text) {
        if (*text == '\033') { // Start of ANSI escape sequence
            while (*text && !isalpha(*text)) {
                text++;
            }
            if (*text) text++; // Skip the final letter of the escape sequence
        } else {
            size++;
            text++;
        }
    }
    return size;
}

char *format_title(int id, char *name, char *genus, char *shiny) {
  char *result = NOT_FOUND;

  char *bg;
  char *color;
  char p_id[7];
  char *text;

  int size;
   
  // Background color for ID
  size = strlen(BG) + strlen(WHITE) + strlen(FG) + strlen(BLACK) + 1;
  bg = malloc(sizeof(char) * size);
  snprintf(bg, size, "%s%s%s%s", BG, WHITE, FG, BLACK);

  // Color for shiny
  if (strcmp(shiny, "shiny") == 0) {
    size = strlen(FG) + strlen(ELECTRIC) + 1;
    color = malloc(sizeof(char) * size);
    snprintf(color, size, "%s%s", FG, ELECTRIC);
  } else {
    color = "\0";
  }

  // Create text for ID
  snprintf(p_id, sizeof(p_id), " %04d ", id);

  // Create text for name and genus
  size = 1 + strlen(name) + 3 + strlen(genus) + 1;  // +5 for the spaces and the '\0'
  text = malloc(sizeof(char) * size);
  snprintf(text, size, " %s - %s", name, genus);

  // Store result
  size = strlen(bg) + strlen(p_id) + 2 * strlen(DEFAULT) + strlen(color) + strlen(text) + 3;
  result = malloc(sizeof(char) * size);
  snprintf(result, size, " %s%s%s%s%s%s ", bg, p_id, DEFAULT, color, text, DEFAULT);

  // Free everything
  free(bg);
  if (strcmp(shiny, "shiny") == 0) { free(color); }
  free(text);
  
  return result;
}

char *type_color(char *type) {
  char *result;
  // Define the type mappings to match the names with the colors
  struct TypeMapping {
      const char *name;
      char *value;
  };
  // Mapping of type names to their corresponding color codes
  struct TypeMapping typeMappings[] = {
      {"normal", NORMAL}, {"fire", FIRE}, {"water", WATER}, {"grass", GRASS},
      {"fighting", FIGHTING}, {"flying", FLYING}, {"poison", POISON},
      {"electric", ELECTRIC}, {"ground", GROUND}, {"rock", ROCK},
      {"psychic", PSYCHIC}, {"ice", ICE}, {"bug", BUG}, {"dragon", DRAGON},
      {"ghost", GHOST}, {"dark", DARK}, {"steel", STEEL}, {"fairy", FAIRY}
  };
  // Get the number of mappings
  size_t numTypes = sizeof(typeMappings) / sizeof(typeMappings[0]);
  result = NOT_FOUND; // Default value for unknown type
  // Iterate through the mappings to find the matching type
  for (size_t i = 0; i < numTypes; i++) {
      if (strcmp(type, typeMappings[i].name) == 0) {
          result = typeMappings[i].value;
          break;
      }
  }

  return result;
}


char *spaces(size_t size, char *text) {
  char *space = malloc(sizeof(char) * (size + 1));
  if (!space) return NULL; // always check malloc

  for (size_t i = 0; i < size; i++) {
    space[i] = ' ';
  }
  space[size] = '\0';

  size_t new_size = strlen(space) + strlen(text) + 1;
  char *result = malloc(sizeof(char) * new_size);
  if (!result) {
    free(space);
    return NULL;
  }

  snprintf(result, new_size, "%s%s", space, text);
  free(space);

  return result;
}


char *format_types(size_t max_size, char *types[2]) {
  char *result = NOT_FOUND;
  char *color, *text[2], *bg;
  size_t size, spaces_size;
  
  for (int i = 0; i < 2; i++) {
    if (strcmp(types[i], NOT_FOUND) != 0) {
      // Get the color for the type
      color = type_color(types[i]);

      // Create background color
      size = strlen(BG) + strlen(color) + 1;
      bg = malloc(sizeof(char) * size);
      snprintf(bg, size, "%s%s", BG, color);

      // Create text for the type
      size = strlen(bg) + strlen(types[i]) + strlen(DEFAULT) + 5;
      text[i] = malloc(sizeof(char) * size);
      snprintf(text[i], size, " %s %s %s ", bg, types[i], DEFAULT);

      // Free everything
      free(bg);
    } else {
      text[i] = "";
    }
  }

  // Create the result string
  size = strlen(text[0]) + strlen(text[1]) + strlen(DEFAULT) + 1;
  if (strcmp(types[1], NOT_FOUND) == 0) {
    size += 4; // For the spaces
  }
  
  result = malloc(sizeof(char) * size);
  if (strcmp(types[1], NOT_FOUND) == 0) {
    snprintf(result, size, "%s%s", text[0], DEFAULT);
  } else {
    snprintf(result, size, "%s    %s%s", text[0], text[1], DEFAULT);
  }
  // Add spaces to center the result
  spaces_size = (max_size - raw_text_size(result))/2;  
  char *spaces_result = spaces(spaces_size, result);
  free(result);
  result = malloc(sizeof(char) * (strlen(spaces_result) + 1));
  snprintf(result, strlen(spaces_result) + 1, "%s", spaces_result);
  free(spaces_result);

  // Free the text strings
  for (int i = 0; i < 2; i++) {
    if (strcmp(types[i], NOT_FOUND) != 0) {
      free(text[i]);
    }
  }

  return result;
}

int format_desc(char *desc);

int display(struct Pokemon *pokemon, char *shiny) {
  char imagePath[512];
  snprintf(imagePath, sizeof(imagePath), "%s/%s/%s", "assets/icons", shiny, pokemon->alias);

  // Image of the pokémon
  // char *image = fetch_icon(imagePath, pokemon->alias, 0);
  
  char *title = format_title(pokemon->id, pokemon->name, pokemon->genus, shiny);
  if (strcmp(title, NOT_FOUND) == 0) {
    fprintf(stderr, "Error in display.c: Failed to format title.\n");
    return 1;
  }
  size_t title_size = raw_text_size(title);
  printf("%s\n", title);
  
  char *types = format_types(title_size, pokemon->types);
  if (strcmp(types, NOT_FOUND) == 0) {
    fprintf(stderr, "Error in display.c: Failed to format types.\n");
    free(title);
    return 1;
  }
  printf("%s\n", types);
  
  // Free the memory allocated
  free(title);
  free(types);

  return 0;
}
