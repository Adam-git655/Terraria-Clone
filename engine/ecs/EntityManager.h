#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>

#include "Entity.h"
#include "ComponentStorage.h"

class EntityManager
{
public:
	Entt create()
	{
		return entityID++;
	}

	template<typename T>
	void addComponent(Entt e, const T& component)
	{
		getStorage<T>().add(e, component);
	}

	template<typename T>
	void removeComponent(Entt e)
	{
		getStorage<T>().remove(e);
	}

	template<typename T>
	bool hasComponent(Entt e)
	{
		return getStorage<T>().has(e);
	}

	template<typename T>
	T& getComponent(Entt e)
	{
		return getStorage<T>().get(e);
	}

	template<typename T>
	ComponentStorage<T>& getComponentStorage()
	{
		return getStorage<T>();
	}

	void destroy(Entt e)
	{
		for (auto& [type, storage] : storages)
		{
			storage->remove(e);
		}
	}

private:
	Entt entityID = 0;

	std::unordered_map<std::type_index, std::shared_ptr<IComponentStorage>> storages;

	template<typename T>
	ComponentStorage<T>& getStorage()
	{
		std::type_index index = typeid(T);

		if (storages.find(index) == storages.end())
		{
			storages[index] = std::make_shared<ComponentStorage<T>>();
		}

		return *static_cast<ComponentStorage<T>*>(storages[index].get());
	}

};