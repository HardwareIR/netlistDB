#include <netlistDB/hw_type/common.h>

namespace netlistDB {
namespace hw_type {

NETLISTDB_PUBLIC HwInt hw_uint64(64, false);
NETLISTDB_PUBLIC HwInt hw_uint32(32, false);
NETLISTDB_PUBLIC HwInt hw_uint16(16, false);
NETLISTDB_PUBLIC HwInt hw_uint8(8, false);
NETLISTDB_PUBLIC HwInt hw_char(8, true);

NETLISTDB_PUBLIC HwInt hw_int64(64, true);
NETLISTDB_PUBLIC HwInt hw_int32(32, true);
NETLISTDB_PUBLIC HwInt hw_int16(16, true);
NETLISTDB_PUBLIC HwInt hw_int8(8, true);

NETLISTDB_PUBLIC HwInt hw_bit(1, false);
NETLISTDB_PUBLIC HwInt hw_bool(1, false);


}
}
