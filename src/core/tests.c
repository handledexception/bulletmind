#include "core/types.h"
#include "core/buffer.h"
#include "core/memory.h"
#include "core/hashmap.h"
#include "core/random.h"
#include "core/string.h"
#include "core/timer.h"
#include "core/vector.h"
#include "platform/platform.h"

#include <psi/psi.h>

PSI_MAIN()

TEST(core_buffer, basic)
{
	buffer_t* buf = buffer_new(0x400);
	CHECK_EQ(buf->size, 0x400);
	CHECK_EQ(buf->capacity, 0x400);
	CHECK_EQ(buf->pos, 0);
	buffer_free(buf);
}
// TEST(core_memory_suite, basic)
// {
// 	size_t sz = 0x800000;
// 	char* scratch = mem_alloc(sz);
// 	char* dest = mem_alloc(sz);
// 	char* dest2 = aligned_malloc(sz, 16);
// 	char* dest3 = mem_alloc(sz);

// 	int* p = (int*)&scratch[0];
// 	for (int i = 0; i < sz; i += 4) {
// 		int val = random(INT32_MIN, INT32_MAX);
// 		*p = val;
// 		CHECK_EQ(*(int*)(&scratch[i]), val);
// 		p++;
// 	}

// 	u64 s = os_get_time_ns();
// 	mem_copy((void*)dest, (void*)scratch, sz);
// 	u64 e = os_get_time_ns() - s;
// 	printf("mem_copy: %lldns\n", e);

// 	s = os_get_time_ns();
// 	mem_copy_sse2_aligned((void*)dest2, (void*)scratch, sz);
// 	e = os_get_time_ns() - s;
// 	printf("mem_copy_sse2_aligned: %lldns\n", e);

// 	s = os_get_time_ns();
// 	memcpy((void*)dest3, (void*)scratch, sz);
// 	e = os_get_time_ns() - s;
// 	printf("memcpy: %lldns\n", e);

// 	CHECK(memcmp(dest3, dest, sz) == 0);
// 	CHECK(memcmp(dest3, dest2, sz) == 0);
// 	free(scratch);
// 	free(dest);
// 	aligned_free(dest2);
// 	scratch = NULL;
// 	dest = NULL;
// 	dest2 = NULL;
// }

TEST(core_string_suite, basic)
{
	string_t s;
	str_init(&s);
	CHECK_EQ(s.capacity, 0);
	CHECK_EQ(s.size, 0);
	CHECK_NULL(s.data);
	CHECK_NULL(s.refs);
	for (size_t i = 0; i < SSO_SIZE; i++)
		CHECK_EQ(s.data_sso[i], CSTR_NULL);

	str_reserve(&s, 8);
	CHECK_EQ(s.capacity, 16);
	str_reserve(&s, 32);
	CHECK_EQ(s.capacity, 64);
	str_reserve(&s, 128);
	CHECK_EQ(s.capacity, 256);

	str_copy_cstr(&s, "Hello, string!", 14);
}

TEST(core_vector_suite, basic)
{
	// vector_init
	{
		struct vector v;
		vector_init(&v);
		CHECK_EQ(v.elems, NULL);
		CHECK_EQ(v.num_elems, 0);
		CHECK_EQ(v.capacity, 0);
	}

	// vector_push_back
	{
		struct vector v;
		vector_init(&v);
		int a = 42;
		int b = 69;
		vector_push_back(&v, &a, sizeof(int));
		CHECK_EQ(v.num_elems, 1);
		CHECK_EQ(v.capacity, 1);
		vector_push_back(&v, &b, sizeof(int));
		CHECK_EQ(v.num_elems, 2);
		CHECK_EQ(v.capacity, 2);
	}

	// vector_find
	{
		// 1. integer
		struct vector v;
		vector_init(&v);
		int a = 42;
		int b = 69;
		vector_push_back(&v, &a, sizeof(int));
		vector_push_back(&v, &b, sizeof(int));
		int c = 0;
		size_t bdx = vector_find(&v, &b, sizeof(int), 1);
		CHECK_EQ(bdx, 1);
	}

	{
		// 2. structured data
		struct foo {
			int id;
			float val;
			char str[16];
		};

		struct foo f;
		f.id = 53;
		f.val = 3.14159f;
		strcpy(&f.str[0], "hello\0");

		struct foo f2;
		f2.id = 101;
		f2.val = 6.022141f;
		strcpy(&f2.str[0], "world\0");

		struct vector v;
		vector_init(&v);

		vector_push_back(&v, &f, sizeof(struct foo));
		vector_push_back(&v, &f2, sizeof(struct foo));

		size_t bdx = vector_find(&v, &f2, sizeof(struct foo), 1);
		CHECK_EQ(bdx, 1);
		struct foo* fptr = (struct foo*)vector_elem(&v, sizeof(struct foo), bdx);
		CHECK_EQ(fptr->id, f2.id);
		REQUIRE_STREQ(&fptr->str[0], &f2.str[0]);
		CHECK_EQ(strcmp(&fptr->str[0], &f2.str[0]), 0);
	}
}

TEST(core_hashmap_suite, basic)
{
	typedef struct test_data {
		int id;
		char name[256];
		float data;
	} test_data_t;
	hashmap_t* map = hashmap_new();
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
	hashmap_insert(map, k1, &td1, sizeof(struct test_data));
	hashmap_insert(map, k2, &td2, sizeof(struct test_data));
	struct test_data* found = NULL;
	hashmap_find(map, k2, &found);
	CHECK_EQ(td2.id, found->id);
	REQUIRE_STREQ(&td2.name[0], &found->name[0]);

	// size_t tds = sizeof(test_data_t);
	// HASHMAP(test_data_t*) td_map;
	// map_init(td_map);
	// map_insert(td_map, &k1, &td1);
	// hashmap_find(&td_map, &k1, &found);
	// printf("hello");
}

TEST(timer_suite, basic)
{
	timer_t t;
	timer_start(&t);
	os_sleep_ms(100);
	timer_stop(&t);
	u64 elapsed = timer_elapsed(&t, TIMER_PRECISION_MILLISECONDS);
	CHECK_GE(elapsed, 95);
	CHECK_LE(elapsed, 105);
}
