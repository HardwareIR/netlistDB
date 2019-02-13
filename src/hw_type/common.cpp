#include "common.h"


namespace netlistDB {
namespace hw_type {

HwInt hw_uint64(64, false);
HwInt hw_uint32(32, false);
HwInt hw_uint16(16, false);
HwInt hw_uint8(8, false);
HwInt hw_char(8, true);

HwInt hw_int64(64, true);
HwInt hw_int32(32, true);
HwInt hw_int16(16, true);
HwInt hw_int8(8, true);

HwInt hw_bit(1, false);
HwInt hw_bool(1, false);


}
}
