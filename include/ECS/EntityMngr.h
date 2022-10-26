#pragma once

#include <queue>
#include <map>
#include <bitset>
#include <memory>
#include <set>
#include "Types.h"
#include "CompContainer.h"

/* 
  实体管理器
  负责实体的创建于销毁
  为实体移除或添加组件也通过实体管理器完成
*/ 
class EntityMngr
{
public:
    EntityMngr();

    /* 
      创建一个实体并返回
    */ 
    EntityId CreateEntity();

    /* 
      销毁一个实体
      \param eid 需要销毁的实体 ID，EntityId 类型可直接使用 Entity 类型传参
    */ 
    void DestroyEntity(EntityId eid);

    /* 
      获取到拥有指定签名的所有实体
    */ 
    std::set<EntityId> GetEntitys(Signature sig);

    /* 
      获取一个实体的签名
    */ 
    Signature GetSignature(EntityId eid);

    /* 
      模板函数
      将指定类型的组件添加到目标实体
      \param eid 目标实体
      \return    被添加的组件
    */ 
    template<typename T>
    T& AtachComp(EntityId eid, T comp);

    /* 
      模板函数
      将指定类型的组件从实体中移除
    */ 
    template<typename T>
    void DeAtachComp(EntityId eid);

    /* 
      模板函数
      判断实体是否拥有指定类型的组件
    */ 
    template<typename T>
    bool HaveComp(EntityId eid);

    /* 
      模板函数
      从实体中获取指定类型的组件
    */ 
    template<typename T>
    T& GetComp(EntityId eid);

private:
    /* 
      组件信息变化时，更新实体的签名信息
      \param eid                  目标实体
      \param changed_comp_type_Id 目标实体更新的组件类型
      \param changed_type         组件变更类型，false 为移除组件，true 为添加组件
    */ 
    void UpdateSignature(EntityId eid, CTID changed_comp_type_Id, bool changed_type);

    /* 
      获取到一个唯一的组件类型 ID
    */ 
    CTID GetUniqueCTID();

    /* 
      模板函数
      获取一个组件的类型 ID
      若组件未曾注册，则会在此函数中进行注册
      即为其类型分配一个唯一的组件类型 ID
    */ 
    template<typename T>
    CTID GetCompTypeId();

    // 当前实体总数
    int m_entity_num;
    // 当前可用的实体 ID
    std::queue<EntityId> m_available_eids;
    // 记录每个实体对应的签名
    std::map<EntityId, Signature> m_eid_to_signature;
    // 记录每种签名下已有的实体，键值为每种签名对应的数值型
    std::map<unsigned long, std::set<EntityId> > m_signature_to_eids;
    // 存储不同类型的组件
    std::map<const char*, std::shared_ptr<ICompContainer> > m_type_to_comp_container;
};

template<typename T>
CTID EntityMngr::GetCompTypeId()
{
    // 如果当前组件类型未被注册，则将其注册，以确保其持有一个唯一类型 ID
    static const CTID comp_type_Id = GetUniqueCTID();
    return comp_type_Id;
}

template<typename T>
T& EntityMngr::AtachComp(EntityId eid, T comp)
{
    // 如果实体不存在，进行警告
    assert (m_eid_to_signature.find(eid) != m_eid_to_signature.end() &&
        "Entity does not exist");

    // 获取当前组件类型的类型 ID
    CTID current_CTID = GetCompTypeId<T>();
    // 获取类型的说明字符作为键值
    const char* comp_type = typeid(T).name();

    // 如果该类型的组件容器尚不存在，则进行创建
    if (m_type_to_comp_container.find(comp_type) == m_type_to_comp_container.end())
    {
        m_type_to_comp_container.insert(
            {comp_type, std::make_shared<CompContainer<T> >()}
        );
    }

    // 将 ICompContainer 类智能指针转化为对应的模板容器类
    std::shared_ptr<CompContainer<T> > comp_container
        = std::static_pointer_cast<CompContainer<T> >(
            m_type_to_comp_container[ comp_type ]
    );

    // 如果实体已经拥有了该组件
    // 直接返回已有组件
    if (m_eid_to_signature[ eid ][ current_CTID ] == 1)
    {
        return comp_container->GetComp(eid);
    }
    // 如果实体尚未拥有该组件
    // 为其添加组件
    comp_container->AddComp(eid, comp);
    // 实体组件信息变化，更新签名信息
    UpdateSignature(eid, current_CTID, true);

    return comp_container->GetComp(eid);
}

template<typename T>
void EntityMngr::DeAtachComp(EntityId eid)
{
    assert (m_eid_to_signature.find(eid) != m_eid_to_signature.end() &&
        "Entity does not exist");

    CTID current_CTID = GetCompTypeId<T>();
    const char* comp_type = typeid(T).name();
    std::shared_ptr<CompContainer<T> > comp_container
        = std::static_pointer_cast<CompContainer<T> >(
            m_type_to_comp_container[ comp_type ]
    );

    // 如果实体尚未拥有该组件
    // 不执行任何操作
    if (m_eid_to_signature[ eid ][ current_CTID ] == 0)
    {
        return ;
    }
    // 如果实体拥有该组件
    // 为其移除组件
    comp_container->RemoveComp(eid);

    UpdateSignature(eid, current_CTID, false);
}

template<typename T>
bool EntityMngr::HaveComp(EntityId eid)
{
    assert (m_eid_to_signature.find(eid) != m_eid_to_signature.end() &&
        "Entity does not exist");

    const char* comp_type = typeid(T).name();
    std::shared_ptr<CompContainer<T> > comp_container
        = std::static_pointer_cast<CompContainer<T> >(
            m_type_to_comp_container[ comp_type ]
    );

    return comp_container->HaveComp(eid);
}

template<typename T>
T& EntityMngr::GetComp(EntityId eid)
{
    assert (m_eid_to_signature.find(eid) != m_eid_to_signature.end() &&
        "Entity does not exist");

    CTID current_CTID = GetCompTypeId<T>();
    const char* comp_type = typeid(T).name();
    std::shared_ptr<CompContainer<T> > comp_container
        = std::static_pointer_cast<CompContainer<T> >(
            m_type_to_comp_container[ comp_type ]
    );

    assert (m_eid_to_signature[ eid ][ current_CTID ] == 1 &&
        "The component is not included in the entity!");

    return comp_container->GetComp(eid);
}
