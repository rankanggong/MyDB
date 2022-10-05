#include "rocksdb_keyvaluestore.h"

#include <memory>
#include <utility>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <rocksdb/convenience.h>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/sst_file_manager.h>

namespace Storage {
    using rocksdb::Slice;

    const char *db_dir = "./db";
    const char *dummy_cf = "dummy_cf";

    KVStore::KVStore(rocksdb::WriteOptions* options)
        : env_(rocksdb::Env::Default()) {
        SetWriteOptions(*options);
    };

    KVStore::~KVStore() {
        CloseDB();
    }

    void KVStore::SetWriteOptions(const rocksdb::WriteOptions& options) {
        KVStore::write_options_ = options;
    }

    rocksdb::Options KVStore::GetDefaultOptions() {
        // TODO Provide all required options for test. 
        return rocksdb::Options();
    }

    rocksdb::BlockBasedTableOptions KVStore::GetDefaultTableOptions() {
        // TODO Provide all required table options for test. 
        return rocksdb::BlockBasedTableOptions();
    }

    Status KVStore::OpenDB() {
        KVStore::closing_ = false;

        rocksdb::Options options = GetDefaultOptions();
        CreateColumnFamilies(options);

        rocksdb::BlockBasedTableOptions table_options = GetDefaultTableOptions();

        std::vector<rocksdb::ColumnFamilyDescriptor> column_families;
        column_families.push_back(rocksdb::ColumnFamilyDescriptor(dummy_cf, table_options));
        
        std::vector<std::string> previous_column_families;
        rocksdb::Status status = rocksdb::DB::ListColumnFamilies(options, db_dir, &previous_column_families);
        if (!status.ok()) return Status(Status::Error, status.ToString());

        status = rocksdb::DB::Open(options, db_dir, column_families, &cf_handles_, &db_);
        if (!status.ok()) return Status(Status::Error, status.ToString());

        return Status(Status::StatusCode::Success);
    }

    void KVStore::CloseDB() {
        if (KVStore::db_ == nullptr) return;

        KVStore::closing_ = true;
        rocksdb::CancelAllBackgroundWork(db_, true);
        for (auto handle : cf_handles_) db_->DestroyColumnFamilyHandle(handle);
        delete db_;
        db_ = nullptr;
    }

    Status KVStore::CreateColumnFamilies(const rocksdb::Options& options) {
        rocksdb::DB* db;
        rocksdb::ColumnFamilyOptions cf_options(options);
        rocksdb::Status status = rocksdb::DB::Open(options, db_dir, &db);
        if (status.ok()) {
            std::vector<std::string> cf_names = { dummy_cf };
            std::vector<rocksdb::ColumnFamilyHandle*> cf_handles;
            status = db->CreateColumnFamilies(cf_options, cf_names, &cf_handles);
            if (!status.ok()) {
                delete db;
                return Status(Status::DBOpenError, status.ToString());
            }
            for (auto handle : cf_handles) db->DestroyColumnFamilyHandle(handle);
            db->Close();
            delete db;
        }

        if (!status.ok()) {
            return Status(Status::StatusCode::Error, status.ToString());
        }
        return Status(Status::StatusCode::Success);
    }

    rocksdb::Status KVStore::Write(const rocksdb::WriteOptions& options, rocksdb::WriteBatch* batch) {
        return db_->Write(options, batch);
    }

    rocksdb::Status KVStore::Delete(const rocksdb::WriteOptions& options,
        rocksdb::ColumnFamilyHandle* cf_handle,
        const rocksdb::Slice& key) {
        rocksdb::WriteBatch batch;
        batch.Delete(cf_handle, key);
        return Write(options, &batch);
    }

    rocksdb::Status KVStore::Compact(const Slice* begin, const Slice* end) {
        rocksdb::CompactRangeOptions compact_opts;
        compact_opts.change_level = true;
        for (const auto& cf_handle : KVStore::cf_handles_) {
            rocksdb::Status s = db_->CompactRange(compact_opts, cf_handle, begin, end);
            if (!s.ok()) return s;
        }
        return rocksdb::Status::OK();
    }

    rocksdb::DB *KVStore::GetDB() { return db_; }

    rocksdb::ColumnFamilyHandle* KVStore::GetCFHandle() {
        return KVStore::cf_handles_[0];
    }
}