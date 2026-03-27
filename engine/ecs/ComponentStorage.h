#pragma once
#include <Entity.h>
#include <unordered_map>
#include <vector>

template<typename T>
class ComponentStorage
{
public:
	void add(Entt e, const T& component)
	{
		components[e] = component;
	}

	bool has(Entt e)
	{
		return components.find(e) != components.end();
	}

	T& get(Entt e)
	{
		return components[e];
	}

	std::unordered_map<Entt, T> getAll()
	{
		return components;
	}

private:
	std::unordered_map<Entt, T> components;
};