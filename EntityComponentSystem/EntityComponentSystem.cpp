#include "EntityComponentSystem.hpp"
#include "Core/Macro.hpp"

Entity Scene::CreateEntity(std::string_view name)
{
    Entity entity = {mRegistry.create(), this};
    entity.AddComponent<EntityMetadata>().name = name;
    return entity;
}

void Scene::DestroyEntity(Entity &entity)
{
    if (&entity.GetParentScene() != this)
    {
        ERROR("Cannot delete entity. Entity not part of this scene");
        return;
    }

    mRegistry.destroy(entity.mId);

    entity = Entity();
}

Entity Scene::GetEntityById(EntityID id)
{
    return {id, this};
}

Entity::Entity(EntityID id, Scene *scene)
    : mId(id), mScene(scene), mConstScene(scene)
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
