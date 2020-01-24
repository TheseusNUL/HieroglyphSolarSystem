//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

//------------DX TK AND STD/STL Includes-------------------------------------
#include <sstream>
#include <iomanip>

//------------Include Hieroglyph Engine Files--------------------------------

//Include the Logging System
#include "Log.h"

//Include the Event System
#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"
#include "ScriptManager.h"

//Include the DirectX Rendering Components
#include "PipelineManagerDX11.h"
#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "MaterialGeneratorDX11.h"

#include "FirstPersonCamera.h"
#include "PointLight.h"

//Add a Using Directive to avoid typing Glyph3 for basic constructs
using namespace Glyph3;
//Include our own application Namespace
using namespace LJMUDX;

LJMULevelDemo AppInstance; 

//---------CONSTRUCTORS-------------------------------------------------------

///////////////////////////////////////
//
///////////////////////////////////////
LJMULevelDemo::LJMULevelDemo():
m_2DRenderText(nullptr),
m_renderView(nullptr),
m_objCamera(nullptr),
m_pRenderer11(nullptr),
m_pWindow(nullptr),
m_swapIndex(0),
m_targetDepth(nullptr),
m_targetRender(nullptr),
m_index(0)
{
	
}

//---------METHODS------------------------------------------------------------

//////////////////////////////////////
// Get the Window Name of this Application
//////////////////////////////////////
std::wstring LJMULevelDemo::GetName()
{
	return(std::wstring(L"5108COMP: Coursework Template"));
}

/////////////////////////////////////
// Assemble our Input Layouts for this
// Stage of the Pipeline.
/////////////////////////////////////
void LJMULevelDemo::inputAssemblyStage()
{
	static const float PI = 3.14159265f;

	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------

	//Set up sun as root
	GeometryActor* pSun = LoadPlanet(L"planets/sun.jpg", 25.0f, nullptr, L"Sun");

	//set up empty node for planets to rotate around
	m_pPlanetNode = new Node3D();
	pSun->GetNode()->AttachChild(m_pPlanetNode);
	m_pPlanetNode->Position() = Vector3f(0.0f, 0.0f, 0.0f);

	//Set up empty nodes for planet tilts
	Node3D* pMercuryTilt = CreateTiltNode(Vector3f(PI * (2.11 / 180), 0.0f, 0.0f), Vector3f(50.0f, 0.0f, 0.0f));
	Node3D* pVenusTilt = CreateTiltNode(Vector3f(PI * (177 / 180), 0.0f, 0.0f), Vector3f(-100.0f, 0.0f, -20.0f));
	Node3D* pEarthTilt = CreateTiltNode(Vector3f(PI * (23.5 / 180), 0.0f, -PI * 0.2), Vector3f(150.0f, 0.0f, 160.0f));
	Node3D* pMarsTilt = CreateTiltNode(Vector3f(PI * (25 / 180), 0.0f, 0.0f), Vector3f(200.0f, 0.0f, -650.0f));
	Node3D* pJupiterTilt = CreateTiltNode(Vector3f(PI * (3.13/ 180), 0.0f, 0.0f), Vector3f(-400.0f, 0.0f, -150.0f));
	Node3D* pSaturnTilt = CreateTiltNode(Vector3f(PI * (26.7 / 180), 0.0f, 0.0f), Vector3f(600.0f, 0.0f, 30.0f));
	Node3D* pUranusTilt = CreateTiltNode(Vector3f(PI * (98 / 180), 0.0f, 0.0f), Vector3f(-700.0f, 0.0f, 300.0f));
	Node3D* pNeptuneTilt = CreateTiltNode(Vector3f(PI * (28 / 180), 0.0f, 0.0f), Vector3f(900.0f, 0.0f, -70.0f));
	Node3D* pPlutoTilt = CreateTiltNode(Vector3f(PI * (120 / 180), 0.0f, 0.0f), Vector3f(-1000.0f, 0.0f, 900.0f));

	//Node3D* pMercuryTilt = CreateTiltNode(Vector3f(PI * (2.11 / 180), 0.0f, 0.0f), Vector3f(50.0f, 0.0f, 0.0f));
	//Node3D* pVenusTilt = CreateTiltNode(Vector3f(PI * (177 / 180), 0.0f, 0.0f), Vector3f(100.0f, 0.0f, 0.0f));
	//Node3D* pEarthTilt = CreateTiltNode(Vector3f(PI * (23.5 / 180), 0.0f, -PI * 0.2), Vector3f(150.0f, 0.0f, 0.0f));
	//Node3D* pMarsTilt = CreateTiltNode(Vector3f(PI * (25 / 180), 0.0f, 0.0f), Vector3f(200.0f, 0.0f, 0.0f));
	//Node3D* pJupiterTilt = CreateTiltNode(Vector3f(PI * (3.13 / 180), 0.0f, 0.0f), Vector3f(400.0f, 0.0f, 0.0f));
	//Node3D* pSaturnTilt = CreateTiltNode(Vector3f(PI * (26.7 / 180), 0.0f, 0.0f), Vector3f(600.0f, 0.0f, 0.0f));
	//Node3D* pUranusTilt = CreateTiltNode(Vector3f(PI * (98 / 180), 0.0f, 0.0f), Vector3f(700.0f, 0.0f, 0.0f));
	//Node3D* pNeptuneTilt = CreateTiltNode(Vector3f(PI * (28 / 180), 0.0f, 0.0f), Vector3f(900.0f, 0.0f, 0.0f));
	//Node3D* pPlutoTilt = CreateTiltNode(Vector3f(PI * (120 / 180), 0.0f, 0.0f), Vector3f(1000.0f, 0.0f, 0.0f));

	GeometryActor* pMercury = LoadPlanet(L"planets/mercury.jpg", 3.0f, pMercuryTilt, L"Mercury");
	GeometryActor* pVenus = LoadPlanet(L"planets/venus.jpg", 7.0f, pVenusTilt, L"Venus");
	GeometryActor* pEarth = LoadPlanet(L"planets/earth_daymap.jpg", 7.0f, pEarthTilt, L"Earth");
	GeometryActor* pMars = LoadPlanet(L"planets/mars.jpg", 4.0f, pMarsTilt, L"Mars");
	GeometryActor* pJupiter = LoadPlanet(L"planets/jupiter.jpg", 15.0f, pJupiterTilt, L"Jupiter");
	GeometryActor* pSaturn = LoadPlanet(L"planets/saturn.jpg", 12.0f, pSaturnTilt, L"Saturn");
	GeometryActor* pUranus = LoadPlanet(L"planets/uranus.jpg", 10.0f, pUranusTilt, L"Uranus");
	GeometryActor* pNeptune = LoadPlanet(L"planets/neptune.jpg", 10.0f, pNeptuneTilt, L"Neptune");
	GeometryActor* pPluto = LoadPlanet(L"planets/pluto.jpg", 1.0f, pPlutoTilt, L"Pluto");

	//Set up empty node for moon rotation
	m_pMoonNode = new Node3D();
	pEarth->GetNode()->AttachChild(m_pMoonNode);
	m_pPlanetNode->Position() = Vector3f(0.0f, 0.0f, 0.0f);

	GeometryActor* pMoon = LoadPlanet(L"planets/moon.jpg", 2.0f, m_pMoonNode, L"Moon");
	pMoon->GetNode()->Position() = Vector3f(-20.0f, 0.0f, 0.0f);

	//saturns rings
	ResourcePtr pRingsTex = RendererDX11::Get()->LoadTexture(L"planets/saturn_ring.png");
	GeometryActor* pRings = new GeometryActor();
	pRings->SetColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	pRings->UseTexturedMaterial(pRingsTex);
	pSaturn->GetNode()->AttachChild(pRings->GetNode());
	pRings->DrawCylinder(Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), 20.0f, 20.0f, 20U);
	pRings->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	m_planetNames.push_back(L"rings");

	m_objects.push_back(pSun);					//1
	m_objects.push_back(pMercury);				//2
	m_objects.push_back(pVenus);				//3
	m_objects.push_back(pEarth);				//4
	m_objects.push_back(pMoon);					//5
	m_objects.push_back(pMars);					//6
	m_objects.push_back(pJupiter);				//7
	m_objects.push_back(pSaturn);				//6
	m_objects.push_back(pUranus);				//9
	m_objects.push_back(pNeptune);				//10
	m_objects.push_back(pPluto);				//11
	m_objects.push_back(pRings);				//12

	m_pScene->AddActor(pSun);

	PointLight* sunlight = new PointLight;
	sunlight->SetAmbient(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	sunlight->SetDiffuse(Vector4f(0.6f, 0.6f, 0.6f, 1.0f));
	sunlight->SetSpecular(Vector4f(0.6f, 0.6f, 0.6f, 0.6f));
	sunlight->GetNode()->Position() = Vector3f(0.0f, 100.0f, 25.0f);
	m_pScene->AddLight(sunlight);
}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	inputAssemblyStage();

	m_objCamera = new FirstPersonCamera();
	m_objCamera->SetEventManager(&EvtManager);

	//Set Position
	Vector3f camerapos(0.0f, 20.0f, -500.0f);
	m_objCamera->Spatial().SetTranslation(camerapos);

	m_renderView = new ViewPerspective(*m_pRenderer11, m_targetRender, m_targetDepth);
	m_renderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	m_objCamera->SetCameraView(m_renderView);

	//Construction of 2D text object
	m_2DRenderText = new LJMUTextOverlay(*m_pRenderer11, m_targetRender, std::wstring(L"Cambria"), 15);
	m_objCamera->SetOverlayView(m_2DRenderText);
	
	float width = m_pWindow->GetWidth();
	float height = m_pWindow->GetHeight();
	m_objCamera->SetProjectionParams(0.1f, 2000.0f, width / height, static_cast<float>(GLYPH_PI) / 2.0f);
	
	m_pScene->AddCamera(m_objCamera);	

	//Skybox
	ResourcePtr skyboxTexture = m_pRenderer11->LoadTexture(L"space_cubemap.dds");
	m_pSkyBox = new SkyboxActor(skyboxTexture, 5000.0f);
	m_pScene->AddActor(m_pSkyBox);
}

///////////////////////////////////
// Update the State of our Game and 
// Output the Results to Screen (Render)
/////////////////////////////////// 
void LJMULevelDemo::Update()
{
	m_pTimer->Update();
	EvtManager.ProcessEvent(EvtFrameStartPtr( new EvtFrameStart(m_pTimer->Elapsed())));


	//----------START RENDERING--------------------------------------------------------------

	//----------3D SCENE UPDATES ------------------------------------------------------------
	
	float timeperframe = m_pTimer->Elapsed();
	float runtime = m_pTimer->Runtime();
	
	//Rotate planets around sun (around node)
	Matrix3f rotation;
	rotation.RotationY(-runtime);
	m_pPlanetNode->Rotation() = rotation;
	
	//Planets spinning on axis
	for (int i = 1; i < m_objects.size() - 1; i++)
	{
			m_objects[i]->GetNode()->Rotation() = rotation;
	}

	//Moon rotation around earth
	Matrix3f moonRotation;
	moonRotation.RotationY(runtime * 5);
	m_pMoonNode->Rotation() = moonRotation;
		

	//--------2D TEXT OVERLAY----------------------------------------------------------------
	
//	static const unsigned int SCREEN_WIDTH = 1000;
	unsigned int debugWidth = 150;

	Matrix4f textpos = Matrix4f::Identity();
	float x = 30.0f;
	float y = 20.0f;
	Vector4f white(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f red(1.0f, 0.0f, 0.0f, 1.0f);
	Vector4f yellow(1.0f, 1.0f, 0.0f, 1.0f);

	textpos.SetTranslation(Vector3f(x, y, 0.0f));
	std::wstring matrixinfo = L"Object 1 Matrix\n";
	matrixinfo.append(OutputDebugMatrix(*m_objects[0]));
	m_2DRenderText->writeText(matrixinfo, textpos, yellow);

	x = m_pWindow->GetWidth() - debugWidth - x;
	
	textpos.SetTranslation(Vector3f(x, y, 0.0f));
	m_2DRenderText->writeText(std::wstring(L"Normalised Vectors: "), textpos, red);

	for (int i = 0; i < m_objects.size() - 1; i++)
	{
		y += 15;
		textpos.SetTranslation(Vector3f(x, y, 0.0f));
		m_2DRenderText->writeText(OutputDebugInfo(m_objects[i]), textpos, red);
		m_index++;
	}

	m_index = 0;
	
	x = 30.0f;
	y = m_pWindow->GetHeight() - 20.0f;

	textpos.SetTranslation(Vector3f(x, y, 0.0f));
	m_2DRenderText->writeText(outputFPSInfo(), textpos, white);

	m_pScene->Update(m_pTimer->Elapsed());
	m_pScene->Render(m_pRenderer11);

	//--------END RENDERING-------------------------------------------------------------
	m_pRenderer11->Present(m_pWindow->GetHandle(), m_pWindow->GetSwapChain());
}

///////////////////////////////////
// Configure the DirectX 11 Programmable
// Pipeline Stages and Create the Window
// Calls 
///////////////////////////////////
bool LJMULevelDemo::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int width = 1024;
	int height = 768;

	// Set the render window parameters and initialize the window
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition(25, 25);
	m_pWindow->SetSize(width, height);
	m_pWindow->SetCaption(GetName());
	m_pWindow->Initialize(this);


	// Create the renderer and initialize it for the desired device
	// type and feature level.
	m_pRenderer11 = new RendererDX11();

	if (!m_pRenderer11->Initialize(D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0))
	{
		Log::Get().Write(L"Could not create hardware device, trying to create the reference device...");

		if (!m_pRenderer11->Initialize(D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0))
		{
			ShowWindow(m_pWindow->GetHandle(), SW_HIDE);
			MessageBox(m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device!", L"5002 MATH - Session 05", MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
			RequestTermination();
			return(false);
		}
		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep(1.0f / 10.0f);
	}

	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.
	SwapChainConfigDX11 config;
	config.SetWidth(m_pWindow->GetWidth());
	config.SetHeight(m_pWindow->GetHeight());
	config.SetOutputWindow(m_pWindow->GetHandle());
	m_swapIndex = m_pRenderer11->CreateSwapChain(&config);
	m_pWindow->SetSwapChain(m_swapIndex);
	
	//Create Colour and Depth Buffers
	m_targetRender = m_pRenderer11->GetSwapChainResource(m_swapIndex);

	Texture2dConfigDX11 depthconfig;
	depthconfig.SetDepthBuffer(width, height);
	m_targetDepth = m_pRenderer11->CreateTexture2D(&depthconfig, 0);

	// Bind the swap chain render target and the depth buffer for use in rendering.  
	m_pRenderer11->pImmPipeline->ClearRenderTargets();
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, m_targetRender->m_iResourceRTV);
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState(m_targetDepth->m_iResourceDSV);
	m_pRenderer11->pImmPipeline->ApplyRenderTargets();

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >(width);
	viewport.Height = static_cast< float >(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	int tvpindex = m_pRenderer11->CreateViewPort(viewport);
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState(1);
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState(0, tvpindex);
	return(true);
}

//////////////////////////////////
//Handle Input Events in the Application
////////////////////////////////// 
bool LJMULevelDemo::HandleEvent(EventPtr event)
{
	eEVENT e = event->GetEventType();

	if (e == SYSTEM_KEYBOARD_KEYDOWN)
	{
		EvtKeyDownPtr key_down = std::static_pointer_cast<EvtKeyDown>(event);
		unsigned int  keycode = key_down->GetCharacterCode();
	}
	else if (e == SYSTEM_KEYBOARD_KEYUP)
	{
		EvtKeyUpPtr key_up = std::static_pointer_cast<EvtKeyUp>(event);
		unsigned int keycode = key_up->GetCharacterCode();
	}

	return(Application::HandleEvent(event));
}

//////////////////////////////////
// Destroy Resources created by the engine
//////////////////////////////////
void LJMULevelDemo::ShutdownEngineComponents()
{
	if (m_pRenderer11)
	{
		m_pRenderer11->Shutdown();
		delete m_pRenderer11;
	}

	if (m_pWindow)
	{
		m_pWindow->Shutdown();
		delete m_pWindow;
	}
}

//////////////////////////////////
// Shutdown the Application
//////////////////////////////////
void LJMULevelDemo::Shutdown()
{
	//NOTHING TO DO HERE
}

//////////////////////////////////
// Take a Screenshot of the Application
//////////////////////////////////
void LJMULevelDemo::TakeScreenShot()
{
	if (m_bSaveScreenshot)
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot(0, GetName());
	}
}

Glyph3::GeometryActor* LJMUDX::LJMULevelDemo::LoadPlanet(const std::wstring& filename, float radius, Node3D* parent, const std::wstring& name)
{	
	ResourcePtr texture = RendererDX11::Get()->LoadTexture(filename);

	GeometryActor* pActor = new GeometryActor();

	//Create Primitive with local space and size
	//actor->SetColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	pActor->UseTexturedMaterial(texture);
	pActor->DrawSphere(Vector3f(0.0f, 0.0f, 0.0f), radius, 12U, 20U);
	
	if (parent != nullptr)
	{
		parent->AttachChild(pActor->GetNode());
	}

	m_planetNames.push_back(name);

	return pActor;
}

Glyph3::Node3D* LJMUDX::LJMULevelDemo::CreateTiltNode(Vector3f angle, Vector3f position)
{
	Node3D* pTiltNode = new Node3D();
	m_pPlanetNode->AttachChild(pTiltNode);
	pTiltNode->Rotation().RotationZYX(angle);
	pTiltNode->Position() = Vector3f(position);

	return pTiltNode;
}

//////////////////////////////////////
// Output our Frame Rate
//////////////////////////////////////
std::wstring LJMULevelDemo::outputFPSInfo()
{
	std::wstringstream out;
	out << L"FPS: " << m_pTimer->Framerate();
	return out.str();
}

std::wstring LJMULevelDemo::OutputDebugInfo(Actor* actor)
{
	std::wstringstream out;
	Vector3f translation = actor->GetNode()->Position();
	
	Vector3f unit = translation / translation.Magnitude();
	
	out << m_planetNames[m_index] << L" Translation " << ": [" << unit.x << " " << unit.y << " " << unit.z << "]";

	return out.str();

}

std::wstring LJMULevelDemo::OutputDebugMatrix(const GeometryActor& actor)
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

