#include "core/types.h"
#include "core/hashmap.h"
#include "core/vector.h"

// struct foobar {
//     s32 id;
//     const char* name;
//     f32 value;
// };

// int main(int argc, char* argv[])
// {
//     hashmap_t map;
//     hashmap_new(&map, sizeof(struct foobar), 5);
//     struct foobar f = {
//         .id = 1,
//         .name = "Foobar",
//         .value = 3.14159f,
//     };
//     u32 h1 = hash_string("foob", strlen("foob"));
//     u32 h2 = hash_string("food", strlen("food"));
//     hashmap_insert(&map, "foo", &f);

//     return 0;
// }

#include <tau/tau.h>

TAU_MAIN()

TEST(core_vector_suite, basic)
{
	int a = 42;
	int b = 13;
	struct vector v;
	vector_init(&v);
	vector_push_back(&v, &a, sizeof(int));
	vector_push_back(&v, &b, sizeof(int));
	CHECK_EQ(v.num_elems, 2);
	int* f = (int*)vector_find(&v, sizeof(int), &b);
	CHECK_NOT_NULL(f);
	if (f != NULL)
		CHECK_EQ(*f, 13);
	vector_pop_back(&v, sizeof(int));
	CHECK_EQ(v.num_elems, 1);
	vector_pop_back(&v, sizeof(int));
	CHECK_EQ(v.num_elems, 0);
	CHECK_EQ(v.capacity, 2);

	for (size_t i = 0; i < 256; i++) {
		vector_push_back(&v, (void*)(int*)(&i), sizeof(int));
		int val = *(int*)vector_elem(&v, sizeof(int), i);
		CHECK_EQ(val, i);
	}
	CHECK_EQ(v.num_elems, 256);
	CHECK_EQ(v.capacity, 256);
}

struct test_data {
	int id;
	char name[256];
	float data;
};

TEST(core_hashmap_suite, basic)
{
	struct hashmap map;
	hashmap_new(&map, 16);
	struct test_data td1, td2;
	memset(&td1, 0, sizeof(td1));
	memset(&td2, 0, sizeof(td2));
	td1.id = 1;
	td2.id = 2;
	strcpy(td1.name, "test_data_1");
	strcpy(td2.name, "test_data_2");
	td1.data = 3.14f;
	td2.data = 6.28f;
	hashmap_insert(&map, "foo", &td1, sizeof(struct test_data));
	hashmap_insert(&map, "bar", &td2, sizeof(struct test_data));
	struct test_data* found = NULL;
	hashmap_find(&map, "bar", (struct test_data*)&found);
}
// TEST(core_vector_suite, basic_tests2) {
//     int a = 42;
//     int b = 13;
//     struct vector v;
//     vector_push_back(&v, &a, sizeof(int));
//     vector_push_back(&v, &b, sizeof(int));
//     CHECK_EQ(v.num_elems, 4);
// }
// TEST(foo, bar2) {
//     char* a = "foo";
//     char* b = "foobar";
//     REQUIRE_STREQ(a, a); // pass :)
//     REQUIRE_STREQ(a, b); // fail - Test suite aborted
// }
