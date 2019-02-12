//
// Created by Ted Klein Bergman on 11/21/18.
//

#ifndef NEXT_GROUP_LIST_H
#define NEXT_GROUP_LIST_H
#include <iostream>

#include <memory>
#include <string>
#include <vector>



template <typename T, unsigned int groups, unsigned int count_per_group>
class GroupList
{
public:
    // // Defer the add as adding something invalidates all references.
    // // Cannot remove immediately as that wouldn't work when iterating over the list, and iterating over the list is
    // // the only way to get a reference to the elements. Elements should not be held after we apply the changes.
    // template <typename ... Targs>
    // void DefferedAdd(unsigned int group, Targs ... args) {}
    // void DefferedRemove(const T& element) {}
    // void ApplyChanges() {}


    template <typename ... Targs>
    void Add(unsigned int group, Targs ... args)
    {
        if (IsFull())
            throw std::runtime_error("GroupList is full!");

        for (int i = groups-1; i > group; --i)  // Using signed type to protect it from wrapping around when decrementing.
        {
            unsigned int free  = ends[i];
            unsigned int taken = ends[i-1];
            std::swap(content[free], content[taken]);

            ends[i] += 1;
        }

        unsigned int free = ends[group];
        content[free] = T(std::forward<Targs>(args)...);
        ends[group] += 1;
    }

    void Remove(const T * const element)
    {
        // WARNING! The address of the element can be invalid after we add or remove!
        unsigned long index = element - content;  // Note(ted): ONLY WORKS IF GROUP LIST OWNS THE OBJECT!!!!!

        if (index < 0 || index > groups * count_per_group)
            throw std::runtime_error(
                    std::string("This object is invalid! Expected objects at memory address between ")
                    + static_cast<char*>(static_cast<void*>(&content[0])) + " - " + static_cast<char*>(static_cast<void*>(&content[groups * count_per_group])) +
                    ". Got " + static_cast<const char*>(static_cast<const void*>(&element)) + "."
            );

        unsigned long group = 0;
        for (unsigned int i = 0; i < groups; ++i)
            if (index < ends[i])
            { group = i; break; }


        std::swap(content[index], content[ends[group]-1]);  // Swap the object with the last valid element in the group.
        ends[group] -= 1;
    }

    T* const GroupStart(unsigned int group)
    {
        if (group > groups)
            throw std::runtime_error("Index " + std::to_string(group) + " is out of bounds!");

        if (group == 0)
            return &content[0];
        else
            return &content[ends[group-1]];
    }
    T* const GroupStop(unsigned int group)
    {
        if (group > groups)
            throw std::runtime_error("Index " + std::to_string(group) + " is out of bounds!");

        return &content[ends[group]];
    }

    inline bool IsFull() const noexcept { return ends[groups-1] >= groups * count_per_group; }


private:

    T content[groups * count_per_group]{};  // Continuous array of element
    unsigned int ends[groups]{};            // Index to the end of each group. End points after last element.
};

//
// int main()
// {
//     constexpr unsigned int size = 5;
//     GroupList<int, size, 10> group;
//
//
//     group.Add(3, 13);
//     group.Add(3, 14);
//     group.Add(3, 15);
//     group.Add(3, 16);
//
//     group.Add(0, 0);
//     group.Add(0, 1);
//     group.Add(0, 2);
//     group.Add(0, 3);
//     group.Add(0, 4);
//     group.Add(0, 5);
//     group.Add(0, 6);
//
//     group.Add(2, 9);
//     group.Add(2, 10);
//     group.Add(2, 11);
//     group.Add(2, 12);
//
//     group.Add(1, 7);
//     group.Add(1, 8);
//
//
//     for (auto it = group.GroupStart(0); it != group.GroupStop(0); ++it)
//     {
//         if (*it % 2 == 0)
//         {
//             group.Remove(it);
//             --it;
//         }
//     }
//
//     group.Add(0, 0);
//     group.Add(0, 1);
//     group.Add(0, 2);
//     group.Add(0, 3);
//     group.Add(0, 4);
//     group.Add(0, 5);
//     group.Add(0, 6);
//
//     group.Add(2, 9);
//     group.Add(2, 10);
//     group.Add(2, 11);
//     group.Add(2, 12);
//
//     group.Add(1, 7);
//     group.Add(1, 8);
//
//     for (unsigned int i = 0; i < size; ++i)
//     {
//         std::cout << "Group: " << i << std::endl;
//         for (auto* it = group.GroupStart(i) ; it != group.GroupStop(i); ++it)
//             std::cout << *it << ", ";
//         std::cout << std::endl;
//     }
// }

#endif //NEXT_GROUP_LIST_H
