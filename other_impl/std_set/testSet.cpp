#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VAL 0xFFFFFFFFFFFFFFFF

void insert_set (std::set <uint64_t>& s, uint64_t key) {
  s.insert(key);
}


uint64_t prev_set (std::set <uint64_t>& s, uint64_t key) {
  auto it = s.lower_bound(key);
  while ((it != s.begin()) && (*it > key)) {
    it--;
  }
  if (it == s.begin()){
    return MAX_VAL;
  }
  return *it;
}

uint64_t next_set (std::set <uint64_t>& s, uint64_t key) {
  auto it = s.lower_bound(key);
  if (it == s.end()){
    return 0;
  }
  return *it;
}

void erase_set (std::set <int>& s, uint64_t key) {
  s.erase(key);
}


uint8_t * read_ops(char * filepath, uint64_t * len){

  FILE * file = fopen(filepath, "rb");
  if (!file){
    fprintf(stderr, "Error: Could not read ops from file: %s...\n", filepath);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  uint64_t file_len = ftell(file);
  rewind(file);

  *len = file_len;

  uint8_t * op_buffer = (uint8_t *) malloc(file_len * sizeof(uint8_t));
  if (!op_buffer){
    fprintf(stderr, "Error: Could not allocate op buffer...\n");
    return NULL;
  }

  fread(op_buffer, 1, file_len, file);
  fclose(file);

  return op_buffer;
}


uint64_t * read_operands(char * filepath, uint64_t * len){

  FILE * file = fopen(filepath, "rb");
  if (!file){
    fprintf(stderr, "Error: Could not read operands from file: %s...\n", filepath);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  uint64_t file_len = ftell(file);

  uint64_t num_els = file_len / sizeof(uint64_t);

  *len = num_els;

  rewind(file);

  uint64_t * operand_buffer = (uint64_t *) malloc(num_els * sizeof(uint64_t));
  if (!operand_buffer){
    fprintf(stderr, "Error: Could not allocate operand buffer...\n");
    return NULL;
  }

  fread(operand_buffer, sizeof(uint64_t), num_els, file);
  fclose(file);

  return operand_buffer;
}

int main(int argc, char * argv[]){

  if (argc != 3){
    fprintf(stderr, "Error. Usage: ./testSet <ops_filepath> <operands_filepath>\n");
    return -1;
  }

  char * ops_filepath = argv[1];
  char * operands_fileapth = argv[2];

  uint64_t ops_len;
  uint64_t operands_len;

  uint8_t * ops = read_ops(ops_filepath, &ops_len);
  if (!ops){
    fprintf(stderr, "Error: unable to read ops...\n");
    return -1;
  }

  uint64_t * operands = read_operands(operands_fileapth, &operands_len);
  if (!operands){
    fprintf(stderr, "Error: uanble to read operands...\n");
    return -1;
  }

  uint64_t num_ops = ops_len;

  if (num_ops != operands_len){
    fprintf(stderr, "Error: # ops (%lu) != # operands (%lu)...\n", num_ops, operands_len);
    return -1;
  }

  printf("First op: %d\nFirst operand: %lu\n", ops[0], operands[0]);


}