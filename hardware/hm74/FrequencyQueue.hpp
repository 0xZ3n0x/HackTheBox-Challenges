#ifndef FREQUENCY_QUEUE_HPP
#define FREQUENCY_QUEUE_HPP

#include <unordered_map>
#include <queue>
#include <vector>

/**
 * @class FrequencyQueue
 * @brief A structure that tracks the frequency of bytes and retrieves
 * the most frequent ones first using a Max-Heap strategy.
 * * It uses "Lazy Deletion": when a frequency changes, the old (stale)
 * entries remain in the priority queue. They are simply ignored during pop().
 */
class FrequencyQueue
{
private:
    // Tracks current frequency of each byte: {byte_value -> count}
    std::unordered_map<unsigned char, int> counts;

    // Max-Heap to store pairs of {frequency, byte_value}
    // The highest frequency stays at the top.
    std::priority_queue<std::pair<int, unsigned char>> maxHeap;

    // Total number of valid elements currently tracked
    int totalElements = 0;

public:
    /**
     * @brief Adds a value to the queue and updates its frequency.
     * @param value The byte to track.
     */
    void push(unsigned char value)
    {
        counts[value]++;
        // Push the new frequency state onto the heap
        maxHeap.push({counts[value], value});
        totalElements++;
    }

    /**
     * @brief Retrieves and removes the most frequent byte.
     * @return The byte with the highest frequency, or 0 if empty.
     */
    unsigned char pop()
    {
        while (!maxHeap.empty())
        {
            // Get the current candidate for "most frequent"
            auto [heapFreq, value] = maxHeap.top();
            maxHeap.pop();

            // Check if this heap entry is "stale"
            // If the frequency in the map doesn't match the one from the heap,
            // it means this value was pushed again with a higher frequency later.
            if (counts[value] == heapFreq)
            {
                counts[value]--;
                totalElements--;
                return value;
            }
            // Skip stale entries and continue searching
        }
        return 0;
    }

    /**
     * @brief Checks if there are any valid elements left in the queue.
     */
    bool empty() const
    {
        return totalElements == 0;
    }
};

#endif