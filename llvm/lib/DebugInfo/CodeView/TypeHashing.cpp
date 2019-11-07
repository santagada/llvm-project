//===- TypeHashing.cpp -------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/DebugInfo/CodeView/TypeHashing.h"

#include "llvm/DebugInfo/CodeView/TypeIndexDiscovery.h"

#define XXH_STATIC_LINKING_ONLY
#define XXH_INLINE_ALL
#include "llvm/DebugInfo/CodeView/xxHash.h"
#undef XXH_INLINE_ALL
#undef XXH_STATIC_LINKING_ONLY 

using namespace llvm;
using namespace llvm::codeview;

LocallyHashedType DenseMapInfo<LocallyHashedType>::Empty{0, {}};
LocallyHashedType DenseMapInfo<LocallyHashedType>::Tombstone{hash_code(-1), {}};

static uint64_t EmptyHash = 0;
static uint64_t TombstoneHash = 0xFF00000000000000;

GloballyHashedType DenseMapInfo<GloballyHashedType>::Empty{EmptyHash};
GloballyHashedType DenseMapInfo<GloballyHashedType>::Tombstone{TombstoneHash};

LocallyHashedType LocallyHashedType::hashType(ArrayRef<uint8_t> RecordData) {
  return {llvm::hash_value(RecordData), RecordData};
}

GloballyHashedType
GloballyHashedType::hashType(ArrayRef<uint8_t> RecordData,
                             ArrayRef<GloballyHashedType> PreviousTypes,
                             ArrayRef<GloballyHashedType> PreviousIds) {
  SmallVector<TiReference, 4> Refs;
  discoverTypeIndices(RecordData, Refs);
  XXH64_state_t state;
  state.total_len = 0;
  state.mem64[0] = 0;
  state.mem64[1] = 0;
  state.mem64[2] = 0;
  state.mem64[3] = 0;
  state.memsize = 0;
  state.v1 = PRIME64_1 + PRIME64_2;
  state.v2 = PRIME64_2;
  state.v3 = 0;
  state.v4 = - PRIME64_1;
  uint32_t Off = 0;
  ArrayRef<uint8_t> prefix = RecordData.take_front(sizeof(RecordPrefix));
  XXH64_update(&state, prefix.data(), prefix.size());
  RecordData = RecordData.drop_front(sizeof(RecordPrefix));
  for (const auto &Ref : Refs) {
    // Hash any data that comes before this TiRef.
    uint32_t PreLen = Ref.Offset - Off;
    ArrayRef<uint8_t> PreData = RecordData.slice(Off, PreLen);
    XXH64_update(&state, PreData.data(), PreData.size());
    auto Prev = (Ref.Kind == TiRefKind::IndexRef) ? PreviousIds : PreviousTypes;

    auto RefData = RecordData.slice(Ref.Offset, Ref.Count * sizeof(TypeIndex));
    // For each type index referenced, add in the previously computed hash
    // value of that type.
    ArrayRef<TypeIndex> Indices(
        reinterpret_cast<const TypeIndex *>(RefData.data()), Ref.Count);
    for (TypeIndex TI : Indices) {
      if (TI.isSimple() || TI.isNoneType()) {
        const uint8_t *IndexBytes = reinterpret_cast<const uint8_t *>(&TI);
        ArrayRef<uint8_t> BytesToHash = makeArrayRef(IndexBytes, sizeof(TypeIndex));
        XXH64_update(&state, BytesToHash.data(), BytesToHash.size());
      } else {
        if (TI.toArrayIndex() >= Prev.size() ||
            Prev[TI.toArrayIndex()].empty()) {
          // There are references to yet-unhashed records. Suspend hashing for
          // this record until all the other records are processed.
          return {};
        }
        XXH64_update(&state, &Prev[TI.toArrayIndex()].Hash, 8);
      }
      
    }

    Off = Ref.Offset + Ref.Count * sizeof(TypeIndex);
  }

  // Don't forget to add in any trailing bytes.
  ArrayRef<uint8_t> TrailingBytes = RecordData.drop_front(Off);
  XXH64_update(&state, TrailingBytes.data(), TrailingBytes.size());
  return { XXH64_digest(&state) };
}
