#pragma once

#include <memory>
#include "ECS/EntityMngr.h"
#include "ECS/SystemMngr.h"

/* 
  世界类
  管理所有实体和系统
  封装访问实体管理器和系统管理器的方法
  以确保数据更新时能够同步到所有管理器
*/ 
class World
{
public:
    World();

    /* 
      创建一个实体
    */ 
    Entity CreateEntity();

    /* 
      销毁一个实体
    */ 
    void DestroyEntity(Entity entity);

    /* 
      获取一个实体的签名
    */ 
    Signature GetEntitySignature(Entity entity);

    /*
      根据签名获取实体集合
    */
   std::set<EntityId> GetEntities(Signature signature);

    /* 
      模板函数
      向一个实体添加组件
    */ 
    template<typename T>
    T& AtachComp(Entity entity, T comp);

    /* 
      模板函数
      从实体中移除组件
    */ 
    template<typename T>
    void DeAtachComp(Entity entity);

    /* 
      模板函数
      获取到实体的组件
    */ 
    template<typename T>
    T& GetComp(Entity entity);

    /* 
      模板函数
      查询一个实体是否拥有组件
    */ 
    template<typename T>
    bool HaveComp(Entity entity);

    /* 
      模板函数
      注册一个系统
      \param signature 系统关注的实体签名
    */ 
    template<typename T>
    void RegisterSys(Signature signature);

    /* 
      更新一帧
      调用系统管理器的更新方法
      \param dt 当前帧与上一帧的间隔时间
    */ 
    void Update(float dt);

private:
    std::unique_ptr<EntityMngr> m_entity_mngr;
    std::unique_ptr<SystemMngr> m_system_mngr;
};

template<class T> 
T& World::AtachComp(Entity entity, T comp)
{
    m_entity_mngr->AtachComp<T>(entity, comp);
    // 获取当前实体的签名
    Signature signature = m_entity_mngr->GetSignature(entity);
    // 实体签名变更，更新系统订阅的实体集合
    m_system_mngr->UpdateEntities(UpdateEntitiesType::ENTITY_SIGNATURE_UPDATED, entity, signature);
    
    return m_entity_mngr->GetComp<T>(entity);
}

template<class T>
void World::DeAtachComp(Entity entity)
{
    m_entity_mngr->DeAtachComp<T>(entity);
    Signature signature = m_entity_mngr->GetSignature(entity);
    m_system_mngr->UpdateEntities(UpdateEntitiesType::ENTITY_SIGNATURE_UPDATED, entity, signature);
}

template<class T>
T& World::GetComp(Entity entity)
{
    return m_entity_mngr->GetComp<T>(entity);
}

template<class T>
bool World::HaveComp(Entity entity)
{
    return m_entity_mngr->HaveComp<T>(entity);
}

template<class T>
void World::RegisterSys(Signature signature)
{
    m_system_mngr->Register<T>(signature, this);
    // 注册系统之后更新系统关注的实体
    m_system_mngr->SetEntities<T>(m_entity_mngr->GetEntities(signature));
}
