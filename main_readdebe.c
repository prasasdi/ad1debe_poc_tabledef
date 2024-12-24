#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./helper/pager.h"

typedef enum { NUMERIK, KARAKTER, DESIMAL } e_debe_type;

typedef struct {
  char name[255];
  e_debe_type data_type;
} debe_column;

typedef struct {
  char name[255];
  debe_column *columns;
  uint32_t columns_length;
} debe_table_definition;

const uint32_t COLUMN_NAME_SIZE = sizeof(char[255]);
const uint32_t COLUMN_NAME_OFFSET = 0;

const uint32_t COLUMN_TYPE_SIZE = sizeof(e_debe_type);
const uint32_t COLUMN_TYPE_OFFSET = COLUMN_NAME_SIZE;

const uint32_t TABLE_DEFINITION_NAME_SIZE = sizeof(char[255]);
const uint32_t TABLE_DEFINITION_NAME_OFFSET = 0;
const uint32_t TABLE_DEFINITION_COLUMNS_SIZE = sizeof(debe_column *);
const uint32_t TABLE_DEFINITION_COLUMNS_OFFSET = TABLE_DEFINITION_NAME_SIZE;
const uint32_t TABLE_DEFINITION_COLUMNS_LENGTH_SIZE = sizeof(uint32_t);
const uint32_t TABLE_DEFINITION_COLUMNS_LENGTH_OFFSET =
    TABLE_DEFINITION_COLUMNS_SIZE;

void write_table_definition(FILE *file, debe_table_definition *table) {
  uint32_t name_size = sizeof(table->name);
  uint32_t columns_length_size = sizeof(table->columns_length);
  uint32_t columns_size =
      table->columns_length * (sizeof(char[255]) + sizeof(e_debe_type));

  uint32_t total_size = name_size + columns_length_size + columns_size;

  // Allocate buffer
  char *buffer = malloc(total_size);
  char *ptr = buffer;

  // Copy table name
  memcpy(ptr, table->name, name_size);
  ptr += name_size;

  // Copy columns length
  memcpy(ptr, &(table->columns_length), columns_length_size);
  ptr += columns_length_size;

  // Copy columns
  for (uint32_t i = 0; i < table->columns_length; i++) {
    memcpy(ptr, table->columns[i].name, sizeof(table->columns[i].name));
    ptr += sizeof(table->columns[i].name);
    memcpy(ptr, &(table->columns[i].data_type),
           sizeof(table->columns[i].data_type));
    ptr += sizeof(table->columns[i].data_type);
  }

  // Write buffer to file
  fwrite(buffer, total_size, 1, file);

  // Free buffer
  free(buffer);
}

int main(int argc, char *argv[]) {
  // open file
  char *filename = "db.db";

  debe_pager *pager = malloc(sizeof(debe_pager));

  pager->file_descriptor = fopen(filename, "r+b");
  if (pager->file_descriptor == NULL) {
    pager->file_descriptor = fopen(filename, "w+b");
    if (pager->file_descriptor == NULL) {
      perror("Error opening file");
      exit(EXIT_FAILURE);
    }
  }

  pager->file_length = fseek(pager->file_descriptor, 0, SEEK_END);
  printf("file length: %d\n", pager->file_length);
  printf("selesai buka file\n");

  // write satu table ke .db
  debe_table_definition *debe = malloc(sizeof(debe_table_definition));
  strcpy(debe->name, "biodata");
  debe->columns_length = 3;

  debe_column *fields = malloc(sizeof(debe_column) * debe->columns_length);

  strcpy(fields[0].name, "Nama");
  fields[0].data_type = KARAKTER;

  strcpy(fields[1].name, "NoIdentitas");
  fields[1].data_type = NUMERIK;

  strcpy(fields[2].name, "Umur");
  fields[2].data_type = NUMERIK;

  debe->columns = fields;

  // Write table definition to file
  write_table_definition(pager->file_descriptor, debe);

  // Clean up
  free(fields);
  free(debe);
  fclose(pager->file_descriptor);
  free(pager);

  return EXIT_SUCCESS;
}
