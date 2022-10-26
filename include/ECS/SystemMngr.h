#pragma once

#include <map>
#include <set>
#include <memory>
#include "Types.h"

// 系统类需要使用到一个系统类的指针
class World;

/* 
  实体信息变更类型
  用于实体信息发生变更时
  决定怎样去更新系统关注的实体集合
*/ 
enum class UpdateEntitysType
{
    // TODO: 
    // 目前实体被创建时默认使用空签名，不被任何系统订阅
    // 所以实体被创建时不会更新系统的实体集合
    // 如果后续添加了实体[ 带签名创建 ]的功能时，需要添加此种枚举情况
    // // 实体被创建
    // ENTITY_CREATED,

    // 实体被销毁
    ENTITY_DESTROYED,
    // 实体签名更新，即组件信息变更（添加或移除组件）
    ENTITY_SIGNATURE_UPDATED
};

/* 
  系统基类
  用户自定义的系统需要继承自此类
  否则将不允许进行注册
*/ 
struct System
{
    System() : world(nullptr) {}
    // 此函数用来更新一次系统逻辑
    virtual void OnUpdate(float dt) {}

    // 系统所属的世界，用来操作实体
    World* world;
    // 系统关注的实体
    std::set<Entity> entitys;
};

/* 
  系统管理器
  允许注册系统
  管理所有被注册的系统
*/ 
class SystemMngr
{
public:
    /* 
      注册新系统
      这是一个模板函数
      此函数在注册的同时
      会获取当前系统关注的所有实体
      \param signature   关注的实体签名，用来决定系统关注哪些实体
      \param entity_mngr 和系统处于同一世界的实体管理器
    */ 
    template<typename T>
    void Register(Signature signature, World* world);

    /* 
      更新所有系统
      \param dt 当前帧与上一帧的间隔时间
    */ 
    void Update(float dt);

    /* 
      更新系统关注的实体
      在实体签名发生变化时
      即添加或删除组件时
      调用此函数来更新系统关注的实体集合
      \param update_type 实体信息变更类型，详情请查看此枚举定义
      \param entity      变更的实体
      \param signature   实体目前的签名
    */ 
    void UpdateEntitys(UpdateEntitysType update_type, Entity entity, Signature signature);

    /*
      这是提供给 World 类调用的函数
      以确保 World 注册一个系统时可以为其设置实体集合
    */
   template<class T>
    void SetEntitys(std::set<Entity> entitys);

private:
    // 每个系统关注的实体签名，用来决定系统关注哪些实体
    std::map<const char*, Signature> m_type_to_signature;
    // 被注册的系统
    std::map<const char*, std::shared_ptr<System> > m_type_to_system;
};

template<class T>
void SystemMngr::Register(Signature signature, World* world)
{
    // 若当前类不是继承自 System 则报错
    static_assert(
        std::is_base_of<System, T>::value,
        "This class does not inherit from System!"
    );

    const char* type_name = typeid(T).name();
    if (m_type_to_signature.find(type_name) == m_type_to_signature.end())
    {
        m_type_to_signature.insert({type_name, signature});
        m_type_to_system.insert({type_name, std::make_shared<T>()});
    }
    // 记录实体管理器
    m_type_to_system.find(type_name)->second->world = world;
}

template<class T>
void SystemMngr::SetEntitys(std::set<Entity> entitys)
{
    const char* type_name = typeid(T).name();
    // 更新系统订阅的实体集合
    m_type_to_system.find(type_name)->second->entitys = entitys;
}
