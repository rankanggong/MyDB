/*
* Configuration for RocksDB storage and Action Center.
* Currently is unavailable as the whole design is not completed.
* 
* TODO
* Integrate rocksdb, action center and message queue config here.
*/

#pragma once	

#include <rocksdb/options.h>

namespace Storage {
	class KVStore;
}

struct Config {
public:
	struct RocksDB {
		struct WriteOptions {
			bool disableWAL;
		} write_options;
	} RocksDB;
};