#include "ECS/EntityMngr.h"


EntityMngr::EntityMngr()
{
    m_entity_num = 0;
    // 初始化可用的实体 ID
    for (EntityId eid = 0; eid < MAX_ENTITY_NUM; eid++)
    {
        m_available_eids.push(eid);
    }
}

EntityId EntityMngr::CreateEntity()
{
    // 取出一个当前可用的实体 ID
    EntityId eid = m_available_eids.front();
    m_available_eids.pop();
    // 实体默认签名为空
    Signature signature;
    unsigned long sig_long = signature.to_ulong();
    // 更新实体与签名的索引信息
    m_eid_to_signature.insert({eid, std::move(signature)});
    // 若目前不存在使用此签名的集合，则创建一个
    if (m_signature_to_eids.find(sig_long) == m_signature_to_eids.end())
    {
        std::set<EntityId> temp_set;
        m_signature_to_eids.insert({sig_long, std::move(temp_set)});
    }
    m_signature_to_eids[ sig_long ].insert(eid);
    // 更新实体数量
    m_entity_num += 1;

    return eid;
}

void EntityMngr::DestroyEntity(EntityId eid)
{
    if (m_eid_to_signature.find(eid) != m_eid_to_signature.end())
    {
        Signature removed_entity_sig = m_eid_to_signature[ eid ];
        m_eid_to_signature.erase(eid);
        m_signature_to_eids[ removed_entity_sig.to_ulong() ].erase(eid);
        // 清除属于该实体的组件
        std::map<const char*, std::shared_ptr<ICompContainer> >::iterator iter;
        iter = m_type_to_comp_container.begin();
        while (iter != m_type_to_comp_container.end())
        {
            iter->second->RemoveComp(eid);
            iter++;
        }
        // 回收 ID
        m_available_eids.push(eid);
        // 更新实体数量
        m_entity_num -= 1;
    }
}

std::set<EntityId> EntityMngr::GetEntitys(Signature sig)
{
    unsigned long sig_long = sig.to_ulong();
    if (m_signature_to_eids.find(sig_long) == m_signature_to_eids.end())
    {
        return std::set<EntityId>();
    }

    return m_signature_to_eids[ sig_long ];
}

Signature EntityMngr::GetSignature(EntityId eid)
{
    if (m_eid_to_signature.find(eid) == m_eid_to_signature.end())
    {
        return Signature();
    }

    return m_eid_to_signature[ eid ];
}

CTID EntityMngr::GetUniqueCTID()
{
    static CTID unique_Id = 0;
    // 如果组件类型数量达到最大值，进行报警
    assert(unique_Id < MAX_COMP_TYPE_NUM &&
        "The number of component types has reached the maximum!");

    return unique_Id++;
}

void EntityMngr::UpdateSignature(EntityId eid, CTID changed_comp_type_Id, bool changed_type)
{
    // 获取实体当前的签名
    unsigned long old_sig_long = m_eid_to_signature[ eid ].to_ulong();
    // 将实体从旧签名集合中清除
    m_signature_to_eids[ old_sig_long ].erase(eid);
    // 更新实体的签名
    m_eid_to_signature[ eid ][ changed_comp_type_Id ] = changed_type;
    // 获取实体的新签名
    unsigned long new_sig_long = m_eid_to_signature[ eid ].to_ulong();
    // 如果当前签名不存在，则为此签名创建一个新集合
    if (m_signature_to_eids.find(new_sig_long) == m_signature_to_eids.end())
    {
        std::set<EntityId> temp_set;
        m_signature_to_eids.insert({new_sig_long, std::move(temp_set)});
    }
    // 将实体加入新签名集合
    m_signature_to_eids[ new_sig_long ].insert(eid);
}
