#pragma once

#include <list>
#include <memory>

namespace allocators {

class DynamicMemoryResource : public std::pmr::memory_resource {
public:
  DynamicMemoryResource() noexcept;
  DynamicMemoryResource(const DynamicMemoryResource &allocator) = delete;
  DynamicMemoryResource(DynamicMemoryResource &&allocator) noexcept;
  ~DynamicMemoryResource() override;

private:
  void *do_allocate(std::size_t size, std::size_t alignment) override final;
  void do_deallocate(void *ptr, std::size_t size,
                     std::size_t alignment) override final;
  bool do_is_equal(
      const std::pmr::memory_resource &resource) const noexcept override final;

private:
  struct BlockData {
    char *data;
    std::size_t size;
  };
  friend bool operator==(const BlockData &a, const BlockData &b);
  std::list<BlockData> occupied_blocks_;
  std::list<BlockData> free_blocks_;
  std::list<BlockData> all_blocks_;
};
} // namespace allocators
