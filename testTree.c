#include "fast_tree.h"

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


int main(int argc, char * argv[]){

	int ret;

	uint64_t key;
	uint64_t search_key;
	uint64_t search_result;
	uint64_t key_to_remove;

	Fast_Tree * fast_tree = init_fast_tree();

	key = 3;
	printf("Inserting Key: %lu...\n", key);
	ret = insert_fast_tree(fast_tree, key, NULL, false, NULL);
	if (ret){
		fprintf(stderr, "Error: unable to insert key: %lu\n", key);
		return -1;
	}

	key = 4;
	printf("Inserting Key: %lu...\n", key);
	ret = insert_fast_tree(fast_tree, key, NULL, false, NULL);
	if (ret){
		fprintf(stderr, "Error: unable to insert key: %lu\n", key);
		return -1;
	}

	key = 5;
	printf("Inserting Key: %lu...\n", key);
	ret = insert_fast_tree(fast_tree, key, NULL, false, NULL);
	if (ret){
		fprintf(stderr, "Error: unable to insert key: %lu\n", key);
		return -1;
	}


	search_key = 4;

	// Search modifiers...

	/* 
	FAST_TREE_MIN,
	FAST_TREE_MAX,
	FAST_TREE_PREV,
	FAST_TREE_NEXT,
	FAST_TREE_EQUAL,
	FAST_TREE_EQUAL_OR_PREV,
	FAST_TREE_EQUAL_OR_NEXT
	*/

	// only care about the fast_tree_result.key value
	// not worrying about dictionary values...
	Fast_Tree_Result fast_tree_result;

	FastTreeSearchModifier search_mod = FAST_TREE_EQUAL_OR_PREV;

	printf("Search Mod: %d", search_mod);

	ret = search_fast_tree(fast_tree, search_key, search_mod, &fast_tree_result);
	if (ret){
		fprintf(stderr, "Error: no keys matched the searched criteria: Search Key: %lu, Search Type: %s\n", search_key, search_type_to_str(search_mod));
		return -1;
	}
	search_result = fast_tree_result.key;
	printf("\nSearch Result!:\n\tSearch Key: %lu\n\tSearch Type: %s\n\tSearch Result: %lu\n\n", search_key, search_type_to_str(search_mod), search_result);


	key_to_remove = 4;
	
	printf("\nRemoving Key: %lu...\n", key_to_remove);
	ret = remove_fast_tree(fast_tree, key_to_remove, NULL);
	if (ret){
		fprintf(stderr, "Error: unable to remove key: %lu from tree...\n", key_to_remove);
		return -1;
	}


	ret = search_fast_tree(fast_tree, search_key, search_mod, &fast_tree_result);
	if (ret){
		fprintf(stderr, "Error: no keys matched the searched criteria: Search Key: %lu, Search Type: %s\n", search_key, search_type_to_str(search_mod));
		return -1;
	}
	search_result = fast_tree_result.key;
	printf("\nSearch Result!:\n\tSearch Key: %lu\n\tSearch Type: %s\n\tSearch Result: %lu\n\n", search_key, search_type_to_str(search_mod), search_result);


	return 0;

}