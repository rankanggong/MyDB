#pragma once

#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/table.h>

#include "../model/status.h"
// #include "../model/config.h"
// #include "lock_mgt.h"

namespace Storage {
class KVStore {
	public: 
		KVStore(rocksdb::WriteOptions *options);
		~KVStore();

		void SetWriteOptions(const rocksdb::WriteOptions& options);
		Status OpenDB();
		void CloseDB();
		Status CreateColumnFamilies(const rocksdb::Options& options);
		Status Recovery();
		rocksdb::Status Write(const rocksdb::WriteOptions& options, rocksdb::WriteBatch* writeBatch);
		const rocksdb::WriteOptions& DefaultWriteOptions() { return write_options_; }
		rocksdb::Status Delete(const rocksdb::WriteOptions& options,
			rocksdb::ColumnFamilyHandle* cf_handle,
			const rocksdb::Slice& key);
		rocksdb::Status Compact(const rocksdb::Slice* begin, const rocksdb::Slice* end);
		rocksdb::DB* GetDB();
		rocksdb::ColumnFamilyHandle* GetCFHandle();
		// LockMgt *GetLockMgt { return lock_mgt_; }
		rocksdb::Options GetDefaultOptions();
		rocksdb::BlockBasedTableOptions GetDefaultTableOptions();

		uint64_t GetFlushCount() { return flush_count_; }
		void IncreaseFlushCount(uint64_t n) { flush_count_.fetch_add(n); }
		uint64_t GetCompactionCount() { return compaction_count_; }
		void IncreaseCompactionCount(uint64_t n) { compaction_count_.fetch_add(n); }

		KVStore(const KVStore&) = delete;
		KVStore& operator=(const KVStore &) = delete;

	private:
		rocksdb::DB* db_ = nullptr;
		rocksdb::WriteOptions write_options_ = rocksdb::WriteOptions();
		rocksdb::Env* env_;
		std::shared_ptr<rocksdb::SstFileManager> sst_file_manager_;
		// LockMgt lock_mgt_;
		// Config* config_ = nullptr;
		std::vector<rocksdb::ColumnFamilyHandle*> cf_handles_;
		std::atomic<uint64_t> flush_count_{ 0 };
		std::atomic<uint64_t> compaction_count_{ 0 };
		std::atomic<bool> db_error_{ false };
		bool closing_ = true;
};
}