#ifndef USING_MATRIX_CLASS_LINKED_LIST_WORKLOAD_HPP
#define USING_MATRIX_CLASS_LINKED_LIST_WORKLOAD_HPP

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <vector>

namespace linked_list_workload {

constexpr std::uint64_t random_seed = 6252026ULL;
constexpr std::size_t operations_per_node = 200;
constexpr std::size_t minimum_array_size = 5;
constexpr std::size_t array_size_variation = 6;

using InitializeMemory = bool (*)(std::size_t) noexcept;
using ShutdownMemory = bool (*)() noexcept;

class Random final {
public:
    explicit Random(std::uint64_t seed) noexcept
        : state_(seed == 0 ? 1 : seed) {
    }

    std::uint64_t next() noexcept {
        state_ ^= state_ >> 12;
        state_ ^= state_ << 25;
        state_ ^= state_ >> 27;
        return state_ * 2685821657736338717ULL;
    }

    std::size_t bounded(std::size_t upper_bound) noexcept {
        return static_cast<std::size_t>(next() % upper_bound);
    }

    int integer() noexcept {
        return static_cast<int>(bounded(200001)) - 100000;
    }

    char character() noexcept {
        return static_cast<char>('A' + bounded(26));
    }

    bool boolean() noexcept {
        return (next() & 1ULL) != 0;
    }

private:
    std::uint64_t state_;
};

struct Node final {
    Node* next{nullptr};
    Node* previous{nullptr};
    std::size_t registry_index{0};
    std::size_t value_count{0};
    int* values{nullptr};
    int first{0};
    int second{0};
    int third{0};
    int fourth{0};
    int fifth{0};
    int sixth{0};
    char category{'A'};
    char marker{'A'};
    char group{'A'};
    bool active{false};
    bool dirty{false};
    bool selected{false};
    bool visible{false};

    ~Node() {
        delete[] values;
    }
};

class LinkedList final {
public:
    explicit LinkedList(std::size_t maximum_size) {
        nodes_.reserve(maximum_size);
    }

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    ~LinkedList() {
        clear();
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return nodes_.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return nodes_.empty();
    }

    [[nodiscard]] Node* randomNode(Random& random) noexcept {
        return nodes_[random.bounded(nodes_.size())];
    }

    void append(Random& random) {
        Node* node = createNode(random);
        node->previous = tail_;
        if (tail_ != nullptr) {
            tail_->next = node;
        } else {
            head_ = node;
        }
        tail_ = node;
        registerNode(node);
    }

    void insertBefore(Node* position, Random& random) {
        Node* node = createNode(random);
        node->next = position;
        node->previous = position->previous;
        if (position->previous != nullptr) {
            position->previous->next = node;
        } else {
            head_ = node;
        }
        position->previous = node;
        registerNode(node);
    }

    void erase(Node* node) noexcept {
        if (node->previous != nullptr) {
            node->previous->next = node->next;
        } else {
            head_ = node->next;
        }
        if (node->next != nullptr) {
            node->next->previous = node->previous;
        } else {
            tail_ = node->previous;
        }

        const std::size_t index = node->registry_index;
        Node* last = nodes_.back();
        nodes_[index] = last;
        last->registry_index = index;
        nodes_.pop_back();
        delete node;
    }

    void update(Node* node, Random& random) noexcept {
        node->first = random.integer();
        node->second += random.integer();
        node->third ^= random.integer();
        node->fourth = random.integer();
        node->fifth -= random.integer();
        node->sixth = random.integer();
        node->category = random.character();
        node->marker = random.character();
        node->group = random.character();
        node->active = random.boolean();
        node->dirty = !node->dirty;
        node->selected = random.boolean();
        node->visible = !node->visible;
        node->values[random.bounded(node->value_count)] = random.integer();
    }

    void replaceValues(Node* node, Random& random) {
        const std::size_t count =
            minimum_array_size + random.bounded(array_size_variation);
        std::unique_ptr<int[]> replacement(new int[count]);
        for (std::size_t index = 0; index < count; ++index) {
            replacement[index] = random.integer();
        }
        delete[] node->values;
        node->values = replacement.release();
        node->value_count = count;
    }

    [[nodiscard]] bool validateAndChecksum(
        std::uint64_t& checksum
    ) const noexcept {
        checksum = 1469598103934665603ULL;
        const Node* previous = nullptr;
        const Node* current = head_;
        std::size_t count = 0;
        while (current != nullptr) {
            if (current->previous != previous ||
                current->registry_index >= nodes_.size() ||
                nodes_[current->registry_index] != current ||
                count >= nodes_.size()) {
                return false;
            }

            mix(checksum, static_cast<std::uint32_t>(current->first));
            mix(checksum, static_cast<std::uint32_t>(current->second));
            mix(checksum, static_cast<std::uint32_t>(current->third));
            mix(checksum, static_cast<std::uint32_t>(current->fourth));
            mix(checksum, static_cast<std::uint32_t>(current->fifth));
            mix(checksum, static_cast<std::uint32_t>(current->sixth));
            mix(checksum, static_cast<unsigned char>(current->category));
            mix(checksum, static_cast<unsigned char>(current->marker));
            mix(checksum, static_cast<unsigned char>(current->group));
            mix(checksum, current->active ? 1U : 0U);
            mix(checksum, current->dirty ? 1U : 0U);
            mix(checksum, current->selected ? 1U : 0U);
            mix(checksum, current->visible ? 1U : 0U);
            mix(checksum, current->value_count);
            for (std::size_t index = 0; index < current->value_count; ++index) {
                mix(
                    checksum,
                    static_cast<std::uint32_t>(current->values[index])
                );
            }

            previous = current;
            current = current->next;
            ++count;
        }
        return count == nodes_.size() && previous == tail_;
    }

private:
    static Node* createNode(Random& random) {
        std::unique_ptr<Node> node(new Node{});
        node->first = random.integer();
        node->second = random.integer();
        node->third = random.integer();
        node->fourth = random.integer();
        node->fifth = random.integer();
        node->sixth = random.integer();
        node->category = random.character();
        node->marker = random.character();
        node->group = random.character();
        node->active = random.boolean();
        node->dirty = random.boolean();
        node->selected = random.boolean();
        node->visible = random.boolean();
        node->value_count =
            minimum_array_size + random.bounded(array_size_variation);

        std::unique_ptr<int[]> values(new int[node->value_count]);
        for (std::size_t index = 0; index < node->value_count; ++index) {
            values[index] = random.integer();
        }
        node->values = values.release();
        return node.release();
    }

    void registerNode(Node* node) {
        node->registry_index = nodes_.size();
        nodes_.push_back(node);
    }

    void clear() noexcept {
        Node* node = head_;
        while (node != nullptr) {
            Node* next = node->next;
            delete node;
            node = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        nodes_.clear();
    }

    static void mix(std::uint64_t& checksum, std::uint64_t value) noexcept {
        checksum ^= value + 0x9E3779B97F4A7C15ULL +
            (checksum << 6) + (checksum >> 2);
    }

    Node* head_{nullptr};
    Node* tail_{nullptr};
    std::vector<Node*> nodes_;
};

struct OperationCounts {
    std::size_t appended{0};
    std::size_t inserted{0};
    std::size_t erased{0};
    std::size_t updated{0};
    std::size_t arrays_replaced{0};
    std::size_t peak_size{0};
};

inline bool parseMaximumSize(
    int argument_count,
    char** arguments,
    std::size_t& maximum_size
) noexcept {
    if (argument_count != 2) {
        return false;
    }

    errno = 0;
    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(arguments[1], &end, 10);
    if (errno != 0 || end == arguments[1] || *end != '\0' || parsed == 0 ||
        parsed > std::numeric_limits<std::size_t>::max()) {
        return false;
    }
    maximum_size = static_cast<std::size_t>(parsed);
    return true;
}

inline bool calculateSizes(
    std::size_t maximum_size,
    std::size_t& operation_count,
    std::size_t& pool_size
) noexcept {
    if (maximum_size >
        std::numeric_limits<std::size_t>::max() / operations_per_node) {
        return false;
    }
    operation_count = maximum_size * operations_per_node;

    constexpr std::size_t bytes_per_node = 2048;
    if (maximum_size >
        (std::numeric_limits<std::size_t>::max() - 64 * 1024 * 1024) /
            bytes_per_node) {
        return false;
    }
    pool_size = maximum_size * bytes_per_node + 64 * 1024 * 1024;
    return true;
}

inline void performOperation(
    LinkedList& list,
    std::size_t maximum_size,
    Random& random,
    OperationCounts& counts
) {
    if (list.empty()) {
        list.append(random);
        ++counts.appended;
        counts.peak_size = std::max(counts.peak_size, list.size());
        return;
    }

    const std::size_t operation = random.bounded(100);
    if (operation < 20) {
        if (list.size() < maximum_size) {
            list.append(random);
            ++counts.appended;
        } else {
            list.erase(list.randomNode(random));
            ++counts.erased;
        }
    } else if (operation < 40) {
        if (list.size() < maximum_size) {
            list.insertBefore(list.randomNode(random), random);
            ++counts.inserted;
        } else {
            list.erase(list.randomNode(random));
            ++counts.erased;
        }
    } else if (operation < 60) {
        list.erase(list.randomNode(random));
        ++counts.erased;
    } else if (operation < 85) {
        list.update(list.randomNode(random), random);
        ++counts.updated;
    } else {
        list.replaceValues(list.randomNode(random), random);
        ++counts.arrays_replaced;
    }
    counts.peak_size = std::max(counts.peak_size, list.size());
}

inline int run(
    int argument_count,
    char** arguments,
    const char* memory_name,
    InitializeMemory initialize_memory,
    ShutdownMemory shutdown_memory
) {
    std::size_t maximum_size = 0;
    if (!parseMaximumSize(argument_count, arguments, maximum_size)) {
        std::fprintf(stderr, "usage: %s MAXIMUM_LIST_SIZE\n", arguments[0]);
        return 1;
    }

    std::size_t operation_count = 0;
    std::size_t pool_size = 0;
    if (!calculateSizes(maximum_size, operation_count, pool_size) ||
        !initialize_memory(pool_size)) {
        std::fprintf(stderr, "unable to initialize linked list memory\n");
        return 1;
    }

    OperationCounts counts{};
    std::uint64_t checksum = 0;
    std::size_t final_size = 0;
    bool valid = false;
    {
        Random random(random_seed);
        LinkedList list(maximum_size);
        const std::size_t initial_size = std::max(
            std::size_t{1},
            maximum_size / 2
        );
        for (std::size_t index = 0; index < initial_size; ++index) {
            list.append(random);
        }
        counts.peak_size = list.size();

        for (std::size_t operation = 0;
             operation < operation_count;
             ++operation) {
            performOperation(list, maximum_size, random, counts);
        }
        final_size = list.size();
        valid = list.validateAndChecksum(checksum);
    }

    if (!shutdown_memory()) {
        std::fprintf(stderr, "unable to release linked list memory\n");
        return 1;
    }
    if (!valid) {
        std::fprintf(stderr, "linked list validation failed\n");
        return 1;
    }

    std::printf(
        "%s linked list: maximum=%zu operations=%zu final=%zu peak=%zu "
        "appended=%zu inserted=%zu erased=%zu updated=%zu replaced=%zu "
        "seed=%llu checksum=%llu\n",
        memory_name,
        maximum_size,
        operation_count,
        final_size,
        counts.peak_size,
        counts.appended,
        counts.inserted,
        counts.erased,
        counts.updated,
        counts.arrays_replaced,
        static_cast<unsigned long long>(random_seed),
        static_cast<unsigned long long>(checksum)
    );
    return 0;
}

}

#endif
