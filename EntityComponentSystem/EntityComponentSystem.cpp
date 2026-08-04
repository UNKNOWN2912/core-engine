#include "EntityComponentSystem.hpp"

Entity Scene::CreateEntity(std::string_view name)
{
    return {mRegistry.create(), this};
}

Entity Scene::GetEntityById(EntityID id)
{
    return {id, this};
}

Entity::Entity(EntityID id, Scene *scene)
    : mId(id), mScene(scene)
{
}

Entity Scene::GetEntityByName(std::string_view name)
{
    const auto &view = mRegistry.view<EntityMetadata>();

    Entity result;

    view.each([&](const entt::entity &entity, const EntityMetadata &metadata) {
        if (metadata.name == name)
        {
            result = Entity(entity, this);
        }
    });

    return result;
}
