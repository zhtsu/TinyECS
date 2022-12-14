#include "ECS/SystemMngr.h"

void SystemMngr::Update(float dt)
{
    // 调用所有系统的更新函数
    for (auto pair : m_type_to_system)
    {
        pair.second->OnUpdate(dt);
    }
}

void SystemMngr::UpdateEntities(UpdateEntitiesType update_type, Entity entity, Signature signature)
{
    switch(update_type)
    {
        // 实体被销毁时，将实体从所有系统的实体集合中移除
        case UpdateEntitiesType::ENTITY_DESTROYED:

            for (auto pair : m_type_to_system)
            {
                pair.second->entities.erase(entity);
            }

            break;

        // 当实体签名被更新时，即添加或移除组件时
        case UpdateEntitiesType::ENTITY_SIGNATURE_UPDATED:
            for (auto pair : m_type_to_system)
            {
                const char* type_name = pair.first;
                Signature system_signature = m_type_to_signature[ type_name ];
                if ((signature & system_signature) == system_signature)
                {
                    pair.second->entities.insert(entity);
                }
                else
                {
                    pair.second->entities.erase(entity);
                }
            }

            break;
    }
}
