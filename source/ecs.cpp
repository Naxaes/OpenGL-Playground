#include <vector>
//
//
// /*
//  * We have to write different system when the algorithm has to change. For example, we don't have to write 2 different
//  * render systems when the diffuse texture differs between the objects. However, we do need to write 2 different render
//  * systems if one object has 1 texture and the other has 2, as the algorithm has to change. Another example is if we
//  * gave a physics system and an component with the attribute 'bool gravity'. Then we would have two different systems
//  * as the algorithm would differ between them. A solution to the problem could be to have 'int gravity' as a value of
//  * 0 would turn of gravity without changing the algorithm.
//  *
//  *
//  *
//  * When we crete an object we supply it with the components. Based on the components. we can dispatch it to different
//  * managers. Each manager then has to dispatch it further based on the values of certain components (if the values
//  * causes changes to the algorithm). The manager is also responsible to keep track and dispatch the objects when these
//  * attributes changes. This means that we could have some attributes that are implicitly defined depending on in which
//  * system it exists, thus preventing us for having to keep track on it. When we try to change the value of an attribute
//  * of an object, the manager just checks if the object is in some system, and then move it based on the value submitted.
//  *
//  *
//  * So the division will be as followed:
//  *     * MainManager: Creates entities, add/remove/update components, dispatch to submanagers based on components.
//  *         * SystemManager: Handles a component combination, sorts (swaplist) and dispatch to system based on component values.
//  *             * System: Runs an algorithm on a sorted array.
//  *
//  *
//  * A SubManger should own the component array it always iterate fully over. Otherwise, it'll reference the component.
//  *
//  *
//  *
//  * USE STATIC POLYMORPHISM:
//  *     https://en.wikipedia.org/wiki/Template_metaprogramming#Static_polymorphism
//  * */
// struct Entity
// {
//     unsigned long id;
//     unsigned long key;
// };
//
// struct Component {};
// struct Transform    : public Component {};  // Location in world space
// struct MeshRenderer : public Component {};  // How to render a mesh (the model and textures)
// struct RigidBody    : public Component {};  // How physics acts on an object
// struct Collider     : public Component {};  // How to handle collision detection
// struct Light        : public Component {};  // Light properties
// // struct Color        : public Component;
// // struct Texture      : public Component;
// // struct Refraction   : public Component;
// // struct Reflection   : public Component;
// //
// //
// struct System { virtual void Update() = 0; };
// struct PhysicSystem        : public System {};     // Transform, RigidBody
// struct TextureRenderSystem : public System {};     // Transform, Model, Texture
// struct ColorRenderSystem   : public System {};     // Transform, Model, Color
// struct LightRenderSystem   : public System {};     // Transform, Model, Light
// struct RefractiveRenderSystem : public System {};  // Transform, Model, Refraction
// struct ReflectiveRenderSystem : public System {};  // Transform, Model, Reflection
//
//
// unsigned long GenerateKeyID()
// {
//     static unsigned long id = 0;
//     if (id == 0)
//         id += 1;
//     else
//         id <<= 1;
//     return id;
// }
//
// template <typename T>
// unsigned long KeyOf()
// {
//     static unsigned long id = GenerateKeyID();
//     return id;
// }
//
// template <typename T, typename U, typename ... Targs>
// unsigned long KeyOf()
// {
//     unsigned int key = 0;
//     for (unsigned long id : {KeyOf<T>(), KeyOf<U>(), KeyOf<Targs>()...})
//         key += id;
//     return key;
// }
//
// unsigned long GenerateUniqueID()  // Should be changed to something that can reuse ID's!!!
// {
//     static unsigned long id = 0;
//     return ++id;
// }
//
//
// // Classes to allow polymorphism in main manager.
// class Manager
// {
// public:
//     virtual void Remove(Entity entity) = 0;
//     virtual void Update() = 0;
//     virtual unsigned long Key() const noexcept = 0;
// };
//
// template <typename ... Components>
// class SystemManager : public Manager
// {
// public:
//     virtual void Add(Entity entity, Components ... components) = 0;
// };
//
//
//
// class MainManager
// {
// public:
//     using EntityID     = unsigned long;
//     using ComponentID  = unsigned long;
//     using ComponentKey = unsigned long;
//
//     template <typename ... Components>
//     Entity CreateEntity(Components&& ... components)
//     {
//         Entity entity = { GenerateUniqueID(), KeyOf<Components...>() };
//         DispatchComponents(entity, components...);
//         return entity;
//     }
//
//     template <typename ... Components>
//     void RemoveComponents(Entity& entity)
//     {
//         // Look for dependencies and remove them from each submanager.
//         ComponentKey key = KeyOf<Components...>();
//
//         for (Manager* manager : managers)
//             if (manager->Key() & key)
//                 manager->Remove(entity);
//
//         entity.key ^= key;
//     }
//
//     template <typename ... Components>
//     void AddComponents(Entity entity, Components ... components)
//     {
//         DispatchComponents(entity, components...);
//     }
//
//     // Return proxy? Must specify all arguments?
//     template <typename ... Targs>
//     void UpdateComponent(Entity entity, ComponentID component, Targs ... args)
//     {
//
//     }
//
//     template <typename ... Components>
//     bool HasComponents(Entity entity, Components ... components)
//     {
//         ComponentKey key = KeyOf<Components...>(components...);
//         return (entity.key & key) == key;
//     }
//
//     void Update()
//     {
//         for (Manager* manager : managers)
//             manager->Update();
//     }
//
// // private:
//     template <typename ... Components>
//     void DispatchComponents(Entity entity, Components ... components)
//     {
//         for (Manager* manager : managers)
//         {
//             ComponentID key = manager->Key();
//             if ((key & entity.key) == key)
//             {
//                 // This should work unless the Key() and Add() method of the manager is out of sync.
//                 auto* sub_manager = static_cast<SystemManager<Components...>*>(manager);
//                 sub_manager->Add(entity, components...);
//             }
//         }
//     }
//
//     std::vector<Transform> transforms;  // Here because it's important for most managers. Put reference to this in submanagers?
//     std::vector<Manager*>  managers;
// };
//
//
// // Owns MeshRenderer.
// struct RenderManager : public SystemManager<Transform, MeshRenderer>  // Keeps track on Transform, MeshRenderer pair
// {
//     struct Element { unsigned long id; Transform& transform; MeshRenderer mesh; };
//     struct MeshRenderSystem { virtual void Update(std::vector<Element>& elements) = 0; };
//
//     std::vector<Element> elements;
//     std::vector<MeshRenderSystem*>  systems;
//
//     // NOTE(ted): Fix so we can take Transform!
//     void Add(Entity entity, Transform transform, MeshRenderer mesh) override
//     {
//         // Sort based on value so we can dispatch to systems.
//         elements.emplace_back(entity.id, transform, mesh);
//     }
//
//     void Remove(Entity entity) override
//     {
//         for (unsigned int i = 0; i < elements.size(); ++i)
//         {
//             if (elements[i].id == entity.id)
//             {
//                 elements.erase(std::begin(elements) + i);
//             }
//         }
//     }
//
//     void Update() override
//     {
//         for (MeshRenderSystem* system : systems)
//             for (Element& element: elements)
//                 system->Update(elements);
//     }
//
//
//     unsigned long Key() const noexcept override { return KeyOf<Transform, MeshRenderer>(); }
// };
//
//
//
// struct BasicRenderer : public RenderManager::MeshRenderSystem
// {
//     void Update(std::vector<RenderManager::Element>& elements) override
//     {
//
//     }
// };
//
//
// int main()
// {
//     MainManager manager;
//     auto x = new RenderManager();
//     x->systems.push_back(new BasicRenderer);
//     manager.managers.push_back(x);
//
//     Entity entity = manager.CreateEntity(Transform(), MeshRenderer());
//
//     manager.Update();
// }
//
//
//
//
//
//
//
//
//
// //  * So the division will be as followed:
// //  *     * MainManager:
// //              Is the interface between client and internals.
// //              Creates entities, add/remove/update components, dispatch to submanagers based on components.
// //  *     * SystemManager:
// //              Handles a component combination, sorts (swaplist) and dispatch to system based on component values.
// //  *     * System:
// //              Runs an algorithm on a sorted array.
//
//
// struct Entity {};
//
// template <typename Derived>
// struct System {
//     void Init(MainManager& manager) { static_cast<Derived>(this)->Init(manager); }
//     void Update() { static_cast<Derived>(this)->Update(); }
// };
// template <typename Derived>
// struct SystemManager {
//     void Init(MainManager& manager) { static_cast<Derived>(this)->Init(manager); }
//     void Update() { static_cast<Derived>(this)->Update(); }
// };
//
//
// class MainManager
// {
// public:
//     Entity CreateEntity()  {}
//     Entity DestroyEntity() {}
//
//     template <typename Component, Targs ... args>
//     void CreateComponent(Entity entity, Targs args...) {}
//     template <typename Component>
//     void DestroyComponent(Entity entity) {}
//
//     template <typename System>
//     void AddSystem(System system) {}
//     template <typename System>
//     void RemoveComponent(System system) {}
//
//     void Update() {}
// };
//
//
// class SystemManager
// {
// public:
//     template <typename Component, Targs ... args>
//     void CreateComponent(Entity entity, Targs args...) {}
//     template <typename Component>
//     void DestroyComponent(Entity entity) {}
//
//     template <typename System>
//     void AddSystem(System system) {}
//     template <typename System>
//     void RemoveComponent(System system) {}
//
//     void Update() {}
//
//     template <typename Component>
//     Component* GetComponentArray() {}
// };
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// struct Entity
// {
//     unsigned int id;
//     unsigned int mask;
//     Component** components = new Component*[100];  // Array of pointers.
// };
//
// template <typename ... Components>
// class MainManager
// {
//     constexpr unsigned MAX_COUNT = 10000;
//
//     unsigned int count = 0;
//
//     Entity* entities = new Entity[MAX_COUNT];
//     unsigned* id_map = new unsigned[MAX_COUNT];
//
//     std::tuple<Components...> components;
//
// public:
//     const Entity CreateEntity()
//     {
//         entities.emplace_back(count, 0)
//         return *entities.back();
//     }
//     Entity DestroyEntity(const Entity entity) {}
//
//     template <typename Component, Targs ... args>
//     void CreateComponent(Entity entity, Targs args...)
//     {
//         auto component = std::get<Component>();
//     }
//
//     template <typename Component>
//     void DestroyComponent(Entity entity) {}
//
//     template <typename System>
//     void AddSystem(System system) {}
//     template <typename System>
//     void RemoveComponent(System system) {}
//
//     void Update() {}
// };

#include <iostream>
#include <unordered_map>
#include <tuple>
#include <typeinfo>
#include <type_traits>
#include <algorithm>
// TEMPLATE HELPERS
#include <tuple>
#include <stdlib.h>
#include <stdexcept>
#include <utility>

#define NOINLINE      __attribute__((noinline))
#define ALWAYS_INLINE __attribute__((always_inline))

// START - https://gist.github.com/kyle2/3796580
template <class T, class U> struct return_         { static T& perform(U& u) NOINLINE      { throw std::runtime_error("return type mismatch"); } };
template <class T>          struct return_<T, T>   { static T& perform(T& t) ALWAYS_INLINE { return t;                                         } };
template <class Ret, class Tuple, unsigned S> struct get_by_idx_ {
    static Ret& perform(Tuple& t, int idx) ALWAYS_INLINE {
        constexpr unsigned C = S-1;
        if(idx == C) {
            typedef typename std::tuple_element<C,Tuple>::type elem_type;
            return return_<Ret,elem_type>::perform(std::get<C>(t));
        }
        else {
            return get_by_idx_<Ret,Tuple,C>::perform(t, idx);
        }
    }
};
template<class Ret, class Tuple>
struct get_by_idx_<Ret, Tuple, 0> {
    static Ret& perform(Tuple&, int) NOINLINE {
        throw std::runtime_error("bad field index");
    }
};
template<class Ret, class Tuple>
Ret& get(Tuple& t, int idx) {
    constexpr unsigned S = std::tuple_size<Tuple>::value;
    return get_by_idx_<Ret,Tuple,S>::perform(t, idx);
}
// STOP - https://gist.github.com/kyle2/3796580


template <class F, class ... Ts, std::size_t ... Is>
void for_each_in_tuple(const std::tuple<Ts...>& tuple, F func, std::index_sequence<Is...>)
{
    using expander = int[];
    (void)expander { 0, ((void)func(std::get<Is>(tuple)), 0)... };
}
template <class F, class ... Ts>
void for_each_in_tuple(const std::tuple<Ts...>& tuple, F func){
    for_each_in_tuple(tuple, func, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T, std::size_t ... indices>
struct selector
{
    using type = std::tuple<typename std::tuple_element<indices, T>::type...>;
};


template <typename T>
constexpr size_t index_of()
{
    return 1;
}
template <typename T, typename U, typename... Types>
constexpr size_t index_of()
{
    if (std::is_same<T, U>::value)
        return 0;
    else
        return 1 + index_of<T, Types...>();
}

template <typename T>
constexpr size_t count_of()
{
    return 1;
}
template <typename T, typename U, typename... Types>
constexpr size_t count_of()
{
    return 1 + count_of<T, Types...>();
}




// ENTITY COMPONENT SYSTEM
/*
 *
 * ---- WORLD ----
 *
 * User interface for creating and destroying entities, and dynamically adding/removing components to entities.
 *
 * Add entity:
 *     1. Check which types of components we want to create.
 *     2. Check whether we have created that combination before.
 *          2.1 If we have not, create a new array for all entities with that combination.
 *     3. Add all the components to the corresponding array.
 *
 * Remove entity:
 *     1. Check which types of components the entity have.
 *     2. Find the array with that combination.
 *     3. Remove the entity's components from the array.
 *     4. If the array is now empty, remove the array.
 *     5. Invalidate entity.
 *
 * Add components to entity:
 *     1. Check which types of components the entity have.
 *     2. Find the array with that combination.
 *     3. Fetch all combonents.
 *     4. Delete the entity from the array.
 *     5. Add entity with the previous and new components.
 *
 * Remove components from entity:
 *     1. Check which types of components the entity have.
 *     2. Find the array with the entity's original component combination.
 *     3. Fetch all combonents that isn't from the components we want to remove.
 *     4. Delete the entity from the array.
 *     5. Add entity with the fetched components.
 *
 *
 *
 *
 * ---- ComponentSetStorage ----
 *
 * Stores a specific set of components in a database layout. Each component type has it's own array and an index
 * specify one entity.
 *
 * Add entity:
 *     1. Store each component in the corresponding array.
 *     2. Create an handle for the entity.
 *
 * Remove entity:
 *     1. Remove each component in the corresponding array at the index given by the entity.
 *
 * */

//
// template <unsigned size>
// struct Entity
// {
//     unsigned id;
//     std::bitset<size> mask;
// };
//
//
// struct ComponentSetStorageBase
// {
//     virtual void apply() {};   // Invalidates all indices.
//     virtual void remove(unsigned index) {};
//     virtual unsigned count() { return 0; };
// };
//
//
// template <typename ... Components>
// struct ComponentSetStorage : public ComponentSetStorageBase
// {
//     std::tuple<std::vector<Components>...> component_arrays{};
//
//     unsigned entity_count = 0;
//     std::vector<unsigned> dead;
//
//     ComponentSetStorage() {}
//
//     unsigned add(Components ... components_)
//     {
//         std::tuple<Components...> components(components_...);
//         for_each_in_tuple(components, [this](auto& component)
//         {
//             using type = std::remove_const_t<std::remove_reference_t<decltype(component)>>;
//             // constexpr auto index = index_of<type, Components...>();
//             auto array = std::get<std::vector<type>>(component_arrays);
//
//             array.emplace_back(component);
//         });
//
//         auto entity_index = entity_count++;
//         return entity_index;
//     }
//
//     void remove(unsigned index) override
//     {
//         dead.emplace_back(index);
//     }
//
//     unsigned count() override
//     {
//         return entity_count;
//     }
//
//     void apply() override
//     {
//         for (unsigned index : dead)
//         {
//             for_each_in_tuple(component_arrays, [index, this](auto& array)
//             {
//                 // std::iter_swap(array.begin() + index, array.begin() + entity_count-1);
//             });
//             --entity_count;
//         }
//     }
// };
//
//
//
// template <typename ... RegisteredComponents>
// class World
// {
// public:
//     using Mask   = std::bitset<count<RegisteredComponents...>()>;
//     using Entity = Entity<count<RegisteredComponents...>()>;
//
//     std::unordered_map<Mask, ComponentSetStorageBase*> mapping;
//     std::tuple<RegisteredComponents...> x;
//
//
//     template <typename ... Components>
//     Entity create(Components ... component_pack)
//     {
//         // 1. Check which types of components we want to create.
//         std::tuple<Components...> components = std::make_tuple(component_pack...);
//         Mask mask;
//         for_each_in_tuple(components, [&mask](const auto& component)
//             {
//                 using type = std::remove_const_t<std::remove_reference_t<decltype(component)>>;
//                 auto  index = index_of<type, RegisteredComponents...>();
//                 if (index >= count<RegisteredComponents...>())
//                     throw std::runtime_error("Invalid component!!!");
//
//                 mask.set(index);
//             }
//         );
//
//
//         // 2. Check whether we have created that combination before.
//         auto pair = mapping.find(mask);
//         if (pair == mapping.end())
//             // 2.1 If we have not, create a new array for all entities with that combination.
//             mapping[mask] = new ComponentSetStorage<Components...>();
//
//
//         // 3. Add all the components to the corresponding array.
//         auto storage = static_cast<ComponentSetStorage<Components...>*>(mapping[mask]);
//         auto index   = storage->add(component_pack...);
//
//         return {index, mask};
//     }
//
//     void remove(Entity entity)
//     {
//         // 1. Check which types of components the entity have.
//         // 2. Find the array with that combination.
//         auto* storage = mapping[entity.mask];
//
//         // 3. Remove the entity's components from the array.
//         storage->remove(entity.id);
//
//         // 4. If the array is now empty, remove the array.
//         if (storage->count() <= 0)
//             mapping.erase(entity.mask);
//
//         // 5. Invalidate entity.
//         entity.id = 0;
//         entity.mask = 0;
//     }
//
//
//     void apply()
//     {
//         for (auto& pair : mapping)
//         {
//             auto storage = pair.second;
//             storage->apply();
//         }
//     }
//
//
//
//     template <typename ... Components>
//     std::tuple<std::vector<Components>...> arrays()
//     {
//
//         // 1. Create target bitmask.
//         std::tuple<Components...> components = std::make_tuple<Components...>();
//         Mask target;
//         for_each_in_tuple(components, [&target](const auto& component)
//             {
//                 using type = std::remove_const_t<std::remove_reference_t<decltype(component)>>;
//                 auto  index = index_of<type, RegisteredComponents...>();
//                 if (index >= count<RegisteredComponents...>())
//                     throw std::runtime_error("Invalid component!!!");
//
//                 target.set(index);
//             }
//         );
//
//
//         std::tuple<std::vector<Components>...> result;
//         for (auto& pair : mapping)
//         {
//             if ((pair.first & target) == target)
//             {
//                 auto& storage = pair.second;
//                 for_each_in_tuple(components, [&storage, &result](const auto& component)
//                     {
//                         using type = std::remove_const_t<std::remove_reference_t<decltype(component)>>;
//                         auto& x = storage->template array<type>();
//                         using array_t = std::remove_reference_t<decltype(x)>;
//                         std::get<array_t>(result);
//                     }
//                 );
//             }
//         }
//     }
//
//
//
//
//
//
//
//
// };
//
//
// int main()
// {
//     World<int, float, double, unsigned> world;
//
//     auto entity1 = world.create(1, 1.1);
//     auto entity2 = world.create(2.2f, 2);
//     auto entity3 = world.create(3.3, 3.3f);
//     auto entity4 = world.create(4.4, 4.4f);
//
//     world.apply();
//
//     world.remove(entity1);
//     world.remove(entity2);
//
//     world.apply();
//
//     auto entity5 = world.create(5, 5.5f, 5.5);
//
//     world.remove(entity3);
//     world.remove(entity4);
//
//     world.apply();
//
// }
//




class ArrayBase
{
    virtual void add(void* element) {}
};

template <typename T>
class Array : public ArrayBase
{
    void add(T element) override
    {
        auto x = static_cast<T*>(element);
    }
};


class Storage
{
    std::vector<ArrayBase*> arrays;
};



class World
{
public:



















};