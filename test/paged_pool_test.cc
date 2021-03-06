#include <gtest/gtest.h>

#include "db/paged_pool.h"

#include <cstring>
#include <iostream>

using namespace portal_db;

TEST(PagedPoolTest, RAII) {
  PagedPool<256> pool("unique");
  for(int i = 0; i < pool.capacity() / 4; i++) {
    size_t token = pool.New();
    char* tmp = pool.Get(token);
    EXPECT_TRUE(tmp != NULL);
    memset(tmp, 0, sizeof(char) * 256);
    EXPECT_EQ(pool.size(), i + 1);
  }
}

TEST(PagedPoolTest, Retrieval) {
  PagedPool<32> pool("unique");
  for(size_t i = 0; i < pool.capacity() / 4; i++) {
    size_t token = pool.New();
    char* tmp = pool.Get(token);
    EXPECT_TRUE(tmp != NULL);
    memcpy(tmp, reinterpret_cast<char*>(&i), sizeof(char) * 4);
  }
  for(size_t i = 0; i < pool.capacity() / 4; i++) {
    char* tmp = pool.Get(i);
    size_t retrieve = *reinterpret_cast<size_t*>(tmp);
    EXPECT_EQ(retrieve, i);
  }
}

TEST(PagedPoolTest, Snapshot) {
  PagedPool<32> pool("unique");
  for(size_t i = 0; i < pool.capacity() / 4; i++) {
    size_t token = pool.New();
    char* tmp = pool.Get(token);
    EXPECT_TRUE(tmp != NULL);
    memcpy(tmp, reinterpret_cast<char*>(&i), sizeof(char) * 4);
  }
  EXPECT_TRUE(pool.MakeSnapshot().inspect());
  pool.Close();
  PagedPool<32> shadow("unique");
  EXPECT_TRUE(shadow.ReadSnapshot().inspect());
  for(size_t i = 0; i < shadow.capacity() / 4; i++) {
    char* tmp = shadow.Get(i);
    size_t retrieve = *reinterpret_cast<size_t*>(tmp);
    EXPECT_EQ(retrieve, i);
  }
  EXPECT_TRUE(shadow.DeleteSnapshot().inspect());
}