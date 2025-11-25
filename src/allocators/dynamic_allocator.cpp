#include <allocators/dynamic_allocator.hpp>

namespace allocators {

DynamicMemoryResource::DynamicMemoryResource() noexcept {}
DynamicMemoryResource::DynamicMemoryResource(DynamicMemoryResource &&allocator) noexcept
    : occupied_blocks_(std::move(allocator.occupied_blocks_)),
      free_blocks_(std::move(allocator.free_blocks_)),
      all_blocks_(std::move(allocator.all_blocks_)) {}
DynamicMemoryResource::~DynamicMemoryResource() {
  for (auto block : all_blocks_) {
    delete[] block.data;
  }
}
void *DynamicMemoryResource::do_allocate(std::size_t size, std::size_t alignment) {
  for (auto it = free_blocks_.begin(); it != free_blocks_.end(); ++it) {
    if (it->size >= size) {
      BlockData block = *it;
      free_blocks_.erase(it);
      if (block.size != size) {
        free_blocks_.push_back({block.data + size, block.size - size});
      }
      occupied_blocks_.push_back({block.data, size});
      return block.data;
    }
  }
  BlockData new_block{new char[size], size};
  all_blocks_.push_back(new_block);
  occupied_blocks_.push_back(new_block);
  return new_block.data;
}
void DynamicMemoryResource::do_deallocate(void *ptr, std::size_t size,
                                     std::size_t alignment) {
  for (auto it = occupied_blocks_.begin(); it != occupied_blocks_.end(); ++it) {
    if (it->data == ptr) {
      BlockData block = *it;
      occupied_blocks_.erase(it);
      free_blocks_.push_back(block);
      return;
    }
  }
}
bool DynamicMemoryResource::do_is_equal(
    const std::pmr::memory_resource &resource) const noexcept {
  const DynamicMemoryResource *allocator_ptr_ =
      dynamic_cast<const DynamicMemoryResource *>(&resource);
  if (allocator_ptr_ == nullptr) {
    return false;
  }
  return all_blocks_ == allocator_ptr_->all_blocks_;
}
bool operator==(const DynamicMemoryResource::BlockData &a,
                const DynamicMemoryResource::BlockData &b) {
  return a.data == b.data && a.size == b.size;
}
}; // namespace allocators
