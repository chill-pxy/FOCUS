#include "MaterialManager.h"

namespace FOCUS
{
	std::shared_ptr<BlinnPhongMaterial> MaterialManager::createMaterial(std::string name, std::shared_ptr<Texture> texture)
	{
		if (_globalMaterials.count(name) == 0)
		{
			std::shared_ptr<BlinnPhongMaterial> material = std::make_shared<BlinnPhongMaterial>(texture);
			_globalMaterials.insert({ name, material });
			return material;
		}
		else
		{
			return _globalMaterials[name];
		}
	}

	std::shared_ptr<BlinnPhongMaterial> MaterialManager::getMaterialByName(std::string name)
	{
		return _globalMaterials[name];
	}
}