/**
 * @file fov.cpp
 * @brief Patches the FOV range from [30°, 110°] to [12°, 200°].
 *
 * Patch sites (from zaphkiel disassembly, resolved via IDA signatures):
 *
 *   0xa7af150  mov  w9, #0x42dc0000   ; f32~110.0  (max FOV)
 *   0xa7af164  fmov s1, #30           ; f32~30.0   (min FOV)
 */

#include "pl/memory/Patch.hpp"
#include "pl/memory/Signature.hpp"

#include <cstdint>

// ---------------------------------------------------------------------------
// IDA signatures (from signatures.json)
// Each pattern resolves to the address of its first byte, which is also the
// exact instruction to patch (offset 0 into the signature).
// ---------------------------------------------------------------------------

// 0xa7af150  mov w9, #0x42dc0000   ; max FOV 110.0
static constexpr std::string_view kSigMaxFov =
    "89 ?? A8 52 6A ?? A7 72 00 01 27 1E 22 01 27 1E "
    "43 01 27 1E 01 D0 27 1E A4 83 01 D1 A5 43 02 D1 "
    "E0 03 16 AA 41 ?? 80 52 E2 03 1F 2A 03 ?? 80 52 "
    "?? ?? ?? 94 A8 03 57 38";

// 0xa7af164  fmov s1, #30           ; min FOV 30.0
static constexpr std::string_view kSigMinFov =
    "01 D0 27 1E A4 83 01 D1 A5 43 02 D1 E0 03 16 AA "
    "41 ?? 80 52 E2 03 1F 2A 03 ?? 80 52 ?? ?? ?? 94 "
    "A8 03 57 38 ?? ?? ?? ?? A0 03 58 F8 ?? ?? ?? 95 "
    "A8 03 5A 38 ?? ?? ?? ??";

// ---------------------------------------------------------------------------
// Patch descriptors
// ---------------------------------------------------------------------------

// ARM64 little-endian instruction bytes.
//
// MAX FOV  110.0 → 200.0
//   old: MOVZ w9, #0x42DC, lsl#16  ->  89 5B A8 52
//   new: MOVZ w9, #0x4348, lsl#16  ->  09 69 A8 52
//
// MIN FOV  30.0 → 12.0
//   old: FMOV s1, #30.0  (imm8=0x3E)  ->  01 D0 27 1E   (confirmed by IDA)
//   new: FMOV s1, #12.0  (imm8=0x28)  ->  01 10 25 1E
static constexpr uint8_t kMaxFovNew[] = {0x09, 0x69, 0xA8, 0x52};
static constexpr uint8_t kMinFovNew[] = {0x01, 0x10, 0x25, 0x1E};

// ---------------------------------------------------------------------------
// Patch installation
// ---------------------------------------------------------------------------

__attribute__((constructor)) static void applyFovPatches() {
    const uintptr_t addrMaxFov =
        pl::memory::resolveSignature(kSigMaxFov, "libminecraftpe.so");
    if (addrMaxFov != 0) {
        pl::memory::writeBytes(
            addrMaxFov,
            std::span<const uint8_t>(kMaxFovNew, sizeof(kMaxFovNew)),
            "fov::max_fov_110_to_200"
        );
    }

    const uintptr_t addrMinFov =
        pl::memory::resolveSignature(kSigMinFov, "libminecraftpe.so");
    if (addrMinFov != 0) {
        pl::memory::writeBytes(
            addrMinFov,
            std::span<const uint8_t>(kMinFovNew, sizeof(kMinFovNew)),
            "fov::min_fov_30_to_12"
        );
    }
}
