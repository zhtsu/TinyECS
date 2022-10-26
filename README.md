# TinyECS
[![license](https://img.shields.io/github/license/zhtsu/TinyECS)](LICENSE) 

## Intro
Tiny Entity Component System.

## How to run
### Environment：
- Windows 10 or later
- CMake 3.23.0
- gcc 8.1.0 (MinGW-W64)

### Run
Only double-click Run.bat

## Usage sample
```cpp
#include <ctime>
#include <iostream>

#include "World.h"

// custom Components
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

// custom TestSys
// need to be inherited from class System
// if not, will not be able to call RegisterSys successfully
struct TestSys : public System
{
	// override
	void OnUpdate(float dt) override
	{
		// use variable entitys to traverse all entities
		for (auto entity : entitys)
		{
			// use pointer world to operating entity
			world->GetComp<CompA>(entity).x += 1;
			world->GetComp<CompA>(entity).y += 1;
			world->GetComp<CompB>(entity).a += 2;
			world->GetComp<CompB>(entity).b += 2;
		}
	}
};

int main(int argc, char *argv[])
{
	// define a World
	World w;
	// create a entity
	Entity test = w.CreateEntity();
	// add components to entity that named test
	w.AtachComp<CompA>(test, CompA{0.0f, 0.0f});
	w.AtachComp<CompB>(test, CompB{0, 0});
	// use the signature of test to call RegisterSys
	w.RegisterSys<TestSys>(w.GetEntitySignature(test));

	float dt = 0.0f;
	while (true)
	{
		auto start_time = clock();
		w.Update(dt);
		dt = clock() - start_time;
		// if the registered TestSys is working
		// CompA's x is always 1/2 of CompB's a
		std::cout << "CompA: "
				  << w.GetComp<CompA>(test).x 
				  << "  CompB: " 
				  << w.GetComp<CompB>(test).a
				  << std::endl;
	}

	return 0;
}

```
