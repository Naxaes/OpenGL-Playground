//
// Created by Ted Klein Bergman on 11/19/18.
//

#pragma once

#include <stdexcept>
#include <string>


// Template argument T must be copy assignable or move assignable, as the pool must be able to override existing slots.

template <typename T, unsigned int count>
class ObjectPool
{
public:
    using Index = unsigned long;

    // A valid node contains an object.
    // An invalid node contains a pointer to next invalid node, creating a linked list of freed nodes.
    union Node
    {
        T object;
        Node* next = nullptr;
    };


    class Iterator
    {
    public:
        explicit Iterator(Node* start, Node* stop, Node* free) : current(start), last(stop), free(free) {}

        bool Valid() noexcept
        {
            // Safe to return node if the first free node is located after.
            if (current < free)
                return true;

            while (true)
                if (current == last)
                    return false;
                else if (current == free)
                { current += 1; free = free->next; }
                else if (current > free)  // Shouldn't happen.
                    throw std::runtime_error("Iterator is updating wrong.");
                else
                    return true;
        }

        T& Next()
        {
            // Return current and increment.
            T* result = &current->object;
            current += 1;
            return *result;
        }

    private:
        Node* current;
        Node* free;
        Node* last;
    };


    ObjectPool()
    {
        for (unsigned int i = 0; i < count-1; ++i)
            content[i].next = &content[i+1];
        free = &content[0];
    }

    template <typename ... Targs>
    Index Add(Targs&& ... args)
    {
        // Check that pool is not full.
        if (IsFull()) throw std::runtime_error("Pool is full!");

        // Retrieve the index of where the object is being added to, and update the last index if we're adding it last.
        Index index = IndexOfNode(free);
        if (index == last)
            last = index + 1;  // 'last' always pointing after the last valid element.
        else if (index > last)
            throw std::runtime_error("Member 'last' = " + std::to_string(last) + " has been updated incorrectly. Index = " + std::to_string(index));

        // Create and add object to free available slot.
        // Update free available slot to the next available slot.
        Node* next = free->next;
        free->object = T(std::forward<Targs>(args)...);
        free = next;

        return index;
    }

    void Remove(Index index)
    {
        // Make sure that the linked free list is always ordered from low index to high. This makes more probable
        // we don't have much fragmentation.

        // If the free slot is after the element we're removing.
        if (IndexOfNode(free) > index)
        {
            // Insert this node before free.
            content[index].next = free;
            free = &content[index];
        }
        else
        {
            // Find the free node before this element.
            Node* previous = free;
            while (IndexOfNode(previous->next) < index)
                previous = previous->next;

            // Insert this between node before and node after.
            Node* next = previous->next;
            content[index].next = next;
            previous->next = &content[index];


            // Update last index if the element we're removing is the last.
            // We must find the last valid element. We know the invalid element before this on, so if there is some
            // element between it must be valid. If it isn't, we have to iterate through the linked list again and find
            // the last valid element.
            if (index == last - 1)
            {
                if (index - IndexOfNode(previous) > 1)
                    last = index;  // Last should point to the element after the last valid.
                else
                    throw std::runtime_error("Haven't fixed this yet. Sorry.");
            }
        }
    }

    bool IsFull() const noexcept { return free == nullptr; }
    unsigned long Count() const noexcept { return last; }

    Iterator Range() { return Iterator(Start(), Stop(), free); }

    T& operator[] (unsigned int index) { return content[index].object; }
    T& At (unsigned int index)
    {
        // As we know where the first freed element is, it's safe to return indices before the first free element.
        if (IndexOfNode(free) > index)
            return content[index];

        // Otherwise, go through the linked list until we find a freed element that's not before this index.
        Node* previous = free;
        while (IndexOfNode(previous) < index)
            previous = previous->next;

        // The freed element can now either be the one we're trying to access, or after it.
        if (previous == content[index])
            throw std::runtime_error("This element has been freed!");
        else
            return content[index];
    }

private:

    inline void AddAtFree(const T& object) noexcept
    {
        // Update free available slot to the next available slot.
        Node* next = free->next;
        free->object = object;
        free = next;
    }


    inline Node* Start() const noexcept { return &content[0];    }
    inline Node* Stop()  const noexcept { return &content[last]; }
    inline Index IndexOfNode(Node* node) const noexcept { return node - Start(); }

    Node  content[count];
    Node* free;             // Linked list of freed elements.
    Index last = 0;         // The index of the element just after the last valid.
};












// DYNAMIC VERSION

template <typename T>
class ObjectPoolD
{
public:
    using Index = unsigned long;

    // A valid node contains an object.
    // An invalid node contains a pointer to next invalid node, creating a linked list of freed nodes.
    union Node
    {
        T object;
        Node* next = nullptr;
    };



    explicit ObjectPoolD(Index count) : count(count)
    {
        content = new T[count];
        for (unsigned int i = 0; i < count-1; ++i)
            content[i].next = &content[i+1];
        free = &content[0];
    }


    template <typename ... Targs>
    Index add(Targs&& ... args)
    {
        // Check that pool is not full.
        if (is_full()) throw std::runtime_error("Pool is full!");

        // Retrieve the index of where the object is being added to, and update the last index if we're adding it last.
        Index index = index_of_node(free);
        if (index == last)
            last = index + 1;  // 'last' always pointing after the last valid element.
        else if (index > last)
            throw std::runtime_error("Member 'last' = " + std::to_string(last) + " has been updated incorrectly. Index = " + std::to_string(index));

        // Create and add object to free available slot.
        // Update free available slot to the next available slot.
        Node* next = free->next;
        free->object = T(std::forward<Targs>(args)...);
        free = next;

        return index;
    }

    void remove(Index index)
    {
        // Make sure that the linked free list is always ordered from low index to high. This makes more probable
        // we don't have much fragmentation.

        // If the free slot is after the element we're removing.
        if (index_of_node(free) > index)
        {
            // Insert this node before free.
            content[index].next = free;
            free = &content[index];
        }
        else
        {
            // Find the free node before this element.
            Node* previous = free;
            while (index_of_node(previous->next) < index)
                previous = previous->next;

            // Insert this between node before and node after.
            Node* next = previous->next;
            content[index].next = next;
            previous->next = &content[index];


            // Update last index if the element we're removing is the last.
            // We must find the last valid element. We know the invalid element before this on, so if there is some
            // element between it must be valid. If it isn't, we have to iterate through the linked list again and find
            // the last valid element.
            if (index == last - 1)
            {
                if (index - index_of_node(previous) > 1)
                    last = index;  // Last should point to the element after the last valid.
                else
                    throw std::runtime_error("Haven't fixed this yet. Sorry.");
            }
        }
    }

    bool is_full() const noexcept { return free == nullptr; }
    unsigned long Count() const noexcept { return last; }

    T* begin () { return content;          }
    T* end   () { return content[count-1]; }

    T& operator[] (unsigned int index) { return content[index].object; }
    T& at (unsigned int index)
    {
        // As we know where the first freed element is, it's safe to return indices before the first free element.
        if (index_of_node(free) > index)
            return content[index];

        // Otherwise, go through the linked list until we find a freed element that's not before this index.
        Node* previous = free;
        while (index_of_node(previous) < index)
            previous = previous->next;

        // The freed element can now either be the one we're trying to access, or after it.
        if (previous == content[index])
            throw std::runtime_error("This element has been freed!");
        else
            return content[index];
    }

private:

    inline void add_at_free(const T& object) noexcept
    {
        // Update free available slot to the next available slot.
        Node* next = free->next;
        free->object = object;
        free = next;
    }


    inline Node* Start() const noexcept { return &content[0];    }
    inline Node* Stop()  const noexcept { return &content[last]; }
    inline Index index_of_node(Node* node) const noexcept { return node - Start(); }

    Node* content;
    Node* free;             // Linked list of freed elements. Pointing at the first freed element.
    Index count;
    Index last = 0;         // The index of the element just after the last valid.
};







// int main()
// {
//     ObjectPool<int, 10> pool;
//
//     pool.Add(0);
//     pool.Add(1);
//     pool.Add(2);
//     pool.Add(3);
//     pool.Add(4);
//
//     for (auto it = pool.Range(); it.Valid();)
//         std::cout << it.Next() << ", ";
//     std::cout << std::endl;
//
//     pool.Remove(2);
//     pool.Remove(1);
//     pool.Remove(3);
//
//     for (auto it = pool.Range(); it.Valid();)
//         std::cout << it.Next() << ", ";
//     std::cout << std::endl;
//
//     pool.Add(1);
//     pool.Add(2);
//     pool.Add(3);
//     pool.Add(5);
//     pool.Add(6);
//
//     for (auto it = pool.Range(); it.Valid();)
//         std::cout << it.Next() << ", ";
//     std::cout << std::endl;
//
//     pool.Remove(0);
//     for (auto it = pool.Range(); it.Valid();)
//         std::cout << it.Next() << ", ";
//     std::cout << std::endl;
//
//     pool.Remove(6);
//     for (auto it = pool.Range(); it.Valid();)
//         std::cout << it.Next() << ", ";
//     std::cout << std::endl;
//
//     pool.Remove(3);
//     for (auto it = pool.Range(); it.Valid();)
//         std::cout << it.Next() << ", ";
//     std::cout << std::endl;
//
//     pool.Add(0);
//     for (auto it = pool.Range(); it.Valid();)
//         std::cout << it.Next() << ", ";
//     std::cout << std::endl;
//
// }

