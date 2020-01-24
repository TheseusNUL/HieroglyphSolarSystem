#include "Planet.h"
#include "MaterialDX11.h"
#include "..\LJMU Framework\LJMUShaderMaker.h"

void LJMUDX::Planet::Init(const std::wstring& filename, float radius, Node3D* parent, Vector3f tiltAngle, Vector3f tiltPos, RendererDX11* renderer)
{
	LoadPhongTexturedMaterial(renderer);

	Node3D* planetParent = CreateTiltNode(tiltAngle, tiltPos, parent);
	LoadPlanet(filename, radius, planetParent, renderer);
}

void LJMUDX::Planet::LoadPlanet(const std::wstring& filename, float radius, Node3D* parent, RendererDX11* renderer)
{
	ResourcePtr texture = RendererDX11::Get()->LoadTexture(filename);

	MaterialPtr material = LJMUShaderMaker::MakeActorTextured(*renderer);
	material->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
	GetBody()->SetMaterial(material);

	DrawSphere(Vector3f(0.0f, 0.0f, 0.0f), radius, 12U, 20U);

	if (parent != nullptr)
		parent->AttachChild(this->GetNode());

	m_texture = texture;
}

void LJMUDX::Planet::LoadPhongTexturedMaterial(RendererDX11* renderer)
{
	m_phongTexturedMaterial = MaterialPtr(new MaterialDX11());

	RenderEffectDX11* pEffect = new RenderEffectDX11;
	pEffect->SetVertexShader(renderer->LoadShader(VERTEX_SHADER, std::wstring(L"PhongTextured.hlsl"), std::wstring(L"VSMAIN"), std::wstring(L"vs_4_0")));
	pEffect->SetPixelShader(renderer->LoadShader(PIXEL_SHADER, std::wstring(L"PhongTextured.hlsl"), std::wstring(L"PSMAIN"), std::wstring(L"ps_4_0")));

	m_phongTexturedMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_phongTexturedMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
}



void LJMUDX::Planet::Update(bool shading, RendererDX11* renderer)
{
	if (shading)
	{
		GetBody()->SetMaterial(m_phongTexturedMaterial);
		GetBody()->GetMaterial()->Parameters.SetShaderResourceParameter(L"ColorTexture", m_texture);
	}
	else
	{
		MaterialPtr material = LJMUShaderMaker::MakeActorTextured(*renderer);
		material->Parameters.SetShaderResourceParameter(L"ColorTexture", m_texture);
		GetBody()->SetMaterial(material);
	}
}

Glyph3::Node3D* LJMUDX::Planet::CreateTiltNode(Vector3f angle, Vector3f position, Node3D* parent)
{
	if (parent != nullptr)
	{
		Node3D* pTiltNode = new Node3D();
		parent->AttachChild(pTiltNode);
		pTiltNode->Rotation().RotationZYX(angle);
		pTiltNode->Position() = Vector3f(position);

		return pTiltNode;
	}

	return nullptr;
}
