#pragma once
#include "Renderer/Camera.hpp"
#include <Vendor/entt/single_include/entt/entt.hpp>
#include <functional>

using EntityID = entt::entity;
enum class ComponentId : uint64_t;

struct EntityMetadata
{
    std::string name;
    bool createdFromModel = false;
    bool enableSerializing = false;
};

class Scene;

class Entity
{
public:
    Entity() {};
    Entity(EntityID id, Scene *scene);
    EntityID GetId() const
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

    Scene &GetParentScene()
    {
        return *mScene;
    }

private:
    friend class Scene;
    EntityID mId = (EntityID)UINT64_MAX;
    Scene *mScene = nullptr;
};

class Scene
{
public:
    Entity CreateEntity(std::string_view name);
    Entity GetEntityById(EntityID id);
    Entity GetEntityByName(std::string_view name);

    template <typename ComponentType, typename... Args>
    ComponentType &AddComponent(const Entity &entity, Args... args)
    {
        return mRegistry.emplace<ComponentType>(entity.mId, args...);
    }

    template <typename ComponentType>
    ComponentType &GetComponent(const Entity &entity)
    {
        return mRegistry.get<ComponentType>(entity.mId);
    }

    template <typename ComponentType>
    const ComponentType &GetComponent(const Entity &entity) const
    {
        return mRegistry.get<ComponentType>(entity.mId);
    }

    template <typename ComponentType>
    void Each(const std::function<void(Entity, ComponentType &component)> &callback)
    {
        auto view = mRegistry.view<ComponentType>();
        view.each([&](const entt::entity &id, ComponentType &component) {
            Entity entity(id, this);
            callback(entity, component);
        });
    }

    template <typename ComponentType>
    void Each(const std::function<void(Entity, const ComponentType &component)> &callback) const
    {
        const auto view = mRegistry.view<ComponentType>();
        view.each([&](const entt::entity &id, const ComponentType &component) {
            callback({id, this}, component);
        });
    }

    template <typename ComponentType>
    bool HasComponent(Entity entity)
    {
    }

    void SetCamera(const Camera &camera)
    {
        mCamera = camera;
    }

    const Camera &GetCamera() const
    {
        return mCamera;
    }

private:
    entt::registry mRegistry;
    Camera mCamera;
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
