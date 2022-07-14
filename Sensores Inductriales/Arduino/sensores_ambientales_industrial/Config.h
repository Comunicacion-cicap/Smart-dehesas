
// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = { 0x58, 0x95, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { 0x5B, 0x49, 0xC3, 0x61, 0xE8, 0x2D, 0xFA, 0xEB, 0x53, 0xCB, 0xB8, 0xC0, 0xFB, 0xE1, 0x98, 0xA8 };

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60; // 
