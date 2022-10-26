#include <ctime>
#include <iostream>

#include "World.h"

// 自定义两个组件
struct CompA
{
	float x;
	float y;
};

struct CompB
{
	int a;
	int b;
};

// 自定义一个系统
// 需要继承自 System 类
// 否则无法进行注册
struct TestSys : public System
{
	// 重写更新方法
	void OnUpdate(float dt)
	{
		// 通过 entitys 集合遍历实体列表
		for (auto entity : entitys)
		{
			// 通过 world 指针操作实体的组件
			world->GetComp<CompA>(entity).x += 1;
			world->GetComp<CompA>(entity).y += 1;
			world->GetComp<CompB>(entity).a += 2;
			world->GetComp<CompB>(entity).b += 2;
		}
	}
};

int main(int argc, char *argv[])
{
	// 创建一个世界
	World w;
	// 创建一个实体
	Entity test = w.CreateEntity();
	// 为实体添加两个组件
	w.AtachComp<CompA>(test, CompA{0.0f, 0.0f});
	w.AtachComp<CompB>(test, CompB{0, 0});
	// 使用实体 test 的签名注册一个系统
	w.RegisterSys<TestSys>(w.GetEntitySignature(test));

	float dt = 0.0f;
	while (true)
	{
		auto start_time = clock();
		w.Update(dt);
		dt = clock() - start_time;
		// 查看实体的组件信息
		// 若一切正常则 CompA 的组件信息总是 CompB 的 1/2
		std::cout << "CompA: "
				  << w.GetComp<CompA>(test).x 
				  << "  CompB: " 
				  << w.GetComp<CompB>(test).a
				  << std::endl;
	}

	return 0;
}
