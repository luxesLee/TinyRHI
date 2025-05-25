#pragma once
#include "BaseType.h"

namespace TinyRHI
{
	struct ImageViewDesc
	{
		Uint32 baseMipLevel = 0;
		Uint32 mipLevelsCount = 1;
		Uint32 baseArrayLayer = 0;
		Uint32 arrayLayersCount = 1;
	};

	struct ImageUsage
	{
		Bool colorAttach = false;
		Bool depthAttach = false;
		Bool Storage = false;
		Bool Sample = false;
	};

	struct ImageDesc
	{
		enum class ImageType { e2D, e3D } imageType = ImageType::e2D;
		ImageViewDesc imageViewDesc;
		Uint size3[3] = { 0, 0, 0 };
		Format format = Format::Undefined;
		MSAASamples samples = MSAASamples::e1;
		Bool bDepth = false;
		Bool bStaging = false;
		ImageUsage usage;
	};

	class IImage
	{
	public:
		virtual ~IImage() {}
	};

	class IImageView
	{
	public:
		virtual ~IImageView() {}
	};

	class ITexture
	{
	public:
		virtual ~ITexture() {}
	};
}