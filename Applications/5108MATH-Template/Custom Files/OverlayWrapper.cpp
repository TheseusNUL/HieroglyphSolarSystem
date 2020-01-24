#include "OverlayWrapper.h"
#include <iomanip>

using namespace Glyph3;

void OverlayWrapper::Initalise(std::vector<Glyph3::GeometryActor*> &objs, float timer, Glyph3::Win32RenderWindow* window, Glyph3::RendererDX11& renderer, std::vector<std::wstring> names)
{
	//------------------------------2D OVERLAY-------------------------------------------
	LoadUIElements(renderer);
	m_menuItems.push_back(L"Shading Mode");
	m_menuItems.push_back(L"Rotate Planets around Sun");
	m_menuItems.push_back(L"Spin Planets on Axis");
	m_menuItems.push_back(L"Rotate Moon around Earth");
	m_menuItems.push_back(L"Translate ship");
	m_menuItems.push_back(L"Reset Camera");

	m_menuSystem.itemcount = m_menuItems.size();

	InputAssembly2D(objs, timer, window, names);

	//-----------------------------3D TEXT----------------------------------------------
	for (int i = 0; i < objs.size(); i++)
	{
		TextActor* text = new TextActor();
		text->SetColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

		objs[i]->GetNode()->AttachChild(text->GetNode());

		text->GetNode()->Position() = Vector3f(0.0f, 75.0f, 0.0f);
		text->SetLineJustification(LineJustification::CENTER);
		text->GetNode()->Scale() = Vector3f(10.0f, 10.0f, 10.0f);

		m_pTextElements.push_back(text);
	}

}

void OverlayWrapper::InputAssembly2D(std::vector<GeometryActor*> &objs, float timer, Win32RenderWindow* window, std::vector<std::wstring> names)
{
	Vector4f colour(1.0f, 1.0f, 1.0f, 1.0f);
	Matrix4f matrix;
	matrix.MakeIdentity();

//----------------------------------------UI PANEL---------------------------------------------------
	LoadUIPanel();
//-----------------------------------------UI TEXT----------------------------------------------------
	//Create Player Sprite
	LJMUDX::LJMUSpriteEntry sprite;

	for (int i = 0; i < objs.size(); i++)
	{
		sprite.xform = matrix;
		sprite.colour = colour;
		sprite.Sprite = m_localContent.GetTexture(i + 1);
		m_sprites.push_back(sprite);
		m_sprites[i + 2].xform = Matrix4f::ScaleMatrix(0.5f);
		m_sprites[i + 2].xform.SetTranslation(Vector3f(20.0f, 20.0f, 0.0f));
	}


	//Create FPS Counter
	LJMUDX::LJMUTextEntry text;
	text.xform = matrix;
	text.colour = colour;
	text.font = m_localContent.GetFont(0);
	text.text = OutputFPSInfo(timer);
	text.xform.SetTranslation(Vector3f(50.0f, window->GetHeight() - 50.0f, 0.0f));
	m_text.push_back(text);

	//Matrix info
	text.colour = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
	text.text = OutputDebugInfoAll(objs, names);
	text.xform.SetTranslation(Vector3f(150.0f, 20.0f, 0.0f));
	m_text.push_back(text);

	text.colour = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
	text.text = OutputMatrixText(objs, names);
	text.xform.SetTranslation(Vector3f(window->GetWidth() - 300.0f, 20.0f, 0.0f));
	m_text.push_back(text);
}

void OverlayWrapper::Update2D(float timer, std::vector<Glyph3::GeometryActor*> &objs, std::vector<std::wstring> names)
{
	//----------------------------------Update UI Elements-------------------------------------
	m_p2DRenderer->drawSprite(m_sprites[0]);
	m_p2DRenderer->drawSprite(m_sprites[1]);
	m_p2DRenderer->drawSprite(m_sprites[m_currentObj + 2]);

	for (auto it = m_text.begin(); it != m_text.end(); ++it)
		m_p2DRenderer->drawText(*it);
	//----------------------------------Update Panel Selection----------------------------------
	m_sprites[1].xform = m_text[m_menuSystem.active].xform;

	Vector4f selected = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f norm = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);

	for (int i = 0; i < m_text.size() - 3; ++i)
		m_text[i].colour = i == m_menuSystem.active ? selected : norm;

	//----------------------------------Update UI Text------------------------------------------
	std::wstringstream fps;
	fps << OutputFPSInfo(timer);
	m_text[6].text = fps.str();

	std::wstringstream vectors;
	vectors << OutputDebugInfoAll(objs, names);
	m_text[7].text = vectors.str();

	std::wstringstream matrix;
	matrix << OutputMatrixText(objs, names);
	m_text[8].text = matrix.str();

}

void OverlayWrapper::Update3D(std::vector<GeometryActor*> &objs, Camera* camera3D)
{
	int i = 0;
	for (auto it = m_pTextElements.begin(); it != m_pTextElements.end(); ++it)
	{
		TextActor* text = *(it);
		text->SetText(OutputDebugMatrix(*objs[i]));
		text->GetNode()->Rotation() = camera3D->GetNode()->Rotation();
		i++;
	}
}

void OverlayWrapper::LoadUIElements(Glyph3::RendererDX11& renderer)
{
	m_spriteText = renderer.LoadTexture(std::wstring(L"Face.png"));
	m_spriteFont = SpriteFontLoaderDX11::LoadFont(std::wstring(L"Arial"), 16, 0, false);

	m_localContent.LoadFont(0, std::wstring(L"Calibri"), 18);
	m_localContent.LoadTexture(1, std::wstring(L"2DOverlay/sun.png"));
	m_localContent.LoadTexture(2, std::wstring(L"2DOverlay/mercury.png"));
	m_localContent.LoadTexture(3, std::wstring(L"2DOverlay/venus.png"));
	m_localContent.LoadTexture(4, std::wstring(L"2DOverlay/earth.png"));
	m_localContent.LoadTexture(5, std::wstring(L"2DOverlay/mars.png"));
	m_localContent.LoadTexture(6, std::wstring(L"2DOverlay/jupiter.png"));
	m_localContent.LoadTexture(7, std::wstring(L"2DOverlay/saturn.png"));
	m_localContent.LoadTexture(8, std::wstring(L"2DOverlay/uranus.png"));
	m_localContent.LoadTexture(9, std::wstring(L"2DOverlay/neptune.png"));
	m_localContent.LoadTexture(10, std::wstring(L"2DOverlay/pluto.png"));
	m_localContent.LoadTexture(11, std::wstring(L"2DOverlay/moon.png"));
	m_localContent.LoadTexture(12, std::wstring(L"2DOverlay/spaceship_yellow.png"));
	m_localContent.LoadTexture(13, std::wstring(L"2DOverlay/ufo.png"));
	m_localContent.LoadTexture(14, std::wstring(L"2DOverlay/ufo.png"));
	m_localContent.LoadTexture(15, std::wstring(L"2DOverlay/messenger.png"));

	m_localContent.LoadTexture(20, std::wstring(L"2DOverlay/ui_panel_bg.png"));
	m_localContent.LoadTexture(21, std::wstring(L"2DOverlay/ui_highlight_green.png"));
}

void OverlayWrapper::LoadUIPanel()
{
	Matrix4f matrix;
	matrix.MakeIdentity();
	//------------------------PANEL BOARD---------------------------------------
	// Panel background
	LJMUDX::LJMUSpriteEntry uiPanel;
	uiPanel.xform = matrix;
	uiPanel.colour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	uiPanel.Sprite = m_localContent.GetTexture(20);
	//uiPanel.xform = Matrix4f::ScaleMatrix(0.80f);
	uiPanel.xform.SetTranslation(Vector3f(50.0f, 250.0f, 0.0f));
	m_sprites.push_back(uiPanel);

	//Highlighted
	uiPanel.Sprite = m_localContent.GetTexture(21);
	m_sprites.push_back(uiPanel);

	//-----------------------PANEL TEXT-----------------------------------------
	float height = 384;
	float textsize = m_menuItems.size();
	float gap = height / textsize;
	float posy = 250.0f;

	for (int i = 0; i < m_menuItems.size(); i++)
	{
		LJMUDX::LJMUTextEntry text;
		text.xform = matrix;
		text.colour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		//text.font = m_localContent.GetFont(0);
		//text.xform = Matrix4f::ScaleMatrix(0.80f);
		text.text = m_menuItems[i];
		text.xform.SetTranslation(Vector3f(50.0f, posy, 0.0f));
		//text.xform.SetTranslation(Vector3f(50.0f, 250.0f + (i * 50.0f), 0.0f));
		m_text.push_back(text);

		posy += gap;
	}
}

std::wstring OverlayWrapper::OutputFPSInfo(float timer)
{
	std::wstringstream out;
	out << L"FPS: " << timer;
	return out.str();
}

std::wstring OverlayWrapper::OutputDebugInfoAll(std::vector<GeometryActor*> objs, std::vector<std::wstring> names)
{
	std::wstringstream out;

	GeometryActor* actor = objs[m_currentObj];
	Vector3f trans = actor->GetNode()->Position();
	Vector3f scale = actor->GetNode()->Scale();
	out << names[m_currentObj] << ": ";
	out << L"Translation: [ ";
	out << std::fixed;
	out.precision(2);
	out << trans.x << " " << trans.y << " " << trans.z;
	out << "] ";
	out << L"Scale [";
	out << scale.x << " " << scale.y << " " << scale.x;
	out << "] ";
	out << L"Dist [" << trans.Magnitude() << "]";
	return out.str();
}

std::wstring OverlayWrapper::OutputDebugMatrix(const Glyph3::GeometryActor& actor)
{
	std::wstringstream out;
	Matrix4f worldMatrix = actor.GetNode()->WorldMatrix();

	out << std::fixed << std::setprecision(2);
	out << std::setw(10) << worldMatrix[Matrix4f::m11] << std::setw(10) << worldMatrix[Matrix4f::m12] << std::setw(10) << worldMatrix[Matrix4f::m13] << std::setw(10) << worldMatrix[Matrix4f::m14] << std::endl;
	out << std::setw(10) << worldMatrix[Matrix4f::m21] << std::setw(10) << worldMatrix[Matrix4f::m22] << std::setw(10) << worldMatrix[Matrix4f::m23] << std::setw(10) << worldMatrix[Matrix4f::m24] << std::endl;
	out << std::setw(10) << worldMatrix[Matrix4f::m31] << std::setw(10) << worldMatrix[Matrix4f::m32] << std::setw(10) << worldMatrix[Matrix4f::m33] << std::setw(10) << worldMatrix[Matrix4f::m34] << std::endl;
	out << std::setw(10) << worldMatrix[Matrix4f::m41] << std::setw(10) << worldMatrix[Matrix4f::m42] << std::setw(10) << worldMatrix[Matrix4f::m43] << std::setw(10) << worldMatrix[Matrix4f::m44] << std::endl;
	return out.str();
}

std::wstring OverlayWrapper::OutputDebugTransform(const Glyph3::Matrix4f& matrix, Glyph3::Vector3f point)
{
	std::wstringstream out;
	Vector4f vector;

	/* Calculate Transformed Homeogenous Vector using Matrix Multiplication (Vector is
	a Row vector so column Matrix multiplication is used.*/

	vector.x = (point.x * matrix[0]) + (point.y * matrix[4]) + (point.z * matrix[8]) + (1 * matrix[12]);
	vector.y = (point.x * matrix[1]) + (point.y * matrix[5]) + (point.z * matrix[9]) + (1 * matrix[13]);
	vector.z = (point.x * matrix[2]) + (point.y * matrix[6]) + (point.z * matrix[10]) + (1 * matrix[14]);
	vector.w = (point.x * matrix[3]) + (point.y * matrix[7]) + (point.z * matrix[11]) + (1 * matrix[15]);

	//Output the transformed vector
	out << std::fixed << std::setprecision(2);
	out << L"[";
	out << vector.x << " " << vector.y << " " << vector.z;
	out << "]";

	//Return the formatted string
	return out.str();
}

std::wstring OverlayWrapper::OutputDebugTransform(const Glyph3::GeometryActor& actor, Glyph3::Vector3f point)
{
	return OutputDebugTransform(actor.GetNode()->WorldMatrix(), point);
}

std::wstring OverlayWrapper::OutputMatrixText(std::vector<GeometryActor*> objs, std::vector<std::wstring> names)
{
	std::wstringstream currobjout;
	currobjout << names[m_currentObj] << " Matrix: \n";
	std::wstring matrixInfo = currobjout.str();
	matrixInfo.append(OutputDebugMatrix(*objs[m_currentObj]));
	matrixInfo.append(L"\n");
	matrixInfo.append(OutputDebugTransform(*objs[m_currentObj], Vector3f(0.0f, 0.0f, 0.0f)));

	return matrixInfo;
}
int OverlayWrapper::SetCurrentObject(int number)
{
	if (number != 0)
		m_currentObj = number;
	else
		m_currentObj = 0;

	return m_currentObj;
}

int OverlayWrapper::GetCurrentObject()
{
	return m_currentObj;
}

Camera* OverlayWrapper::SetUpCamera(Glyph3::RendererDX11& renderer, Glyph3::ResourcePtr targetRender)
{
	//----------------------Sprite Rendering------------------------------------------
	//Construct a sprite camera
	m_p2DCamera = new Camera();
	//Construct a sprite overlay
	m_p2DRenderer = new LJMUDX::LJMUTwoDOverlay(renderer, targetRender, m_spriteText, m_spriteFont);
	//Associate the overlay with the camera
	m_p2DCamera->SetOverlayView(m_p2DRenderer);

	return m_p2DCamera;
}

Glyph3::Camera* OverlayWrapper::GetCamera()
{
	return m_p2DCamera;
}

LJMUDX::LJMUTwoDOverlay* OverlayWrapper::GetOverlay()
{
	return m_p2DRenderer;
}

void OverlayWrapper::IncrementPanel()
{
	m_menuSystem.increment();
}

void OverlayWrapper::DecrementPanel()
{
	m_menuSystem.decrement();
}

int OverlayWrapper::GetActiveIndex()
{
	return m_menuSystem.active;
}
