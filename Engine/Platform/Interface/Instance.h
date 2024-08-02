#pragma once
#include<variant>

#include<volk.h>

namespace FOCUS
{
	namespace Platform
	{
		class Instance
		{
		public:
			Instance();

			//intΪ�������ͣ�ʵ��Ӧ���Ϊ����API��instance����
			std::variant<VkInstance*, int> _runtimeInstance;

			//void createSwapChain();

			//void getNumPhysicalDevices();
			//void getAllPhysicalDevices();
			//void getPhysicalDevice();
			
		};


		void createInstance(Instance* instance);
	}
}