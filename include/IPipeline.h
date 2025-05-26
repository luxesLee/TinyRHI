#pragma once
#include <vector>
#include "BaseType.h"

namespace TinyRHI
{
	class IShader;
	class IRenderPass;

	struct PipelineLayoutDesc
	{

	};

	class IPipelineLayout
	{
	public:
		virtual ~IPipelineLayout() {}
	};

	//------------------------------------------------------------------

	enum class AttribType
	{
		Vec1, Vec2, Vec3, Vec4, Vec3_Unorm, Vec4_Unorm, Color32,
	};
	struct VertexDeclaration 
	{
		struct VertexBindingDesc 
		{
			Uint32 binding, stride;
			Bool bInstance;
		};

		struct VertexAttributeDesc 
		{
			Uint32 location, binding, offset;
			AttribType format;
		};

		std::vector<VertexBindingDesc> vertexBindings;
		std::vector<VertexAttributeDesc> attributeDescs;
	};

	struct InputAssemblyState
	{
		enum class PrimitiveTopology {
			PointList, LineList, LineStrip, TriangleList, TriangleStrip, TriangleFan,
			LineListAdjacency, LineStripAdjacency, TriangleListAdjacency, TriangleStripAdjacency,
			PatchList
		} topology = PrimitiveTopology::TriangleList;
		bool bPrimitiveRestart = false;
	};

	struct RasterizeState
	{
		bool depthClamp = false;
		enum class PolygonMode { Fill, Line, Point, } polygonMode = PolygonMode::Fill;
		float lineWidth = 1.0f;
		enum class CullMode { None, Front, Back, } cullMode = CullMode::None;
		enum class FrontFace { cw, ccw } frontFace = FrontFace::cw;
		bool depthBias = false;
	};

	enum class BlendSetting { Opaque, Add, Mixed, AlphaBlend, };

	struct DepthState
	{
		bool stencilTest = false;
		bool depthTest = false;
		bool depthWrite = false;
		Format depthFormat = Format::Undefined;
		CompOp depthTestComp = CompOp::Never;
	};

	struct GfxSetting
	{
		VertexDeclaration vertexDecl;
		InputAssemblyState inputAssemlyState;
		RasterizeState rasterizeState;
		MSAASamples samples = MSAASamples::e1;
		DepthState depthState;
		std::vector<BlendSetting> blendSettings;
	};

	struct GraphicsPipelineDesc
	{
		IShader* vertShader;
		IShader* pixelShader;
		IPipelineLayout* pipelineLayout;
		IRenderPass* renderPass;
		GfxSetting setting;
	};

	struct ComputePipelineDesc
	{
		IShader* compShader;
		IPipelineLayout* pipelineLayout;
	};

	class IPipeline
	{
	public:
		virtual ~IPipeline() {}
	};

	class IGraphicsPipeline : public IPipeline
	{
	};

	class IComputePipeline : public IPipeline
	{
	};
}

namespace std 
{
    template <>
    struct hash<TinyRHI::VertexDeclaration::VertexBindingDesc> 
	{
        std::size_t operator()(const TinyRHI::VertexDeclaration::VertexBindingDesc& vbd) const 
		{
            std::size_t res = 17;
            res = res * 31 + std::hash<Uint32>{}(vbd.binding);
            res = res * 31 + std::hash<Uint32>{}(vbd.stride);
            res = res * 31 + std::hash<Bool>{}(vbd.bInstance);
            return res;
        }
    };

    template <>
    struct hash<TinyRHI::VertexDeclaration::VertexAttributeDesc> 
	{
        std::size_t operator()(const TinyRHI::VertexDeclaration::VertexAttributeDesc& vad) const 
		{
            std::size_t res = 17;
            res = res * 31 + std::hash<Uint32>{}(vad.location);
            res = res * 31 + std::hash<Uint32>{}(vad.binding);
            res = res * 31 + std::hash<Uint32>{}(vad.offset);
            res = res * 31 + hash<TinyRHI::AttribType>{}(vad.format);
            return res;
        }
    };

    template <>
    struct hash<TinyRHI::VertexDeclaration> 
	{
        std::size_t operator()(const TinyRHI::VertexDeclaration& vd) const 
		{
            std::size_t res = 17;
            for (const auto& binding : vd.vertexBindings) 
			{
                res = res * 31 + hash<TinyRHI::VertexDeclaration::VertexBindingDesc>{}(binding);
            }
            for (const auto& attribute : vd.attributeDescs) 
			{
                res = res * 31 + hash<TinyRHI::VertexDeclaration::VertexAttributeDesc>{}(attribute);
            }
            return res;
        }
    };

    template <>
    struct hash<TinyRHI::InputAssemblyState> {
        std::size_t operator()(const TinyRHI::InputAssemblyState& state) const 
		{
            std::size_t res = 17;
            res = res * 31 + hash<TinyRHI::InputAssemblyState::PrimitiveTopology>{}(state.topology);
            res = res * 31 + std::hash<Bool>{}(state.bPrimitiveRestart);
            return res;
        }
    };

    template <>
    struct hash<TinyRHI::RasterizeState> 
	{
        std::size_t operator()(const TinyRHI::RasterizeState& state) const 
		{
            std::size_t res = 17;
            res = res * 31 + std::hash<Bool>{}(state.depthClamp);
            res = res * 31 + hash<TinyRHI::RasterizeState::PolygonMode>{}(state.polygonMode);
            res = res * 31 + std::hash<Float>{}(state.lineWidth);
            res = res * 31 + hash<TinyRHI::RasterizeState::CullMode>{}(state.cullMode);
            res = res * 31 + hash<TinyRHI::RasterizeState::FrontFace>{}(state.frontFace);
            res = res * 31 + std::hash<Bool>{}(state.depthBias);
            return res;
        }
    };

    template <>
    struct hash<TinyRHI::DepthState> 
	{
        std::size_t operator()(const TinyRHI::DepthState& state) const 
		{
            std::size_t res = 17;
            res = res * 31 + std::hash<Bool>{}(state.stencilTest);
            res = res * 31 + std::hash<Bool>{}(state.depthTest);
            res = res * 31 + std::hash<Bool>{}(state.depthWrite);
            res = res * 31 + hash<Format>{}(state.depthFormat);
            res = res * 31 + hash<CompOp>{}(state.depthTestComp);
            return res;
        }
    };

    template <>
    struct hash<TinyRHI::GfxSetting> 
	{
        std::size_t operator()(const TinyRHI::GfxSetting& setting) const 
		{
            std::size_t res = 17;
            res = res * 31 + hash<TinyRHI::VertexDeclaration>{}(setting.vertexDecl);
            res = res * 31 + hash<TinyRHI::InputAssemblyState>{}(setting.inputAssemlyState);
            res = res * 31 + hash<TinyRHI::RasterizeState>{}(setting.rasterizeState);
            res = res * 31 + std::hash<MSAASamples>{}(setting.samples);
            res = res * 31 + hash<TinyRHI::DepthState>{}(setting.depthState);
            for (const auto& blend : setting.blendSettings) 
			{
                res = res * 31 + hash<TinyRHI::BlendSetting>{}(blend);
            }
            return res;
        }
    };
}
