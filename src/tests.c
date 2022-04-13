#include "core/types.h"
#include "core/memory.h"
#include "core/hashmap.h"
#include "core/random.h"
#include "platform/platform.h"
#include "core/string.h"
#include "core/vector.h"

struct foobar {
	s32 id;
	f32 value;
	char name[256];
};

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

TEST(core_memory_suite, basic)
{
	size_t sz = 0x800000;
	char* scratch = malloc(sz);
	char* dest = malloc(sz);
	char* dest2 = malloc(sz);
	char* dest3 = malloc(sz);

	int* p = (int*)&scratch[0];
	for (int i = 0; i < sz; i += 4) {
		int val = random(INT32_MIN, INT32_MAX);
		*p = val;
		CHECK_EQ(*(int*)(&scratch[i]), val);
		p++;
	}

	u64 s = os_get_time_ns();
	mem_copy((void*)dest, (void*)scratch, sz);
	u64 e = os_get_time_ns() - s;
	printf("mem_copy: %lldns\n", e);

	s = os_get_time_ns();
	mem_copy_sse2((void*)dest2, (void*)scratch, sz);
	e = os_get_time_ns() - s;
	printf("mem_copy_sse2: %lldns\n", e);

	s = os_get_time_ns();
	memcpy((void*)dest3, (void*)scratch, sz);
	e = os_get_time_ns() - s;
	printf("memcpy: %lldns\n", e);

	CHECK(memcmp(dest3, dest, sz) == 0);
	CHECK(memcmp(dest3, dest2, sz) == 0);
	free(scratch);
	free(dest);
	free(dest2);
	scratch = NULL;
	dest = NULL;
	dest2 = NULL;
}

TEST(core_string_suite, basic)
{
	string_t s;
	str_init(&s);
	CHECK_EQ(s.capacity, 0);
	CHECK_EQ(s.size, 0);
	CHECK_NULL(s.data);
	CHECK_NULL(s.refs);
	for (size_t i = 0; i < SSO_SIZE; i++)
		CHECK_EQ(s.text[i], CSTR_NULL);

	str_reserve(&s, 8);
	CHECK_EQ(s.capacity, 16);
	str_reserve(&s, 32);
	CHECK_EQ(s.capacity, 64);
	str_reserve(&s, 128);
	CHECK_EQ(s.capacity, 256);

	str_assign(&s, "Hello, string!");
}

TEST(core_vector_suite, basic)
{
	int a = 42;
	int b = 13;
	struct vector v;
	vector_init(&v);
	vector_push_back(&v, &a, sizeof(int));
	vector_push_back(&v, &b, sizeof(int));
	CHECK_EQ(v.num_elems, 2);
	size_t b_index = vector_find(&v, sizeof(int), &b, 1);
	CHECK_EQ(b_index, 1);
	if (b_index == 1) {
		int* b_elem = (int*)vector_elem(&v, sizeof(int), 1);
		if (b_elem != NULL)
			CHECK_EQ(*b_elem, 13);
	}
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

	struct foobar foo;
	struct foobar bar;
	struct foobar* baz = (struct foobar*)malloc(sizeof(*baz));
	foo.id = 5;
	bar.id = 99;
	baz->id = 42;
	foo.value = 3.14159f;
	bar.value = (3.14159f * 2.f);
	baz->value = 1.234f;
	strcpy(foo.name, "foo");
	strcpy(bar.name, "bar");
	strcpy(baz->name, "baz_ptr");
	VECTOR(struct foobar) foo_list;
	vec_init(foo_list);
	vec_push_back(foo_list, &foo);
	vec_push_back(foo_list, baz);
	vec_push_back(foo_list, &bar);

	struct foobar* foo_get = (struct foobar*)&foo_list.elems[0];
	struct foobar* baz_get = (struct foobar*)&foo_list.elems[1];
	struct foobar* bar_get = (struct foobar*)&foo_list.elems[2];
	CHECK(foo_get->id == foo.id);
	CHECK(bar_get->id == bar.id);
	CHECK(foo_get->value == foo.value);
	CHECK(bar_get->value == bar.value);
	// CHECK(strcmp(foo_get->name, foo.name) == 0)
	vec_free(foo_list);
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
	hash_key_t k1 = {
		"foo",
		strlen("foo"),
	};
	hash_key_t k2 = {
		"bar",
		strlen("bar"),
	};
	hashmap_insert(&map, &k1, &td1, sizeof(struct test_data));
	hashmap_insert(&map, &k2, &td2, sizeof(struct test_data));
	struct test_data* found = NULL;
	hashmap_find(&map, &k2, (struct test_data*)&found);
	CHECK_EQ(td2.id, found->id);
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
