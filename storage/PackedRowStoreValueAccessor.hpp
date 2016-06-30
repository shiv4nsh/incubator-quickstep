/**
 *   Copyright 2011-2015 Quickstep Technologies LLC.
 *   Copyright 2015 Pivotal Software, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 **/

#ifndef QUICKSTEP_STORAGE_PACKED_ROW_STORE_VALUE_ACCESSOR_HPP_
#define QUICKSTEP_STORAGE_PACKED_ROW_STORE_VALUE_ACCESSOR_HPP_

#include <utility>

#include "catalog/CatalogRelationSchema.hpp"
#include "catalog/CatalogTypedefs.hpp"
#include "storage/StorageBlockInfo.hpp"
#include "storage/ValueAccessor.hpp"
#include "types/Type.hpp"
#include "types/TypedValue.hpp"
#include "utility/BitVector.hpp"
#include "utility/Macros.hpp"

namespace quickstep {

class PackedRowStoreTupleStorageSubBlock;

class PackedRowStoreValueAccessorHelper {
 public:
  PackedRowStoreValueAccessorHelper(const CatalogRelationSchema &relation,
                                    const tuple_id num_tuples,
                                    const void *tuple_storage,
                                    const BitVector<false> *null_bitmap)
      : relation_(relation),
        num_tuples_(num_tuples),
        tuple_storage_(tuple_storage),
        null_bitmap_(null_bitmap),
        attr_max_lengths_(relation.getMaximumAttributeByteLengths()) {
  }

  inline tuple_id numPackedTuples() const {
    return num_tuples_;
  }

  template <bool check_null>
  inline const void* getAttributeValue(const tuple_id tuple,
                                       const attribute_id attr) const {
    DEBUG_ASSERT(tuple < num_tuples_);
    DEBUG_ASSERT(relation_.hasAttributeWithId(attr));
    if (check_null) {
      const int nullable_idx = relation_.getNullableAttributeIndex(attr);
      if ((nullable_idx != -1)
          && null_bitmap_->getBit(tuple * relation_.numNullableAttributes() + nullable_idx)) {
        return nullptr;
      }
    }

    return static_cast<const char*>(tuple_storage_)          // Start of actual tuple storage.
           + (tuple * relation_.getFixedByteLength())        // Tuples prior to 'tuple'.
           + relation_.getFixedLengthAttributeOffset(attr);  // Attribute offset within tuple.
  }

  template <bool check_null>
  inline std::pair<const void*, std::size_t> getAttributeValueAndByteLength(const tuple_id tuple,
                                                                        const attribute_id attr) const {
    DEBUG_ASSERT(tuple < num_tuples_);
    DEBUG_ASSERT(relation_.hasAttributeWithId(attr));
    if (check_null) {
      const int nullable_idx = relation_.getNullableAttributeIndex(attr);
      if ((nullable_idx != -1)
          && null_bitmap_->getBit(tuple * relation_.numNullableAttributes() + nullable_idx)) {
        return std::make_pair(nullptr, 0);
      }
    }

    return std::make_pair(static_cast<const char*>(tuple_storage_)
                              + (tuple * relation_.getFixedByteLength())
                              + relation_.getFixedLengthAttributeOffset(attr),
                          attr_max_lengths_[attr]);
  }

  inline TypedValue getAttributeValueTyped(const tuple_id tuple,
                                           const attribute_id attr) const {
    const Type &attr_type = relation_.getAttributeById(attr)->getType();
    const void *untyped_value = getAttributeValue<true>(tuple, attr);
    return (untyped_value == nullptr)
        ? attr_type.makeNullValue()
        : attr_type.makeValue(untyped_value, attr_type.maximumByteLength());
  }

 private:
  const CatalogRelationSchema &relation_;
  const tuple_id num_tuples_;
  const void *tuple_storage_;
  const BitVector<false> *null_bitmap_;
  const std::vector<std::size_t> &attr_max_lengths_;

  DISALLOW_COPY_AND_ASSIGN(PackedRowStoreValueAccessorHelper);
};

typedef PackedTupleStorageSubBlockValueAccessor<
    PackedRowStoreTupleStorageSubBlock,
    PackedRowStoreValueAccessorHelper,
    ValueAccessor::Implementation::kPackedRowStore>
        PackedRowStoreValueAccessor;

}  // namespace quickstep

#endif  // QUICKSTEP_STORAGE_PACKED_ROW_STORE_VALUE_ACCESSOR_HPP_
