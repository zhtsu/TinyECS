#pragma once

#include <cassert>
#include <map>
#include <array>
#include "Types.h"

/* 
  组件容器接口类
  组件容器模板类将会继承于此类
  以支持在不确定类型时的声明
*/ 
class ICompContainer
{
public:
    virtual ~ICompContainer() = default;
    // 移除容器内的一个组件
    virtual void RemoveComp(EntityId eid) = 0;
};

/* 
  组件容器类
  这是一个模板类
  一个容器对象只能存储一种类型的组件
  并保证容器内的组件紧密存储
*/ 
template<typename T>
class CompContainer : public ICompContainer
{
public:
    /*
      初始化容器内组件数量为 0
    */
    CompContainer() : m_current_comp_num(0) {}

    /* 
      向容器内添加一个组件
      \param entity 组件所属实体
      \param comp   被添加的组件
    */ 
    void AddComp(EntityId eid, T comp);

    /* 
      移除容器内的一个组件
      重载自 ICompContainer
      本方法允许通过父类指针调用
      容器内组件的紧密存储体现于此
      详情请参考此函数的具体实现
      \param entity 要移除的组件所属的实体
    */ 
    void RemoveComp(EntityId eid) override;

    /* 
      获取容器内的一个组件
      \param entity 要获取的组件所属的实体
    */ 
    T& GetComp(EntityId eid);

    /* 
      检查容器内是否包含属于某个实体的组件
      \param entity 待检查的实体
    */ 
    bool HaveComp(EntityId eid);

private:
    // 记录实体与其对应的组件在容器中的下标
    std::map<EntityId, int> m_eid_to_idx;
    // 与 m_entity_to_idx 相反
    std::map<int, EntityId> m_idx_to_eid;
    // 组件数组
    std::array<T, MAX_COMP_NUM> m_comps;
    // 当前的组件数量
    int m_current_comp_num;
};

/* 
  TODO: 后续考虑增加移动语义
*/ 
template<typename T>
void CompContainer<T>::AddComp(EntityId eid, T comp)
{
    // 该实体未拥有此种类型组件时添加才会生效
    if (m_eid_to_idx.find(eid) == m_eid_to_idx.end())
    {
        int idx = m_current_comp_num;
        m_eid_to_idx.insert({eid, idx});
        m_idx_to_eid.insert({idx, eid});
        m_comps[ idx ] = comp;
        m_current_comp_num += 1;
    }
}

template<typename T>
void CompContainer<T>::RemoveComp(EntityId eid)
{  
    // 该实体拥有此种组件时移除才会生效
    if (m_eid_to_idx.find(eid) != m_eid_to_idx.end())
    {
        // 记录下被移除组件的下标
        int removed_comp_index = m_eid_to_idx[ eid ];
        // 记录下容器中最后一个组件对应的实体
        int last_comp_entity = m_idx_to_eid[ m_current_comp_num - 1 ];
        // 获取到容器中最后一个组件
        T last_comp = m_comps[ m_current_comp_num - 1 ];
        // 用最后一个组件覆盖被删除的组件
        m_comps[ removed_comp_index ] = std::move(last_comp);
        // 更新最后组件被移动后对应实体的索引信息
        m_eid_to_idx[ last_comp_entity ] = removed_comp_index;
        m_idx_to_eid[ removed_comp_index ] = last_comp_entity;
        // 移除索引信息
        m_eid_to_idx.erase(eid);
        m_idx_to_eid.erase(m_current_comp_num - 1);
        // 计数更新
        m_current_comp_num -= 1;
    }
}

template<typename T>
T& CompContainer<T>::GetComp(EntityId eid)
{
    int idx = -1;
 
    if (m_eid_to_idx.find(eid) != m_eid_to_idx.end())
    {
        idx = m_eid_to_idx[ eid ];
    }

    // 如果该实体未拥有此类型组件则报错
    assert(idx != -1 && "This entity does not own components of this type");

    return m_comps[ idx ];
}

template<typename T>
bool CompContainer<T>::HaveComp(EntityId eid)
{
    if (m_eid_to_idx.find(eid) == m_eid_to_idx.end())
    {
        return false;
    }

    return true;
}
