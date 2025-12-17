/*
 * Copyright (c) 2025, Friedt Professional Engineering Services, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <zephyr/ztest.h>
#include <zephyr/sys/elastipool.h>
#include <zephyr/toolchain.h>

struct elem {
	uint64_t blob[2];
};

SYS_ELASTIPOOL_DEFINE(static_pool, sizeof(struct elem), __alignof(struct elem), 1, 1);
SYS_ELASTIPOOL_DEFINE_STATIC(dynamic_pool, sizeof(struct elem), __alignof(struct elem), 0, 1);
SYS_ELASTIPOOL_DEFINE_ADVANCED(
	elastic_pool, sizeof(struct elem), __alignof(struct elem), 1,
	(size_t)CONFIG_TEST_ELASTIC_POOL_MAX_ELEMS,
	SYS_ELASTIPOOL_HEAP_ALLOC_FN(1, (size_t)CONFIG_TEST_ELASTIC_POOL_MAX_ELEMS), static);

ZTEST(elastipool, test_sys_elastipool_init)
{
	struct sys_elastipool pool;
	struct sys_elastipool_api api;
	struct sys_elastipool_config config;
	struct sys_elastipool_data data;

	zexpect_ok(sys_elastipool_init(
		&pool, &api, &config, &data, sizeof(struct elem), __alignof(struct elem), 1,
		CONFIG_TEST_ELASTIC_POOL_MAX_ELEMS, elastic_pool.config->storage,
		elastic_pool.config->bmp, elastic_pool.config->map, elastic_pool.api->heap_alloc));

	zexpect_mem_equal(&api, elastic_pool.api, sizeof(api));
	zexpect_mem_equal(&config, elastic_pool.config, sizeof(config));
}

ZTEST(elastipool, test_sys_elastipool_alloc)
{
	void *elem, *elem2;

	zexpect_ok(sys_elastipool_alloc(&static_pool, &elem));
	zexpect_equal(-ENOMEM, sys_elastipool_alloc(&static_pool, &elem));

	zexpect_ok(sys_elastipool_alloc(&dynamic_pool, &elem));
	zexpect_equal(-ENOMEM, sys_elastipool_alloc(&dynamic_pool, &elem));

	zexpect_ok(sys_elastipool_alloc(&elastic_pool, &elem));
	zexpect_ok(sys_elastipool_alloc(&elastic_pool, &elem2));
}

ZTEST(elastipool, test_sys_elastipool_free)
{
	void *elem, *elem2;

	zexpect_ok(sys_elastipool_alloc(&static_pool, &elem));
	zexpect_ok(sys_elastipool_free(&static_pool, elem));
	zexpect_equal(-EINVAL, sys_elastipool_free(&static_pool, (void *)0x4242));

	zexpect_ok(sys_elastipool_alloc(&dynamic_pool, &elem));
	zexpect_ok(sys_elastipool_free(&dynamic_pool, elem));
	zexpect_equal(-EINVAL, sys_elastipool_free(&dynamic_pool, (void *)0x4242));

	zexpect_ok(sys_elastipool_alloc(&elastic_pool, &elem));
	zexpect_ok(sys_elastipool_alloc(&elastic_pool, &elem2));
	zexpect_ok(sys_elastipool_free(&elastic_pool, elem));
	zexpect_ok(sys_elastipool_free(&elastic_pool, elem2));
	zexpect_equal(-EINVAL, sys_elastipool_free(&elastic_pool, (void *)0x4242));
}

ZTEST(elastipool, test_sys_elastipool_clear)
{
	void *elem;
	const struct sys_elastipool *pools[] = {
		&static_pool,
		&dynamic_pool,
	};

	/* test clear for the fixed-size pools (limit is based on available memory) */
	ARRAY_FOR_EACH(pools, i) {
		const struct sys_elastipool *const pool = pools[i];

		/* allocate all blocks */
		for (size_t i = 0; i < pool->config->max_obj; i++) {
			zexpect_ok(sys_elastipool_alloc(pool, &elem));
		}
		/* check that we have used all of the available blocks */
		zexpect_equal(pool->data->pool_size, pool->config->max_obj);
		/* clear all allocations */
		sys_elastipool_clear(pool);
		/* check that all of the available blocks are free */
		zexpect_equal(pool->data->pool_size, 0);
		/* re-allocate all blocks */
		for (size_t i = 0; i < pool->config->max_obj; i++) {
			zexpect_ok(sys_elastipool_alloc(pool, &elem));
		}
	}

	/* test clear for the elastic allocation strategy (limit is based on available memory) */

	/* allocate as many blocks as possible */
	size_t count = 0;

	for (size_t i = 0; i < elastic_pool.config->max_obj; i++, ++count) {
		if (sys_elastipool_alloc(&elastic_pool, &elem) < 0) {
			zassert_true(count >= 2);
			break;
		}
	}

	TC_PRINT("Allocated %zu objects of size %zu\n", count, sizeof(struct elem));

	/* clear all allocations */
	sys_elastipool_clear(&elastic_pool);
	/* check that all of the available blocks are free */
	zexpect_equal(elastic_pool.data->pool_size, 0);
	/* re-allocate as many blocks as possible */
	for (size_t i = 0; i < elastic_pool.config->max_obj; i++) {
		if (sys_elastipool_alloc(&elastic_pool, &elem) < 0) {
			zassert_true(i == count, "failed to re-allocate object %zu", i);
			break;
		}
	}

	TC_PRINT("Re-allocated %zu objects of size %zu\n", count, sizeof(struct elem));
}

static void after(void *arg)
{
	sys_elastipool_clear(&static_pool);
	sys_elastipool_clear(&dynamic_pool);
	sys_elastipool_clear(&elastic_pool);
}

ZTEST_SUITE(elastipool, NULL, NULL, NULL, after, NULL);
