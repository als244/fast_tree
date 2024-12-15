#include "common.h"
#include "fast_tree.h"

#define INSERT_OP 0
#define REMOVE_OP 1
#define SEARCH_PREV_OP 2
#define SEARCH_NEXT_OP 3

#define MAX_VAL 0xFFFFFFFFFFFFFFFF

#define TRACE_DIR "./benchmark/traces"
#define SAVE_DIR "/home/shein/Desktop/veb_results/fast_tree"

char * search_type_to_str(FastTreeSearchModifier search_mod){

	switch(search_mod){
		case FAST_TREE_MAX:
			return "Max";
		case FAST_TREE_MIN:
			return "Min";
		case FAST_TREE_PREV:
			return "Prev";
		case FAST_TREE_NEXT:
			return "Next";
		case FAST_TREE_EQUAL:
			return "Equal";
		case FAST_TREE_EQUAL_OR_PREV:
			return "Equal or Prev";
		case FAST_TREE_EQUAL_OR_NEXT:
			return "Equal or Next";
		default:
			return "Unknown Search Type...";
	}

	return "";
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

// returns ops elapsed time in ns
uint64_t do_operation(Fast_Tree * fast_tree, uint8_t op_type, uint64_t operand, uint64_t * result){

	int ret = 0;

	Fast_Tree_Result fast_tree_result;

	struct timespec start, stop;

	clock_gettime(CLOCK_MONOTONIC, &start);
	switch(op_type){
		case INSERT_OP:
			ret = insert_fast_tree(fast_tree, operand, NULL, false, NULL);
			break;
		case REMOVE_OP:
			ret = remove_fast_tree(fast_tree, operand, NULL);
			break;
		case SEARCH_PREV_OP:
			ret = search_fast_tree(fast_tree, operand,  FAST_TREE_EQUAL_OR_PREV, &fast_tree_result);
			break;
		case SEARCH_NEXT_OP:
			ret = search_fast_tree(fast_tree, operand, FAST_TREE_EQUAL_OR_NEXT, &fast_tree_result);
			break;
		default:
			fprintf(stderr, "Error: unknown operation...\n");
			return 0;
	}

	clock_gettime(CLOCK_MONOTONIC, &stop);

	uint64_t timestamp_start = start.tv_sec * 1e9 + start.tv_nsec;
	uint64_t timestamp_stop = stop.tv_sec * 1e9 + stop.tv_nsec;

	uint64_t elapsed_ns = timestamp_stop - timestamp_start;

	if (ret){
		switch(op_type){
			case INSERT_OP:
				// should never happen
				*result = MAX_VAL;
				break;
			case REMOVE_OP:
				*result = MAX_VAL;
				break;
			case SEARCH_PREV_OP:
				*result = MAX_VAL;
				break;
			case SEARCH_NEXT_OP:
				*result = 0;
				break;
			default:
				fprintf(stderr, "Error: unknown operation...\n");
				break;
		}
	}
	else{
		switch(op_type){
			case INSERT_OP:
				// should never happen
				*result = 0;
				break;
			case REMOVE_OP:
				*result = 0;
				break;
			case SEARCH_PREV_OP:
				*result = fast_tree_result.key;
				break;
			case SEARCH_NEXT_OP:
				*result = fast_tree_result.key;
				break;
			default:
				fprintf(stderr, "Error: unknown operation...\n");
				return 0;
		}
	}

	return elapsed_ns;
}

uint64_t get_memory_usage(){

	pid_t pid = getpid();

	char filename[PATH_MAX];
	char line[256];

	long rss;

	 // Construct the filename for the /proc/[pid]/statm file
    snprintf(filename, sizeof(filename), "/proc/%d/statm", pid);

    // Open the file
    FILE * fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening /proc/[pid]/statm");
        return 1;
    }

    // Read the first line (contains the memory information)
    if (fgets(line, sizeof(line), fp) != NULL) {
        // Parse the line (RSS is the second value)
        sscanf(line, "%*d %ld", &rss);
    }

    // Close the file
    fclose(fp);

    // Calculate RSS in bytes (page size is usually 4KB)
    rss *= sysconf(_SC_PAGESIZE);

    return (uint64_t) (rss);
}

int save_results(uint64_t N, uint64_t el_size, char * file_base, char * file_ext, void * arr){

	FILE * fp;

	char * file_path;
	asprintf(&file_path, "%s/%s.%s", SAVE_DIR, file_base, file_ext);
	fp = fopen(file_path, "wb");
	if (!fp){
		fprintf(stderr, "Error: failure to open file for saving arr: %s\n", file_path);
		return -1;
	}

	size_t num_written = fwrite(arr, el_size, N, fp);
	if (num_written != N){
		fprintf(stderr, "Error: failure to save array. Wrote %lu elements, expected: %lu...\n", num_written, N);
		return -1;
	}

	fclose(fp);

	return 0;
}


int main(int argc, char * argv[]){

	struct timespec sim_start, sim_end;

	clock_gettime(CLOCK_MONOTONIC, &sim_start);

	int ret;
	
  	if (argc != 7){
		fprintf(stderr, "Error. Usage: ./testTree <max_capacity> <rand_seed> <search_pct> <search_prev_pct> <removal_pct> <removal_exist_pct>\n");
		return -1;
  	}


  	uint64_t max_capacity = atol(argv[1]);
  	uint64_t rand_seed = atol(argv[2]);
  	int search_pct = atoi(argv[3]);
  	int search_prev_pct = atoi(argv[4]);
  	int removal_pct = atoi(argv[5]);
  	int removal_exist_pct = atoi(argv[6]);


  	char * trace_base;
  	asprintf(&trace_base, "%lu_%lu_%d_%d_%d_%d", max_capacity, rand_seed, search_pct, search_prev_pct, removal_pct, removal_exist_pct);


 	char * ops_filepath;
 	asprintf(&ops_filepath, "%s/%s.ops", TRACE_DIR, trace_base);
 	
 	char * operands_fileapth;
 	asprintf(&operands_fileapth, "%s/%s.operands", TRACE_DIR, trace_base);

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


  	uint64_t * num_els = malloc(num_ops * sizeof(uint64_t));
  	if (!num_els){
  		fprintf(stderr, "Error: malloc failed to alloc num_els\n");
  		return -1;
  	}

  	uint64_t * op_times = malloc(num_ops * sizeof(uint64_t));
  	if (!op_times){
  		fprintf(stderr, "Error: malloc failed to alloc op times\n");
  		return -1;
  	}

  	uint64_t * mem_usage = malloc(num_ops * sizeof(uint64_t));
  	if (!mem_usage){
  		fprintf(stderr, "Error: malloc failed to alloc mem_usage\n");
  		return -1;
  	}

  	uint64_t * op_results = malloc(num_ops * sizeof(uint64_t));
  	if (!op_results){
  		fprintf(stderr, "Error: malloc failed to alloc op results\n");
  		return -1;
  	}

  	Fast_Tree * fast_tree = init_fast_tree();

  	uint64_t total_time = 0;
  	uint64_t max_mem = 0;

  	uint64_t num_inserts = 0;
    uint64_t num_searches = 0;
    uint64_t num_removals = 0;

    int op_type;
  	for (uint64_t i = 0; i < num_ops; i++){
  		
  		// for args 8192, 0, 90, 5, 5, 95 first error occurs when searching prev as op index 320
  		num_els[i] = fast_tree -> cnt;
  		op_times[i] = do_operation(fast_tree, ops[i], operands[i], &op_results[i]);
  		mem_usage[i] = get_memory_usage();
  		total_time += op_times[i];
  		max_mem = MY_MAX(max_mem, mem_usage[i]);
  		op_type = (int) ops[i];
  		switch (op_type){
		    case INSERT_OP:
		      num_inserts++;
		      break;
		    case REMOVE_OP:
		      num_removals++;
		      break;
		    default:
		      num_searches++;
		      break; 
      	}
  	}


  	ret = save_results(num_ops, sizeof(uint64_t), trace_base, "num_els", num_els);
  	if (ret){
  		fprintf(stderr, "Error: failure to save num els array...\n");
  		return -1;
  	}

  	ret = save_results(num_ops, sizeof(uint64_t), trace_base, "op_times", op_times);
  	if (ret){
  		fprintf(stderr, "Error: failure to save num els array...\n");
  		return -1;
  	}

  	ret = save_results(num_ops, sizeof(uint64_t), trace_base, "mem_usage", mem_usage);
  	if (ret){
  		fprintf(stderr, "Error: failure to save mem usage array...\n");
  		return -1;
  	}

  	ret = save_results(num_ops, sizeof(uint64_t), trace_base, "op_results", op_results);
  	if (ret){
  		fprintf(stderr, "Error: failure to save op results...\n");
  		return -1;
  	}	

  	clock_gettime(CLOCK_MONOTONIC, &sim_end);

  	uint64_t sim_start_timestamp = sim_start.tv_sec * 1e9 + sim_start.tv_nsec;
  	uint64_t sim_end_timestamp = sim_end.tv_sec * 1e9 + sim_end.tv_nsec;

  	uint64_t sim_elapsed_ns = sim_end_timestamp - sim_start_timestamp;

  	double sim_elapsed_sec = ((double) sim_elapsed_ns) / 1e9;

    printf("""\n\nSUCCESS -- Fast Tree!\n\tEnv Configuration:\n\t\tMax Capacity: %lu\n\t\tRandom Seed: %lu\n\t\tSearch Prob: %d\n\t\tSearch Prev Prob: %d\n\t\tRemoval Prob: %d\n\t\tRemoval Exists Prob: %d\n \
        \nPerf Results:\n\t\tTotal Operation Time: %f sec\n\t\t\tNum Inserts: %lu\n\t\t\tNum Searches: %lu\n\t\t\tNum Removals: %lu\n\t\tMax Memory Usage: %lu bytes\n\n \
        \nOverall Simulation Time: %f sec\n\n""", max_capacity, rand_seed, search_pct, search_prev_pct, removal_pct, removal_exist_pct, ((double) total_time / 1e9), num_inserts, num_searches, num_removals, max_mem, sim_elapsed_sec);


	return 0;

}