#pragma once

#include <cstdint>

using Uint8 = std::uint8_t;
using Uint16 = std::uint16_t;
using Uint32 = std::uint32_t;
using Uint64 = std::uint64_t;
using Int8 = std::int8_t;
using Int16 = std::int16_t;
using Int32 = std::int32_t;
using Int64 = std::int64_t;
using Bool32 = std::uint32_t;
using Bool = bool;
using Char = char;
using Uchar = unsigned char;
using Short = short;
using Ushort = unsigned short;
using Int = int;
using Uint = unsigned int;
using Wchar = wchar_t;
using Float = float;
using Float64 = double;

struct Extent2D
{
	Uint32 width, height;
};

struct Extent3D
{
	Uint32 width, height, depth;
};

enum class MSAASamples { e1, e2, e4, e8, e16 };

struct ClearValues
{
	Float color[4];
	Float depth; 
	Uint32 stencil;
};

enum class Format
{
	Undefined,
	R8_UINT,
	R32_UINT,
	R32_FLOAT,
	RGB8_UNORM,
	RGBA8_UNORM,
	BGRA8_SRGB,
	RGBA32_FLOAT,
	D32_FLOAT,
	D24_UNORM_S8_UINT,
};

enum class CompOp { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
