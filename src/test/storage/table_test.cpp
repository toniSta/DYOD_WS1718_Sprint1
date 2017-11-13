#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../base_test.hpp"
#include "gtest/gtest.h"

#include "../lib/resolve_type.hpp"
#include "../lib/storage/dictionary_column.hpp"
#include "../lib/storage/table.hpp"

namespace opossum {

class StorageTableTest : public BaseTest {
 protected:
  void SetUp() override {
    t.add_column("col_1", "int");
    t.add_column("col_2", "string");
  }

  Table t{2};
};

TEST_F(StorageTableTest, ChunkCount) {
  EXPECT_EQ(t.chunk_count(), 1u);
  t.append({4, "Hello,"});
  t.append({6, "world"});
  t.append({3, "!"});
  EXPECT_EQ(t.chunk_count(), 2u);
}

TEST_F(StorageTableTest, GetChunk) {
  t.get_chunk(ChunkID{0});
  // TODO(anyone): Do we want checks here?
  // EXPECT_THROW(t.get_chunk(ChunkID{q}), std::exception);
  t.append({4, "Hello,"});
  t.append({6, "world"});
  t.append({3, "!"});
  t.get_chunk(ChunkID{1});
}

TEST_F(StorageTableTest, ColCount) { EXPECT_EQ(t.col_count(), 2u); }

TEST_F(StorageTableTest, RowCount) {
  EXPECT_EQ(t.row_count(), 0u);
  t.append({4, "Hello,"});
  t.append({6, "world"});
  t.append({3, "!"});
  EXPECT_EQ(t.row_count(), 3u);
}

TEST_F(StorageTableTest, GetColumnName) {
  EXPECT_EQ(t.column_name(ColumnID{0}), "col_1");
  EXPECT_EQ(t.column_name(ColumnID{1}), "col_2");
  // TODO(anyone): Do we want checks here?
  // EXPECT_THROW(t.column_name(ColumnID{2}), std::exception);
}

TEST_F(StorageTableTest, GetColumnType) {
  EXPECT_EQ(t.column_type(ColumnID{0}), "int");
  EXPECT_EQ(t.column_type(ColumnID{1}), "string");
  // TODO(anyone): Do we want checks here?
  // EXPECT_THROW(t.column_type(ColumnID{2}), std::exception);
}

TEST_F(StorageTableTest, GetColumnIdByName) {
  EXPECT_EQ(t.column_id_by_name("col_2"), 1u);
  EXPECT_THROW(t.column_id_by_name("no_column_name"), std::exception);
}

TEST_F(StorageTableTest, GetChunkSize) { EXPECT_EQ(t.chunk_size(), 2u); }

TEST_F(StorageTableTest, CompressChunk) {
  t.append({4, "Hello,"});
  t.append({6, "world"});
  t.append({3, "!"});
  t.compress_chunk(ChunkID(0));
  EXPECT_EQ(t.chunk_count(), 2u);
  auto col = t.get_chunk(ChunkID(0)).get_column(ColumnID(0));
  auto dict = std::dynamic_pointer_cast<DictionaryColumn<int>>(col);
  EXPECT_EQ(dict->get(0), 4);
}

TEST_F(StorageTableTest, NoDuplicateTableNames) { EXPECT_THROW(t.add_column("col_1", "int"), std::exception); }

TEST_F(StorageTableTest, AddColumnsToEmptyTablesOnly) {
  t.add_column("col_3", "int");
  t.append({4, "Hello,", 1});
  t.append({6, "world", 2});
  EXPECT_THROW(t.add_column("col_4", "int"), std::exception);
}

TEST_F(StorageTableTest, ColumnNames) {
  std::vector<std::string> column_names = {"col_1", "col_2"};
  EXPECT_EQ(t.column_names(), column_names);
}

}  // namespace opossum
