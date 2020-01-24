#include "WorldObject.h"

#include "../LJMU Framework/LJMUShaderMaker.h"
#include "../LJMU Framework/LJMUMeshOBJ.h"

using namespace Glyph3;
using namespace LJMUDX;

void WorldObject::LoadObject(const std::wstring& filename, Vector4f colour, Glyph3::ResourcePtr texture, RendererDX11* renderTarget)
{
	MaterialPtr material = LJMUShaderMaker::MakeActorTextured(*renderTarget);
	material->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
	GetBody()->SetGeometry(GenerateOBJMesh(filename, colour));
	GetBody()->SetMaterial(material);

	LoadPhongTexturedMaterial(renderTarget);

	//GetBody()->SetMaterial(m_phongTexturedMaterial);
	//GetBody()->GetMaterial()->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
	
	GetBody()->SetGeometry(GenerateOBJMesh(filename, colour));

	m_texture = texture;
}

BasicMeshPtr WorldObject::GenerateOBJMesh(std::wstring meshname, Vector4f colour)
{
	std::wstring folder = m_fileSystem.GetModelsFolder() + meshname;

	LJMUDX::LJMUMeshOBJ* mesh = new LJMUDX::LJMUMeshOBJ(m_fileSystem.GetModelsFolder() + meshname);
	int vertcount = mesh->positions.size();

	auto ia = std::make_shared<DrawExecutorDX11<BasicVertexDX11::Vertex>>();
	ia->SetLayoutElements(BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements);
	ia->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ia->SetMaxVertexCount(vertcount);

	BasicVertexDX11::Vertex vertex;
	vertex.color = colour;

	for (auto& object : mesh->objects)
	{
		for (auto& face : object.faces)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				vertex.position = mesh->positions[face.PositionIndices[i]];
				vertex.normal = mesh->normals[face.NormalIndices[i]];
				vertex.texcoords = mesh->coords[face.CoordIndices[i]];
				ia->AddVertex(vertex);
			}
		}
	}

	return ia;
}

void WorldObject::AddParent(Actor* parent)
{
	parent->GetNode()->AttachChild(this->GetNode());
}

void WorldObject::LoadPhongTexturedMaterial(RendererDX11* renderer)
{
	m_phongTexturedMaterial = MaterialPtr(new MaterialDX11());

	RenderEffectDX11* pEffect = new RenderEffectDX11;
	pEffect->SetVertexShader(renderer->LoadShader(VERTEX_SHADER, std::wstring(L"PhongTextured.hlsl"), std::wstring(L"VSMAIN"), std::wstring(L"vs_4_0")));
	pEffect->SetPixelShader(renderer->LoadShader(PIXEL_SHADER, std::wstring(L"PhongTextured.hlsl"), std::wstring(L"PSMAIN"), std::wstring(L"ps_4_0")));

	m_phongTexturedMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_phongTexturedMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
}

void WorldObject::Update(bool shading, RendererDX11* renderTarget)
{
	if (shading)
	{
		GetBody()->SetMaterial(m_phongTexturedMaterial);
		GetBody()->GetMaterial()->Parameters.SetShaderResourceParameter(L"ColorTexture", m_texture);
	}
	else
	{
		MaterialPtr material = LJMUShaderMaker::MakeActorTextured(*renderTarget);
		material->Parameters.SetShaderResourceParameter(L"ColorTexture", m_texture);
		GetBody()->SetMaterial(material);
	}

}
