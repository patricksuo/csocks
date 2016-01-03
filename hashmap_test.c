#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "malloc.h"
#include "hashmap.h"

/*
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
*/

#define MAP_CAP  32
#define DATASET_SIZE 100

static int test_status[DATASET_SIZE];
/*
 *  generate test data in python:
 * import string, random
 * string.join([str(x) for x in [random.randrange(1,1000) for _ in range(100)]], ",")
 */
static int test_data[DATASET_SIZE] = {
216,16,894,873,902,606,758,822,514,342,179,318,290,951,396,865,739,459,210,668,60,864,918,237,189,101,873,823,861,667,243,870,364,69,23,576,219,84,579,994,744,103,560,141,588,756,674,52,240,886,11,167,217,166,126,158,247,734,370,262,296,810,477,430,410,105,859,923,177,279,634,986,434,865,634,16,938,906,4,756,382,883,818,525,16,561,422,493,725,901,73,316,974,506,68,868,515,509,598,233,
};


int main(int argc, char **argv) {
	int i, key, inmap, insert, data;
	void *datap;
	struct hashmap_t *map = hashmap_new(5);
	srandom(0);
	for (i=0; i<10000; i++) {
		key = rand()%DATASET_SIZE;
		insert = rand()%2;
		inmap = test_status[key];
		data = test_data[key];
		if (inmap && insert) { 
			/* update */
			data++;
			hashmap_upsert(map, &key, sizeof(int), INT2PTR(data),  &datap);
			assert((intptr_t)datap == (intptr_t)test_data[key]);
			test_data[key] = data;
		} else if ( !inmap && insert) {
			/* insert */
			hashmap_upsert(map, &key, sizeof(int), INT2PTR(data),  &datap);
			assert( (intptr_t)datap == 0);
			test_status[key] = 1;
		} else if (inmap && !insert) {
			/* delete */
			hashmap_delete(map, &key, sizeof(int), &datap);
			assert((intptr_t)datap == (intptr_t)test_data[key]);
			test_status[key] = 0;
		} else if (!inmap && !insert) {
			/* nothing to be deleted */
			hashmap_delete(map, &key, sizeof(int), &datap);
			assert((intptr_t)datap == 0);
		}
	}
	return 0;
}
