#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// personal files
#include "../include/pokemon.h"
#include "../include/parser.h"

/**
 * @struct Memory
 * @brief A structure representing a memory space.
 *
 * This structure is used to store response from a HTTP request.
 */
struct Memory {
  char *response; /**< Response to save in the memory */
  size_t size;    /**< Size of the response */
};

/**
 * @brief Callback function for handling HTTP response data.
 *
 * This function is used with `libcurl` to process incoming HTTP response data
 * and dynamically store it in a `Memory` struct.
 *
 * @param contents Pointer to the received data chunk
 * @param size Size of each data element (typically 1)
 * @param nmemb Number of elements in the data chunk
 * @param userp Pointer to a `Memory` struct where the response is stored
 * @return The total number of bytes successfully written
 *
 * @see Memory
 */
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t total_size = size * nmemb;
  struct Memory *mem = (struct Memory *)userp;

  char *ptr = realloc(mem->response, mem->size + total_size + 1);
  if (ptr == NULL)
    return 0; // Memory allocation failed

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), contents, total_size);
  mem->size += total_size;
  mem->response[mem->size] = '\0';

  return total_size;
}

/**
 * @brief Function used for fetching from a HTTP response data.
 *
 * This function sends an HTTP GET request to the PokéAPI to retrieve Pokémon
 * data in JSON format based on the given Pokémon ID.
 *
 * @param id ID of the pokémon to fetch (e.g., 25 for Pikachu)
 * @param data Where the data are fetched in the PokéAPI (e.g., 'pokemon' for
 * basic information)
 * @return A dynamically allocated string containing the API response (JSON
 * format), or `NULL` if the request fails
 *
 * @see write_callback()
 * @see Memory
 */
char *fetch_pokemon(const char *link, const char *data, int id) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  // Curl variables
  CURL *curl;
  CURLcode res;
  struct Memory chunk = {NULL, 0};

  // Initialize curl
  curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "Curl initialization failed\n");
    curl_global_cleanup();
    return NULL;
  }

  // Build API URL
  char url[strlen(link) + strlen(data) + 9];
  if (id == 0) {
    snprintf(url, sizeof(url), "%s/%s", link, data);
  } else {
    snprintf(url, sizeof(url), "%s/%s/%d/", link, data, id);
  }

  // Setup curl_easy_setopt() options
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  // Perform a HTTP request
  res = curl_easy_perform(curl);
  // Error handling
  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    free(chunk.response);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return NULL;
  }

  // Clean up resources
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  // Return response
  return chunk.response;
}

/**
 * @brief Retrieve the total number of pokémon in pokéAPI
 *
 * This function calls the `fetch_pokemon()` function to retrieve Pokémon data
 * in JSON format, parses the JSON, and extracts the "count" field, which
 * represents the number of Pokémon.
 * If the JSON parsing fails or the "count" field is not found or is not a
 * number, the function will return `0`.
 *
 * @return The number of Pokémon as an integer. If an error occurs or the
 * "count" field is missing or invalid, it returns `0`
 *
 * @see fetch_pokemon()
 */
int pokemon_count() {
  char *json_str = fetch_pokemon(POKEAPI, "pokemon-species", 0);
  cJSON *json = cJSON_Parse(json_str);
  int result = 0;
  if (!json) {
    fprintf(stderr, "pokemon JSON parsing failed\n");
    free(json_str);
    return 0;
  }
  free(json_str);

  cJSON *data = cJSON_GetObjectItem(json, "count");
  if (cJSON_IsNumber(data)) {
    result = data->valueint;
  }
  cJSON_Delete(json);
  return result;
}

/**
 * @brief Retrieve a string value from a cJSON object.
 *
 * This function extracts a string from a given cJSON object by key name.
 * If the key is does not exist or is not a string, it returns "Not Found".
 *
 * @param json Pointer to a cJSON object
 * @param name The key name to search for in the cJSON object
 * @return The corresponding string value if found, otherwise "Not Found"
 */
char *get_str(cJSON *json, char *name, char *lang) {
  cJSON *data = cJSON_GetObjectItem(json, name);
  char *result = NOT_FOUND;
  if (lang == NULL) {
    if (cJSON_IsString(data)) {
      result = strdup(data->valuestring);
    }
  } else {
    int size = cJSON_GetArraySize(data);
    for (int i = 0; i < size; i++) {
      cJSON *name_data = cJSON_GetArrayItem(data, i);
      cJSON *lang_data = cJSON_GetObjectItem(name_data, "language");
      char *lang_str = cJSON_GetObjectItem(lang_data, "name")->valuestring;
      if (strcmp(lang, lang_str) == 0) {
        result = strdup(cJSON_GetObjectItem(name_data, "name")->valuestring);
      }
    }
  }
  return result;
}

/**
 * @brief Retrieve an int value from a cJSON object.
 *
 * This function extracts an int from a given cJSON object by key name.
 * If the key is does not exist or is not a string, it returns "Not Found".
 *
 * @param json Pointer to a cJSON object
 * @param name The key name to search for in the cJSON object
 * @return The corresponding int value if found, otherwise "Not Found"
 */
int get_int(cJSON *json, char *name) {
  cJSON *json_int = cJSON_GetObjectItem(json, name);
  if (cJSON_IsNumber(json_int)) {
    return json_int->valueint;
  }
  return 0;
}

/**
 * @brief Retrieve the types from a cJSON object.
 *
 * This function extracts either one or two string value from a given cJSON
 * object that represents the types of the pokémon. For each types not found the
 * result is "Not Found".
 *
 * @param json Pointer to a cJSON object
 * @param types An array of two strings to store the result
 */
void get_types(cJSON *json, char *types[2]) {
  cJSON *data = cJSON_GetObjectItem(json, "types");
  if (cJSON_IsArray(data)) {
    int size = cJSON_GetArraySize(data);
    for (int i = 0; i < size; i++) {
      cJSON *type_json = cJSON_GetArrayItem(data, i);
      if (type_json) {
        cJSON *type = cJSON_GetObjectItem(type_json, "type");
        cJSON *name = cJSON_GetObjectItem(type, "name");

        types[i] = strdup(name->valuestring);
      }
    }
  }
}

char *read_json_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);

  char *json_data = (char *)malloc(length + 1);
  if (!json_data) {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  fread(json_data, 1, length, file);
  fclose(file);

  json_data[length] = '\0'; // Null-terminate the string
  return json_data;
}


void convert_types(char *types[2], char *lang) {
  char *json_str = read_json_file("assets/types.json");

  cJSON *json = cJSON_Parse(json_str);
  if (!json) {
    fprintf(stderr, "Type JSON parsing failed\n");
    return;
  }

  if (cJSON_IsArray(json)) {
    int size = cJSON_GetArraySize(json);
    for (int i = 0; i < size; i++) {
      cJSON *types_json = cJSON_GetArrayItem(json, i);
      char *type = cJSON_GetObjectItem(types_json, "en")->valuestring;
      if (strcmp(types[0], type) == 0) {
        types[0] = strdup(cJSON_GetObjectItem(types_json, lang)->valuestring);
      } else if (strcmp(types[1], type) == 0) {
        types[1] = strdup(cJSON_GetObjectItem(types_json, lang)->valuestring);
      }
    }
  }
  cJSON_Delete(json);
}

/**
 * @brief Retrieve the description from a cJSON object.
 *
 * This function extracts the description as a string value from a given
 * cJSON object and returns it, if it does not exists returns "Not Found".
 *
 * @param json Pointer to a cJSON object
 * @param version Version of the description (e.g. "omega-ruby" by default)
 * @param lang Language of the description (e.g. "fr" by default)
 * @return The description as a string value if found, otherwise "Not Found"
 */
char *get_desc(cJSON *json, char *version, char *lang) {
  if (version == NULL)
    version = "omega-ruby";
  if (lang == NULL)
    lang = "fr";

  cJSON *flavor_text_entries = cJSON_GetObjectItem(json, "flavor_text_entries");
  if (cJSON_IsArray(flavor_text_entries)) {
    int size = cJSON_GetArraySize(flavor_text_entries);
    for (int i = 0; i < size; i++) {
      cJSON *poke_json = cJSON_GetArrayItem(flavor_text_entries, i);
      if (poke_json) {
        // Get the version name
        cJSON *version_data = cJSON_GetObjectItem(poke_json, "version");
        cJSON *version_name = cJSON_GetObjectItem(version_data, "name");

        // Get the language name
        cJSON *lang_data = cJSON_GetObjectItem(poke_json, "language");
        cJSON *lang_name = cJSON_GetObjectItem(lang_data, "name");

        // Check if the language and the version is the one we ask for
        if (cJSON_IsString(lang_name) && cJSON_IsString(version_name)) {
          if (strcmp(lang_name->valuestring, lang) == 0 &&
              strcmp(version_name->valuestring, version) == 0) {
            cJSON *flavor_text = cJSON_GetObjectItem(poke_json, "flavor_text");
            return strdup(flavor_text->valuestring);
          }
        }
      }
    }
  }
  return NOT_FOUND;
}

/**
 * @brief Retrieve the genus from a cJSON object.
 *
 * This function extracts the genus as a string value from a given
 * cJSON object and returns it, if it does not exists returns "Not Found".
 *
 * @param json Pointer to a cJSON object
 * @param lang Language of the genus (e.g. "fr" by default)
 * @return The genus as a string value if found, otherwise "Not Found"
 */
char *get_genus(cJSON *json, char *lang) {
  if (lang == NULL)
    lang = "fr";

  cJSON *data = cJSON_GetObjectItem(json, "genera");
  if (cJSON_IsArray(data)) {
    int size = cJSON_GetArraySize(data);
    for (int i = 0; i < size; i++) {
      cJSON *genus_json = cJSON_GetArrayItem(data, i);
      if (genus_json) {
        // Get the language name
        cJSON *lang_data = cJSON_GetObjectItem(genus_json, "language");
        cJSON *lang_name = cJSON_GetObjectItem(lang_data, "name");

        // Check if the language and the version is the one we ask for
        if (cJSON_IsString(lang_name)) {
          if (strcmp(lang_name->valuestring, lang) == 0) {
            return strdup(cJSON_GetObjectItem(genus_json, "genus")->valuestring);
          }
        }
      }
    }
  }
  return NOT_FOUND;
}

/**
 * @brief Get the image as pixelart of the pokemon
 *
 * This function search into the assets files generated by `make icon` and
 * stores it in a char to print it later.
 *
 * @param filename const char* path to the pokemon to get
 * @param buf char *buffer where the result will be stored
 * @param len int the size of the buffer
 *
 * @return an int 0 if buffer is filled, -1 if error, the size needed for the buffer otherwise
 */
int fetch_icon(char *filename, char *buf, int len) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    file = fopen("assets/icons/unknown.txt", "r");
    if (file == NULL) {
      fprintf(stderr, "Error opening file");
      return -1;
    }
  }

  fseek(file, 0, SEEK_END); // end of file
  long size = ftell(file) + 1;  // get the file size
  rewind(file);             // return to the start of the file

  if (len < size) return size;

  char *result = malloc(size);
  if (result == NULL) {
    perror("Error malloc");
    fclose(file);
    return -1;
  }

  fread(result, 1, size, file);
  result[size-1] = '\0';
  strcpy(buf, result);

  free(result);
  fclose(file);

  return 0;
}

/**
 * @brief Parse the pokemon from two json data.
 *
 * This function creates a typedef Pokemon where is stored all of its
 * information including: name, id, types, height, weight, description and
 * genus.
 *
 * @param json_str Json data as a string where we can find the name, id, types,
 * height and weight
 * @param json_spe_str Json data as a string where we can find the description
 * and the genus
 * @return A typedef Pokemon that contains information about it, with "Not
 * Found" or 0 as a result for each not found information
 *
 * @see Pokemon
 * @see get_str()
 * @see get_int()
 * @see get_types()
 * @see get_desc()
 * @see get_genus()
 */
int parse_pokemon_json(struct Pokemon *pokemon, const char *shiny, const char *json_str,
                       const char *json_spe_str, char *version, char *lang) {
  cJSON *json = cJSON_Parse(json_str);
  if (!json) {
    fprintf(stderr, "pokemon JSON parsing failed\n");
    return 1;
  }

  // Extract "name" field in english
  pokemon->alias = get_str(json, "name", NULL);
  // Extract "id" field
  pokemon->id = get_int(json, "id");
  // Extract "types" field
  get_types(json, pokemon->types);
  // convert_types(pokemon->types);
  // Extract "height" field
  pokemon->height = get_int(json, "height");
  // Extract "weight" field
  pokemon->weight = get_int(json, "weight");

  cJSON_Delete(json);

  cJSON *json_spe = cJSON_Parse(json_spe_str);
  if (!json_spe) {
    fprintf(stderr, "pokemon-species JSON parsing failed\n");
    return 1;
  }

  // Extract "name" field
  pokemon->name = get_str(json_spe, "names", lang);
  // Extract "desc" field
  pokemon->desc = get_desc(json_spe, version, lang);
  // Extract "genus" field
  pokemon->genus = get_genus(json_spe, lang);
  // Extract "icon" field
  char imagePath[512];
  snprintf(imagePath, sizeof(imagePath), "%s/%s/%s.txt", "assets/icons", shiny,
           pokemon->alias);

  // Image of the pokémon
  int size = fetch_icon(imagePath, NULL, 0);
  if (size < 0) {
    fprintf(stderr, "Error in parser.c: Icon not found for %s\n", imagePath);
  } else {
    char *image = calloc(size, sizeof(*image));
    fetch_icon(imagePath, image, size);
    if (image == NULL) {
      fprintf(stderr, "Error in parser.c: Failed to fetch pokemon icon.\n");
      return 1;
    }
    printf("test : %s\n%s", imagePath, image);
    pokemon->icon = image;
  }
  cJSON_Delete(json_spe);
  return 0;
}

/**
 * @brief Free the pokemon struct type
 *
 * This function free the typedef Pokemon structure given in argument starting
 * by the name, types, description and genus.
 *
 * @param pokemon typedef Pokemon struct to free
 *
 * @see Pokemon
 */
void free_pokemon(struct Pokemon *pokemon) {
  if (strcmp(pokemon->name, NOT_FOUND) != 0)
    free(pokemon->name);
  if (strcmp(pokemon->alias, NOT_FOUND) != 0)
    free(pokemon->alias);
  if (strcmp(pokemon->desc, NOT_FOUND) != 0)
    free(pokemon->desc);
  if (strcmp(pokemon->genus, NOT_FOUND) != 0)
    free(pokemon->genus);
  if (strcmp(pokemon->icon, NOT_FOUND) != 0)
    free(pokemon->icon);
  if (pokemon->types[0])
    if (strcmp(pokemon->types[0], NOT_FOUND) != 0)
      free(pokemon->types[0]);
  if (pokemon->types[1])
    if (strcmp(pokemon->types[1], NOT_FOUND) != 0)
      free(pokemon->types[1]);
}
