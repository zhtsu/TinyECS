#pragma once

#include <bitset>

// 需要使用到的一些全局的类型或变量

// 容器的最大容量
const int MAX_COMP_NUM = 1000;
// 最大实体数量
const int MAX_ENTITY_NUM = 1000;
// 最大组件类型数量
const int MAX_COMP_TYPE_NUM = 64;

// 组件类型 ID
using CTID = unsigned int;
// 实体 ID，用来唯一标识一个实体
using EntityId = unsigned int;
// 实体即是一个 ID
using Entity = EntityId;
// 实体的签名，用来描述一个组件拥有哪些实体
using Signature = std::bitset<MAX_COMP_TYPE_NUM>;
