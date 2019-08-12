#ifndef ENGINE_GRAPHICS_EFFECTSTRUCTURES_H
#define ENGINE_GRAPHICS_EFFECTSTRUCTURES_H

#include <Windows.h>
#include <DirectXMath.h>

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

namespace Engine 
{
namespace Graphics
{

struct DirectionalLight {
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
};

struct PointLight {
	PointLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Attenuation;	// w = range
	XMFLOAT3 Position;
};

struct SpotLight {
	SpotLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	XMFLOAT3 Attenuation;
	XMFLOAT3 Position;
	float Range;
	float Spot;
};

}
}

#endif