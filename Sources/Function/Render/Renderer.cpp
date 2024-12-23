#include<chrono>

#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>


#include"Renderer.h"
#include"RenderResource.h"

namespace FOCUS
{
	Renderer::Renderer(DRHI::API api, DRHI::PlatformInfo platformCI)
	{
		switch (api)
		{
		default:
		case DRHI::VULKAN:
			DRHI::RHICreateInfo rhiCI{};
			rhiCI.platformInfo = platformCI;
			_rhiContext = std::make_shared<DRHI::VulkanDRHI>(rhiCI);
			break;
		}

		_shadowMap = std::make_shared<ShadowMap>();
	}

	void Renderer::initialize()
	{
		_rhiContext->initialize();

		

		_prepared = true;
	}

	void Renderer::buildAndSubmit(std::vector<std::shared_ptr<RenderResource>>* renderlist, std::vector<DRHI::DynamicCommandBuffer>* commandBuffers, DRHI::DynamicCommandPool* commandPool)
	{
		_submitRenderlist = *renderlist;
		_commandBuffers = *commandBuffers;
		_commandPool = *commandPool;

		_shadowMap->initialize(_rhiContext, &_commandPool);

		for (auto p : _submitRenderlist)
		{
			p->build(_rhiContext, &_commandPool, _shadowMap->_colorImage[0], _shadowMap->_colorImageView[0], _shadowMap->_shadowSampler);
		}

		buildCommandBuffer();
	}

	void Renderer::submitRenderTargetImage(std::vector<DRHI::DynamicImage>* viewportImages, std::vector<DRHI::DynamicImageView>* viewportImageViews, DRHI::DynamicImage* depthImage, DRHI::DynamicImageView* depthImageView)
	{
		_viewportImages = viewportImages;
		_viewportImageViews = viewportImageViews;
		_viewportDepthImage = depthImage;
		_viewportDepthImageView = depthImageView;
	}

	void Renderer::buildCommandBuffer()
	{
		if (_prepared)
		{
			auto aspectFlag = DRHI::DynamicImageAspectFlagBits(_rhiContext->getCurrentAPI());

			DRHI::DynamicRenderingInfo renderInfo{};
			renderInfo.isRenderOnSwapChain = false;
			renderInfo.isClearEveryFrame = true;

			// rendering shadow map
			for (int index = 0; index < _commandBuffers.size(); ++index)
			{
				//renderInfo.targetImage = &_shadowMap->_colorImage[0];
				//renderInfo.targetImageView = &_shadowMap->_colorImageView[0];
				//renderInfo.colorAspectFlag = aspectFlag.IMAGE_ASPECT_COLOR_BIT;
				//renderInfo.targetDepthImage = &_shadowMap->_depthImage;
				//renderInfo.targetDepthImageView = &_shadowMap->_depthImageView;
				//renderInfo.depthAspectFlag = aspectFlag.IMAGE_ASPECT_DEPTH_BIT;

				renderInfo.targetImage = &(*_viewportImages)[index];
				renderInfo.targetImageView = &(*_viewportImageViews)[index];
				renderInfo.colorAspectFlag = aspectFlag.IMAGE_ASPECT_COLOR_BIT;
				renderInfo.targetDepthImage = _viewportDepthImage;
				renderInfo.targetDepthImageView = _viewportDepthImageView;
				renderInfo.depthAspectFlag = aspectFlag.IMAGE_ASPECT_DEPTH_BIT | aspectFlag.IMAGE_ASPECT_STENCIL_BIT;
				renderInfo.isClearEveryFrame = true;

				_rhiContext->beginCommandBuffer(_commandBuffers[index]);
				_rhiContext->beginRendering(_commandBuffers[index], renderInfo);

				// binding shadow map pipeline  
				auto api = _rhiContext->getCurrentAPI();
				auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

				_rhiContext->bindPipeline(_shadowMap->_shadowPipeline, &_commandBuffers[index], bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
				_rhiContext->bindDescriptorSets(&_shadowMap->_descriptorSet, _shadowMap->_shadowPipelineLayout, &_commandBuffers[index], 0);

				for (auto p : _submitRenderlist)
				{
					p->draw(_rhiContext, &_commandBuffers[index], false);
				}

				_rhiContext->endRendering(_commandBuffers[index], renderInfo);
				_rhiContext->endCommandBuffer(_commandBuffers[index]);
			}

			// rendering scene
			/*for (int index = 0; index < _commandBuffers.size(); ++index)
			{
				renderInfo.targetImage = &(*_viewportImages)[index];
				renderInfo.targetImageView = &(*_viewportImageViews)[index];
				renderInfo.colorAspectFlag = aspectFlag.IMAGE_ASPECT_COLOR_BIT;
				renderInfo.targetDepthImage = _viewportDepthImage;
				renderInfo.targetDepthImageView = _viewportDepthImageView;
				renderInfo.depthAspectFlag = aspectFlag.IMAGE_ASPECT_DEPTH_BIT;

				_rhiContext->beginCommandBuffer(_commandBuffers[index]);
				_rhiContext->beginRendering(_commandBuffers[index], renderInfo);

				for (auto p : _submitRenderlist)
				{
					p->draw(_rhiContext, &_commandBuffers[index], true);
				}

				_rhiContext->endRendering(_commandBuffers[index], renderInfo);
				_rhiContext->endCommandBuffer(_commandBuffers[index]);
			}*/
		}
	}

	void Renderer::clean()
	{
		_prepared = false;

		_rhiContext->clean();
	}

	void Renderer::recreate()
	{
		_prepared = true;
		buildCommandBuffer();
	}
}