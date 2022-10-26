#include "World.h"

World::World()
{
    m_entity_mngr = std::make_unique<EntityMngr>();
    m_system_mngr = std::make_unique<SystemMngr>();
}

Entity World::CreateEntity()
{
    return m_entity_mngr->CreateEntity();
}

void World::DestroyEntity(Entity entity)
{
    m_entity_mngr->DestroyEntity(entity);
    // 更新系统的实体集合
    // 实体被销毁时不需要使用到第三个参数，传递空签名即可
    m_system_mngr->UpdateEntitys(UpdateEntitysType::ENTITY_DESTROYED, entity, Signature());
}

Signature World::GetEntitySignature(Entity entity)
{
    return m_entity_mngr->GetSignature(entity);
}

std::set<EntityId> World::GetEntitys(Signature signature)
{
    return m_entity_mngr->GetEntitys(signature);
}

void World::Update(float dt)
{
    m_system_mngr->Update(dt);
}
