import random
import numpy as np
import time
import os
import sys

## defining op type conversions

INSERT_OP = 0
REMOVE_OP = 1
SEARCH_PREV_OP = 2
SEARCH_NEXT_OP = 3

def simulate_ops(max_capacity, search_prob, search_prev_prob, removal_prob, removal_exists_prob, ops_file, operands_file, rand_seed=0):

	insert_cnt = 0
	search_cnt = 0
	removal_cnt = 0

	random.seed(rand_seed)

	## if we need to can use numpy to generate in bulk faster...
	np.random.seed(rand_seed)

	cur_items = set()
	## for better sampling choose a random index and continuosly
	## check if in current set
	all_inserted_items = []

	## will be converted to a numpy array of type uint8_t (could really use 2 bits but ok)
	## TYPES
	ops = []
	operands = []


	## every "iteration" generate a new random value in the range
	## then within the iteration do 0+ searches and 0+ removals (at most size of cur_items)
	## continue until reached full capacity
	while len(cur_items) < max_capacity:

		insert_item = random.getrandbits(64)
		while insert_item in cur_items:
			insert_item = random.getrandbits(64)

		## perform simulated insertion
		cur_items.add(insert_item)
		all_inserted_items.append(insert_item)
		insert_cnt += 1

		ops.append(INSERT_OP)
		operands.append(insert_item)
		

		search_sample = random.random()
		## going to generate new search now
		## can search from any query point
		while (search_sample < search_prob):
			search_item = random.getrandbits(64)

			## choose direction to search
			dir_sample = random.random()
			if (dir_sample < search_prev_prob):
				ops.append(SEARCH_PREV_OP)
			else:
				ops.append(SEARCH_NEXT_OP)

			operands.append(search_item)
			search_cnt += 1

			## potentially search again
			search_sample = random.random()


		removal_sample = random.random()

		## if we are going to remove an item
		while ((removal_sample < removal_prob) and (len(cur_items) > 0)):

			## some of these will alrady have been removed but
			## a convenient way to sample more efficiently...
			max_items = len(all_inserted_items) - 1

			remove_exist = False

			exist_sample = random.random()
			if (exist_sample < removal_exists_prob):
				remove_exist = True

			## will be very slow with high removal rate...
			## and would need better structure to choose what item
			## to remove...
			removal_item = all_inserted_items[random.randint(0, max_items)]
			while (remove_exist and (removal_item not in cur_items)):
				removal_item = all_inserted_items[random.randint(0, max_items)]

			## perform removal
			if (remove_exist):
				cur_items.remove(removal_item)

			removal_cnt += 1

			ops.append(REMOVE_OP)
			operands.append(removal_item)

			## potentially remove again
			removal_sample = random.random()


		## now insert a new item

	## reached max capacity
	## save down ops and data vals as a simulated dataset
	ops_arr = np.array(ops, dtype=np.uint8)
	ops_arr.tofile(ops_file)

	operands_arr = np.array(operands, dtype=np.uint64)
	operands_arr.tofile(operands_file)

	return insert_cnt, search_cnt, removal_cnt



def build_trace(max_capacity, search_prob, search_prev_prob, removal_prob, removal_exists_prob, rand_seed=0, trace_dir="./traces", to_print=True):

	trace_name = f"{max_capacity}_{rand_seed}_{int(search_prob * 100)}_{int(search_prev_prob * 100)}_{int(removal_prob * 100)}_{int(removal_exists_prob * 100)}"
	
	if trace_dir[-1] == "/":
		trace_dir = trace_dir[:-1]

	ops_file = trace_dir + "/" + trace_name + ".ops"
	operands_file = trace_dir + "/" + trace_name + ".operands"

	if (os.path.exists(ops_file)):
		print(f"""Not Simulating, already exists...
				\n\tConfiguration:\n\t\tMax Capacity: {max_capacity}\n\t\tRandom Seed: {rand_seed}\n\t\tSearch Prob: {search_prob}\n\t\tSearch Prev Prob: {search_prev_prob}\n\t\tRemoval Prob: {removal_prob}\n\t\tRemoval Exists Prob: {removal_exists_prob}""")
		return 0, 0, 0

	start = time.time_ns()
	num_inserts, num_searches, num_removals = simulate_ops(max_capacity, search_prob, search_prev_prob, removal_prob, removal_exists_prob, ops_file, operands_file, rand_seed)
	end = time.time_ns()

	elapsed_ms = int((end - start) // 1e6)

	if (to_print):
		print(f"""\nBuilt Trace:
				\n\tConfiguration:\n\t\tMax Capacity: {max_capacity}\n\t\tRandom Seed: {rand_seed}\n\t\tSearch Prob: {search_prob}\n\t\tSearch Prev Prob: {search_prev_prob}\n\t\tRemoval Prob: {removal_prob}\n\t\tRemoval Exists Prob: {removal_exists_prob}
				\n\tSimulated Ops:\n\t\t# Ops: {num_inserts + num_searches + num_removals}\n\t\t\t# Inserts: {num_inserts}\n\t\t\t# Searches: {num_searches}\n\t\t\t# Removals: {num_removals}
				\n\tSimulated Runtime: {elapsed_ms} ms\n\n\n""")

	return num_inserts, num_searches, num_removals





## Calling BUILD!
to_print_env_summary = True

## can keep these fixed
rand_seed = 0
trace_dir = "./traces"

max_capacities = [1 << i for i in range(10, 30)]
search_probs = [.1, .25, .5, .75, .9]
search_prev_probs = [.05, .25, .5]
removal_probs = [0, .05, .1, .25, .4]
removal_exists_probs = [0, .05, .25, .5, .75, .95]


total_envs = len(max_capacities) * len(search_probs) * len(search_prev_probs) * len(removal_probs) * len(removal_exists_probs)

print(f"Total Environments: {total_envs}")

env_num = 0

for max_capacity in max_capacities:
	for search_prob in search_probs:
		for search_prev_prob in search_prev_probs:
			for removal_prob in removal_probs:
				for removal_exists_prob in removal_exists_probs:
					print(f"ENVIRONMENT #{env_num}/{total_envs}...\n\n")
					num_inserts, num_searches, num_removals = build_trace(max_capacity, search_prob, search_prev_prob, removal_prob, removal_exists_prob, 
																rand_seed=rand_seed, trace_dir=trace_dir, to_print=to_print_env_summary)
					env_num += 1











