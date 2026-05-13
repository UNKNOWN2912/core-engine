#include "EntityComponentSystem.hpp"

Entity Scene::CreateEntity(std::string_view name) 
{
    mLastId = EntityId(uint32_t(mLastId) + 1);
    Entity entity = mEntities.emplace_back(mLastId, this);

    entity.AddComponent<EntityMetadata>().name = name;

    return entity;
}

Entity Scene::GetEntityById(EntityId id) 
{
    for (int i = 0; i < mEntities.size(); i++)
    {
        if(mEntities[i].mId == (EntityId)id)
            return mEntities[i];
    }
    return Entity();
}

Entity::Entity(EntityId id, Scene* scene)
    : mId(id), mScene(scene)
{
    
}
