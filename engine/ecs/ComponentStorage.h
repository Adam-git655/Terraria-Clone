#pragma once
#include "engine/ecs/Entity.h"
#include <unordered_map>
#include <vector>

class IComponentStorage
{
public:
	virtual ~IComponentStorage() = default;
	virtual void remove(Entt e) = 0;
};

template<typename T>
class ComponentStorage : public IComponentStorage
{
public:
	void add(Entt e, const T& component)
	{
		components[e] = component;
	}

	void remove(Entt e) override
	{
		components.erase(e);
	}

	bool has(Entt e)
	{
		return components.find(e) != components.end();
	}

	T& get(Entt e)
	{
		return components[e];
	}

	std::unordered_map<Entt, T>& getAll()
	{
		return components;
	}

private:
	std::unordered_map<Entt, T> components;
};