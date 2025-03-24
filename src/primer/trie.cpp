#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {
template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  //throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  if(root_==nullptr) {
    return nullptr;
  }
  auto cur_node=root_;
  for (auto ch:key) {
    auto it=cur_node->children_.find(ch);
    if (it!=cur_node->children_.end()) {
      cur_node=it->second;
    }else {
      return nullptr;
    }
  }
  if (cur_node->is_value_node_) {
    auto cur_node_with_value  =std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(cur_node);
    if (cur_node_with_value!=nullptr) {
      return cur_node_with_value->value_.get();
    }
  }
  return nullptr;

}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  //throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  auto new_root = PutNode(root_, key, std::move(value));
  return Trie(new_root);

}
template<class T>
auto Trie::PutNode(std::shared_ptr<const TrieNode> node, std::string_view key, T value) const
    -> std::shared_ptr<const TrieNode> {

  // Copy-On-Write: 克隆当前节点，或创建新节点
  std::unique_ptr<TrieNode> copy_node = (node == nullptr)
      ? std::make_unique<TrieNode>()
      : node->Clone();

  // Base Case：如果 key 走到头，插入 TrieNodeWithValue
  if (key.empty()) {
    auto value_ptr = std::make_shared<T>(std::move(value));
    return std::make_shared<const TrieNodeWithValue<T>>(copy_node->children_, value_ptr);
  }

  // 递归处理下一个字符
  char cur_ch = key.front();

  // 获取子节点（你的 GetChildNode 应该返回 shared_ptr<const TrieNode>）
  auto child = copy_node->GetChildNode(cur_ch);

  // 递归插入剩余的 key
  auto updated_child = PutNode(child, key.substr(1), std::move(value));

  // 更新当前节点的 children_
  copy_node->children_[cur_ch] = updated_child;

  // 返回 shared_ptr 接管 copy_node
  return std::shared_ptr<const TrieNode>(std::move(copy_node));
}


auto Trie::Remove(std::string_view key) const -> Trie {
  auto new_root = RemoveNode(root_, key, 0);

  if (new_root == nullptr || (new_root->children_.empty() && !new_root->is_value_node_)) {
    return {}; // 返回空 Trie
  }

  return Trie(new_root);
}

auto Trie::RemoveNode(
    const std::shared_ptr<const TrieNode>& node,
    std::string_view key,
    size_t depth) const -> std::shared_ptr<const TrieNode> {

  if (node == nullptr) {
    return nullptr;
  }

  if (depth == key.size()) {
    if (!node->is_value_node_) {
      return node;  // key 不存在，直接返回自己
    }

    // 复制一个当前节点，取消 value 节点标记
    auto new_node = std::make_shared<TrieNode>(node->children_);
    new_node->is_value_node_ = false;

    if (new_node->children_.empty()) {
      return nullptr; // 当前节点没有子节点，也不是 value 节点，可以删除
    }

    return new_node;
  }

  char c = key[depth];

  if (!node->HasChild(c)) {
    return node; // key 不存在，返回自己
  }

  // 递归处理子节点
  auto child_node = RemoveNode(node->GetChildNode(c), key, depth + 1);

  // 克隆当前节点
  auto new_node = node->Clone();

  if (child_node == nullptr) {
    new_node->children_.erase(c);
  } else {
    new_node->children_[c] = child_node;
  }

  if (new_node->children_.empty() && !new_node->is_value_node_) {
    return nullptr; // 当前节点可以删除
  }

  return new_node;
}






// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
