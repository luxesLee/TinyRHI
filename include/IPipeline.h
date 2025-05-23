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
			uint32_t binding, stride;
			Bool bInstance;
		};

		struct VertexAttributeDesc 
		{
			uint32_t location, binding, offset;
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

	struct GraphicsPipelineDesc
	{
		IShader* vertShader;
		IShader* pixelShader;
		VertexDeclaration vertexDecl;
		InputAssemblyState inputAssemlyState;
		RasterizeState rasterizeState;
		MSAASamples samples = MSAASamples::e1;
		DepthState depthState;
		std::vector<BlendSetting> blendSettings;
		IPipelineLayout* pipelineLayout;
		IRenderPass* renderPass;
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