#ifndef ENGINE_GEOMETRY_MESHDRAWLOCK_H
#define ENGINE_GEOMETRY_MESHDRAWLOCK_H

#include "Mesh.h"

namespace Engine
{
namespace Geometry
{

class _declspec(dllexport) MeshDrawLock {
public:
	MeshDrawLock(IMesh* mesh)
		:	_mesh(mesh) {
		_mesh->BeginDraw();
	}

	~MeshDrawLock() {
		_mesh->EndDraw();
		_mesh = nullptr;
	}

private:
	IMesh* _mesh;
};

}
}

#endif