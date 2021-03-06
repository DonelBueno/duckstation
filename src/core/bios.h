#pragma once
#include "types.h"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace BIOS {
enum : u32
{
  BIOS_BASE = 0x1FC00000,
  BIOS_SIZE = 0x80000
};

using Image = std::vector<u8>;

struct Hash
{
  u8 bytes[16];

  ALWAYS_INLINE bool operator==(const Hash& bh) const { return (std::memcmp(bytes, bh.bytes, sizeof(bytes)) == 0); }
  ALWAYS_INLINE bool operator!=(const Hash& bh) const { return (std::memcmp(bytes, bh.bytes, sizeof(bytes)) != 0); }

  std::string ToString() const;
};

#pragma pack(push, 1)
struct PSEXEHeader
{
  char id[8];            // 0x000-0x007 PS-X EXE
  char pad1[8];          // 0x008-0x00F
  u32 initial_pc;        // 0x010
  u32 initial_gp;        // 0x014
  u32 load_address;      // 0x018
  u32 file_size;         // 0x01C excluding 0x800-byte header
  u32 unk0;              // 0x020
  u32 unk1;              // 0x024
  u32 memfill_start;     // 0x028
  u32 memfill_size;      // 0x02C
  u32 initial_sp_base;   // 0x030
  u32 initial_sp_offset; // 0x034
  u32 reserved[5];       // 0x038-0x04B
  char marker[0x7B4];    // 0x04C-0x7FF
};
static_assert(sizeof(PSEXEHeader) == 0x800);
#pragma pack(pop)

Hash GetHash(const Image& image);
std::optional<Image> LoadImageFromFile(std::string_view filename);
std::optional<Hash> GetHashForFile(std::string_view filename);

bool IsValidHashForRegion(ConsoleRegion region, const Hash& hash);

void PatchBIOS(Image& image, u32 address, u32 value, u32 mask = UINT32_C(0xFFFFFFFF));

bool PatchBIOSEnableTTY(Image& image, const Hash& hash);
bool PatchBIOSFastBoot(Image& image, const Hash& hash);
bool PatchBIOSForEXE(Image& image, u32 r_pc, u32 r_gp, u32 r_sp, u32 r_fp);

bool IsValidPSExeHeader(const PSEXEHeader& header, u32 file_size);
} // namespace BIOS