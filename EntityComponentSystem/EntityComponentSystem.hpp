#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

enum class EntityId : uint64_t;
enum class ComponentId : uint64_t;

struct EntityMetadata
{
    std::string name;
    bool createdFromModel = false;
};

class Scene;

class Entity
{
public:
    Entity() {};
    Entity(EntityId id, Scene *scene);
    EntityId GetId() const
    {
        return mId;
    }

    template <typename ComponentType>
    ComponentType &GetComponent();

    template <typename ComponentType>
    const ComponentType &GetComponent() const;

    template <typename ComponentType, typename... Args>
    ComponentType &AddComponent(Args... args);

    template <typename ComponentType>
    bool HasComponent() const;

    bool operator==(const Entity &entity) const
    {
        return mId == entity.mId;
    }

    bool IsValid() const
    {
        return mScene != nullptr;
    }

private:
    friend class Scene;

    EntityId mId = (EntityId)UINT64_MAX;

    Scene *mScene = nullptr;
};

class BaseStorage
{
public:
};

template <typename ComponentType>
class ComponentStorage : public BaseStorage
{
public:
    using EntityComponentPair = std::pair<Entity, ComponentType>;

    template <typename... Args>
    void PushComponent(Entity entity, Args... args)
    {
        mComponents.push_back({entity, ComponentType(args...)});
    }

    template <typename... Args>
    ComponentType &EmplaceComponent(Entity entity, Args... args)
    {
        return mComponents.emplace_back(entity, ComponentType(args...)).second;
    }

    ComponentType &GetComponent(Entity entity)
    {
        for (EntityComponentPair &pair : mComponents)
        {
            if (pair.first == entity)
            {
                return pair.second;
            }
        }
    }
    const ComponentType &GetComponent(Entity entity) const
    {
        for (EntityComponentPair &pair : mComponents)
        {
            if (pair.first == entity)
            {
                return pair.second;
            }
        }
    }

    ComponentId GetTypeId()
    {
        return (ComponentId) typeid(ComponentType).hash_code();
    }

    const std::vector<EntityComponentPair> &GetEntities() const
    {
        return mComponents;
    }

    std::vector<EntityComponentPair> &GetEntities()
    {
        return mComponents;
    }

private:
    std::vector<EntityComponentPair> mComponents;
};

class Scene
{
public:
    using ComponentStoragePair = std::pair<ComponentId, std::shared_ptr<BaseStorage>>;

    Entity CreateEntity(std::string_view name);
    Entity GetEntityById(EntityId id);
    Entity GetEntityByName(std::string_view name);

    template <typename ComponentType, typename... Args>
    ComponentType &AddComponent(const Entity &entity, Args... args)
    {
        ComponentId componentId = (ComponentId) typeid(ComponentType).hash_code();

        for (int i = 0; i < mStorage.size(); i++)
        {
            const ComponentStoragePair &pair = mStorage[i];

            if (pair.first == componentId)
            {
                return ((ComponentStorage<ComponentType> *)mStorage[i].second.get())->EmplaceComponent(entity, args...);
            }
        }

        ComponentStorage<ComponentType> *storage = (ComponentStorage<ComponentType> *)mStorage.emplace_back(ComponentStoragePair(componentId, new ComponentStorage<ComponentType>))
                                                       .second.get();

        return storage->EmplaceComponent(entity, args...);
    }

    template <typename ComponentType>
    ComponentStorage<ComponentType> &GetStorage()
    {
        ComponentId componentId = (ComponentId) typeid(ComponentType).hash_code();

        for (int i = 0; i < mStorage.size(); i++)
        {
            const ComponentStoragePair &pair = mStorage[i];
            if (pair.first == componentId)
            {
                return *(ComponentStorage<ComponentType> *)pair.second.get();
            }
        }

        static ComponentStorage<ComponentType> storage;
        return storage;
    }

    template <typename ComponentType>
    const ComponentStorage<ComponentType> &GetStorage() const
    {
        ComponentId componentId = (ComponentId) typeid(ComponentType).hash_code();

        for (int i = 0; i < mStorage.size(); i++)
        {
            const ComponentStoragePair &pair = mStorage[i];

            if (pair.first == componentId)
            {
                return *(ComponentStorage<ComponentType> *)pair.second.get();
            }
        }

        static ComponentStorage<ComponentType> storage;
        return storage;
    }

    template <typename ComponentType>
    ComponentType &GetComponent(const Entity &entity)
    {
        return GetStorage<ComponentType>().GetComponent(entity);
    }

    template <typename ComponentType>
    const ComponentType &GetComponent(const Entity &entity) const
    {
        return GetStorage<ComponentType>().GetComponent(entity);
    }

    template <typename ComponentType>
    const std::vector<std::pair<Entity, ComponentType>> &GetEntities() const
    {
        return GetStorage<ComponentType>().GetEntities();
    }

    template <typename ComponentType>
    std::vector<std::pair<Entity, ComponentType>> &GetEntities()
    {
        return GetStorage<ComponentType>().GetEntities();
    }

    template <typename ComponentType>
    bool HasComponent(Entity entity)
    {
        std::vector<std::pair<Entity, ComponentType>> v = GetStorage<ComponentType>().GetEntities();
        for (const std::pair<Entity, ComponentType> &pair : v)
        {
            if (pair.first == entity)
            {
                return true;
            }
        }

        return false;
    }

    template <typename ComponentType>
    bool ContainComponent()
    {
        ComponentId componentId = (ComponentId) typeid(ComponentType).hash_code();

        for (int i = 0; i < mStorage.size(); i++)
        {
            const ComponentStoragePair &pair = mStorage[i];

            if (pair.first == componentId)
            {
                return true;
            }
        }

        return false;
    }

    void AddModelFileImporter(const std::string &filename)
    {
        mModelFileDependency.push_back(filename);
    }

    const std::vector<std::string> &GetModelFileImporter() const
    {
        return mModelFileDependency;
    }

private:
    std::vector<Entity> mEntities;
    std::vector<ComponentStoragePair> mStorage;

    std::vector<std::string> mModelFileDependency;
    EntityId mLastId = (EntityId)UINT64_MAX;
};

template <typename ComponentType>
ComponentType &Entity::GetComponent()
{
    return mScene->GetComponent<ComponentType>(*this);
}

template <typename ComponentType>
const ComponentType &Entity::GetComponent() const
{
    return mScene->GetComponent<ComponentType>(*this);
}

template <typename ComponentType, typename... Args>
ComponentType &Entity::AddComponent(Args... args)
{
    return mScene->AddComponent<ComponentType>(*this, args...);
}

template <typename ComponentType>
inline bool Entity::HasComponent() const
{
    return mScene->HasComponent<ComponentType>(*this);
}
