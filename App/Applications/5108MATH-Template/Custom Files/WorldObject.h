#pragma once

#include "FileSystem.h"

//Hieroglyph Includes
#include "GeometryActor.h"

//STL Includes
#include <vector>

//LJMU Framework Includes

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<Glyph3::BasicVertexDX11::Vertex>> BasicMeshPtr;

class WorldObject : public Glyph3::GeometryActor
{
public:
	void LoadObject(const std::wstring& filename, Glyph3::Vector4f colour, Glyph3::ResourcePtr texture, Glyph3::RendererDX11* renderTarget);
	BasicMeshPtr GenerateOBJMesh(std::wstring meshname, Glyph3::Vector4f colour = Glyph3::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	void AddParent(Actor* node);
	void LoadPhongTexturedMaterial(Glyph3::RendererDX11* renderer);

	void Update(bool shading, Glyph3::RendererDX11* renderTarget);

private:
	Glyph3::FileSystem m_fileSystem; //Holds an instance of the Hieroglyph File System
	Glyph3::MaterialPtr m_phongTexturedMaterial;

	Glyph3::ResourcePtr m_texture;
};

