#pragma once

#include<drhi.h>

#include<memory>

#include"Materials/Material.h"
#include"ShadowMap.h"
#include"DefferedPipeline.h"

namespace FOCUS
{
    enum RenderResourcePipeline
    {
        SCENE,
        SHADOW,
        DEFFERED
    };

    class RenderResource
    {
    public:
        std::string _name{ "unnamed object" };

        DRHI::DynamicBuffer       _vertexBuffer;
        DRHI::DynamicDeviceMemory _vertexDeviceMemory;
        
        DRHI::DynamicBuffer       _indexBuffer;   
        DRHI::DynamicDeviceMemory _indexDeviceMemory;

        std::shared_ptr<Material> _material{};
        std::shared_ptr<ShadowMap> _shadow{};
        std::shared_ptr<DefferedPipeline> _deffered{};

        bool _castShadow{ true };

        Vector3 _position{ 0.0,0.0,0.0 };
        Vector3 _color{ 1.0f, 1.0f, 1.0f };
        Vector3 _scale{ 1.0f, 1.0f, 1.0f };
        Matrix4 _rotation{1.0f};

    public:
        RenderResource() = default;

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool) = 0;
        virtual void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer, RenderResourcePipeline pipeline) = 0;
        virtual void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer, DRHI::DynamicPipeline pipeline, DRHI::DynamicPipelineLayout pipelineLayout, DRHI::DynamicDescriptorSet set) = 0;
        virtual void updateUniformBuffer(UniformUpdateData uud) = 0;
    };
}