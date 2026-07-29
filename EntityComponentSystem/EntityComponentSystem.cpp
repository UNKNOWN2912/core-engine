#include "EntityComponentSystem.hpp"

Entity Scene::CreateEntity(std::string_view name)
{
    mLastId = EntityID(uint32_t(mLastId) + 1);
    Entity entity = mEntities.emplace_back(mLastId, this);

    entity.AddComponent<EntityMetadata>().name = name;

    return entity;
}

Entity Scene::GetEntityById(EntityID id)
{
    for (int i = 0; i < mEntities.size(); i++)
    {
        if (mEntities[i].mId == (EntityID)id)
        {
            return mEntities[i];
        }
    }

    return Entity();
}

Entity::Entity(EntityID id, Scene *scene)
    : mId(id), mScene(scene)
{
}

Entity Scene::GetEntityByName(std::string_view name)
{
    for (int i = 0; i < mEntities.size(); i++)
    {
        if (mEntities[i].GetComponent<EntityMetadata>().name == name)
        {
            return mEntities[i];
        }
    }

    return {};
}
const std::vector<std::string> &Scene::GetModelFileImporter() const
{
    return mModelFileDependency;
}
