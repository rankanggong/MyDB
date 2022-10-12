#include <gtest/gtest.h>

#include "../../src/storage/rocksdb_keyvaluestore.h";
#include "../../src/util/util.h"

TEST(KVStore, Create) {
	auto kvstore_ = Util::make_unique<Storage::KVStore>(&rocksdb::WriteOptions());
	Status status = kvstore_->OpenDB();
	assert(status.GetCode() == Status::Success);
}