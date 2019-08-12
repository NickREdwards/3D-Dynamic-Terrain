#ifndef ENGINE_GEOMETRY_WATERPLANE_H
#define ENGINE_GEOMETRY_WATERPLANE_H

#include "Object3D.h"
#include "Vertex.h"

namespace Engine 
{
namespace Geometry 
{

class WaterPlane: public Object3D {
public:
	WaterPlane() {}
	~WaterPlane() {}

	RVAL InitMesh(ID3D11Device* const) override;

private:

};

}
}

#endif