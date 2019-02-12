//
// Created by Ted Klein Bergman on 11/20/18.
//

#ifndef NEXT_SWAP_LIST_H
#define NEXT_SWAP_LIST_H

#include <iostream>

#include <memory>



/*
 * Swaplist
 *
 * A sequential collection with O(1) add, O(1) remove, and all elements are always continuous in memory, allowing for
 * fast iteration.
 *
 * Cons:
 *
 *    1. It doesn't keep elements in order.
 *
 *
 * Things to figure out:
 *
 *    1. How do the user get/remove objects?
 *        a) We can return an index, and have a mapped index list to the object (in case we move the object), but that
 *           requires the user to store these indices whenever they add something. That's a hassle.
 *        b) We can return a reference to the object, and hash it to the mapped index list. This has the problem that
 *           the reference can be invalid if the object is being moved while the user still holds the reference. It
 *           might be possible to solve this with a proxy reference of some sort.
 *        c) We can return a custom element struct, which holds a reference to the object, and it's index and mapped
 *           index. Whenever the user tries to modify the object, the struct check whether it has been moved, and in
 *           that case updates the reference to the correct one.
 *
 *
 *
 *
 */

template <typename T, unsigned int count>
struct SwapList
{
    std::unique_ptr<T[]> content;
    // T content[count];
    unsigned int used = 0;
};

template <typename T, unsigned int count, typename ... Targs>
unsigned Add(SwapList<T, count>& list, Targs ... args)
{
    if (list.used >= count-1)
        throw std::runtime_error("SwapList is full!");

    unsigned int index  = list.used++;
    list.content[index] = std::move(T(std::forward<Targs>(args)...));
    return index;
}

template <typename T, unsigned int count>
void Remove(SwapList<T, count>& list, const T& object)
{
    unsigned long index = &object - list.content;  // Note(ted): ONLY WORKS IF THE LIST OWNS THE OBJECT!!!!!

    if (index < 0 || index >= list.used - 1)
        throw std::runtime_error("Index " + std::to_string(index) + " out of bounds.");

    std::swap(list.content[index], list.content[list.used-1]);
    list.used -= 1;
}

template <typename T, unsigned int count>
T* begin(SwapList<T, count>& list)
{
    return &list.content[0];
}

template <typename T, unsigned int count>
T* end(SwapList<T, count>& list)
{
    return &list.content[list.used];
}

//
// int main()
// {
//
//     SwapList<std::string, 10> list;
//
//     Add(list, "Hello");
//     unsigned index = Add(list, "Ted");
//     Add(list, "where");
//     Add(list, "are");
//     for (std::string& i : list)
//         std::cout << i << " ";
//     std::cout << std::endl;
//
//
//     Remove(list, index);
//     for (std::string& i : list)
//         std::cout << i << " ";
//     std::cout << std::endl;
//
//     //
//     // unsigned value4 = Add(list, "you");
//     // Add(list, "going");
//     // Add(list, "tomorrow?");
//     // for (std::string& i : list)
//     //     std::cout << i << " ";
//     // std::cout << std::endl;
//     //
//     //
//     // Remove(list, value0);
//     // Remove(list, value4);
//     // for (std::string& i : list)
//     //     std::cout << i << " ";
//     // std::cout << std::endl;
//
//
// }

#endif //NEXT_SWAP_LIST_H
