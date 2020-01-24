#pragma once

#include "Timer.h"

//Hieroglyph Includes
#include "GeometryActor.h"

using namespace Glyph3;

namespace LJMUDX
{
	class Planet : public GeometryActor
	{
	public:
		void Init(const std::wstring& filename, float radius, Node3D* parent, Vector3f tiltAngle, Vector3f tiltPos, RendererDX11* renderer);
		Node3D* CreateTiltNode(Vector3f angle, Vector3f position, Node3D* parent);

		void Update(bool shading, RendererDX11* renderer);

	private:
		void LoadPlanet(const std::wstring& filename, float radius, Node3D* parent, RendererDX11* renderer);
		void LoadPhongTexturedMaterial(RendererDX11* renderer);
		
		Node3D*	m_pTiltNode; //Node to tilt planets on angle
		MaterialPtr m_phongTexturedMaterial;
		ResourcePtr m_texture;
	};
}
