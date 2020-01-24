//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

//------------DX TK AND STD/STL Includes-------------------------------------
#include <sstream>
#include <iomanip>
#include <cstdlib>

//------------Include Hieroglyph Engine Files--------------------------------
#include "Tween.h"

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
#include "LJMUFPSCamera.h"

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
//m_2DRenderText(nullptr),
m_renderView(nullptr),
m_objCamera(nullptr),
m_pRenderer11(nullptr),
m_pWindow(nullptr),
m_swapIndex(0),
m_targetDepth(nullptr),
m_targetRender(nullptr),
m_currentObj(0)
{
	
}

static const float PI = 3.14159265f;
static const float HALF_PI = PI / 2;

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
void LJMULevelDemo::InputAssemblyStage()
{
	float timer = m_pTimer->Runtime();

	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------

	CreatecCelestialObjects();

	CreateAllObjs();

	//----------------------OVERLAY----------------------------------------------------------------

	m_overlay.Initalise(m_objects, timer, m_pWindow, *m_pRenderer11, m_objNames);

	//-----------------------LIGHTS------------------------------------------------------------
	m_sunlight = new PointLight;
	m_sunlight->SetAmbient(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	m_sunlight->SetDiffuse(Vector4f(0.6f, 0.6f, 0.6f, 1.0f));
	m_sunlight->SetSpecular(Vector4f(0.6f, 0.6f, 0.6f, 0.6f));;
	m_pScene->AddLight(m_sunlight);
}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	//m_overlay = new OverlayWrapper;

	LoadSharedResource();
	InputAssemblyStage();
	SetupAllInitialState();

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
	float timeperframe = m_pTimer->Elapsed();
	float runtime = m_pTimer->Runtime();
	float framerate = m_pTimer->Framerate();

	m_pScene->Update(m_pTimer->Elapsed());
	//----------2D UI------------------------------------------------------------------------
	m_overlay.Update2D(framerate, m_objects, m_objNames);
	m_overlay.Update3D(m_objects, m_objCamera);

	//----------3D SCENE UPDATES ------------------------------------------------------------
	Matrix3f rotation;
	rotation.Identity();
	rotation.RotationY(-runtime * 0.5);

	std::wstringstream stream;
	std::wstring light;

	ToggleShading();
	TogglePlanetTransforms(rotation, runtime);
	ToggleShipTransform(runtime);

	//UFO 1 Transforms 
	m_parents[1]->GetNode()->Position().x = cosf(runtime * 10) * 30.0f;
	m_parents[1]->GetNode()->Position().y = cosf(runtime) * 300.0f;
	m_objects[12]->GetNode()->Rotation() = rotation;

	//UFO 2 Transforms
	if (m_parents[2]->GetNode()->Position().z >= -500.0f && !m_bUFOForward)
	{
		m_parents[2]->GetNode()->Position().z -= 1.0f;
	}	
	else if (m_parents[2]->GetNode()->Position().x >= -400.0f && !m_bUFOForward)
	{
		m_parents[2]->GetNode()->Position().x -= 1.0f;
		if (m_parents[2]->GetNode()->Position().x <= -400.0f)
			m_bUFOForward = true;
	}
	else if (m_parents[2]->GetNode()->Position().z <= 250.0f && m_bUFOForward)
	{
		m_parents[2]->GetNode()->Position().z += 1.0f;
	}
	else if (m_parents[2]->GetNode()->Position().x <= 300.0f && m_bUFOForward)
	{
		m_parents[2]->GetNode()->Position().x += 1.0f;
		if (m_parents[2]->GetNode()->Position().x >= 300.0f)
			m_bUFOForward = false;
	}
	m_objects[13]->GetNode()->Rotation() = rotation;

	AsterTransforms(runtime);
	MessengerTransform(timeperframe);

	//--------END RENDERING-------------------------------------------------------------
	m_pScene->Render(m_pRenderer11);
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
	//-------------------2D TEXT-------------------------------------------
		if (keycode == '2')
			m_currentObj = m_overlay.SetCurrentObject(m_currentObj + 1);
		if (m_overlay.GetCurrentObject() >= m_objects.size())
			m_currentObj = m_overlay.SetCurrentObject(0);

	//------------------GUI PANEL----------------------------------------
		switch (keycode)
		{
		case VK_UP: m_overlay.DecrementPanel();
			break;
		case VK_DOWN: m_overlay.IncrementPanel();
			break;
		case VK_RETURN:
		{
		 switch (m_overlay.GetActiveIndex())
		 {
		 case MenuItems::E_SHADING_MODE: m_bLShadingOn = !m_bLShadingOn;
			  break;
		 case MenuItems::E_ROT_PLANETS: m_bRotPlanets = !m_bRotPlanets;
			 break;
		 case MenuItems::E_SPIN_PLANETS: m_bSpinPlanets = !m_bSpinPlanets;
			 break;
		 case MenuItems::E_ROT_MOON: m_bRotMoon = !m_bRotMoon;
			 break;
		 case MenuItems::E_TRANS_SHIP: m_bTransShip = !m_bTransShip;
			 break;
		 case MenuItems::E_RESET_CAM:
		 {
			m_objCamera->Spatial().SetTranslation(Vector3f(0.0f, 20.0f, -500.0f));
			m_objCamera->Spatial().SetRotation(Vector3f(0.0f, 0.0f, 0.0f));
		 }

		 }
		}	
		}
	}
	else if (e == SYSTEM_KEYBOARD_KEYUP)
	{
		EvtKeyUpPtr key_up = std::static_pointer_cast<EvtKeyUp>(event);
		unsigned int keycode = key_up->GetCharacterCode();

		if (keycode == '1')
		{
			if (m_overlay.GetCurrentObject() != 0)
				m_currentObj = m_overlay.SetCurrentObject(m_currentObj - 1);
			else
				m_currentObj = m_overlay.SetCurrentObject(m_objects.size() - 1);
		}
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

void LJMULevelDemo::SetupAllInitialState()
{
	//------------------------Camera----------------------------------------------------
	m_objCamera = new FirstPersonCamera();
	m_objCamera->SetEventManager(&EvtManager);
	
	//Set Position
	Vector3f camerapos(0.0f, 20.0f, -500.0f);
	m_objCamera->Spatial().SetTranslation(camerapos);
	
	m_renderView = new ViewPerspective(*m_pRenderer11, m_targetRender, m_targetDepth);
	m_renderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	m_objCamera->SetCameraView(m_renderView);
	
	float width = m_pWindow->GetWidth();
	float height = m_pWindow->GetHeight();
	m_objCamera->SetProjectionParams(0.1f, 2000.0f, width / height, static_cast<float>(GLYPH_PI) / 2.0f);
	
	m_pScene->AddCamera(m_objCamera);

	//----------OVERLAY-----------------------------------------------------
	Camera* camera = m_overlay.SetUpCamera(*m_pRenderer11, m_targetRender);
	m_pScene->AddCamera(camera);

	//-----------GUI STATES-------------------------------------------------
	m_bLShadingOn = false;
	m_bRotPlanets = false;
	m_bSpinPlanets = false;
	m_bRotPlanets = false;
	m_bTransShip = false;

	//---------OBJ STATES-------------------------------------------------
	m_bShipForward = true;
	m_bUFOForward = false;

	m_asteroidTime = 0.0f;
	m_lerpIndex = 0;
	m_lerpStart = m_lerpPos[0];
	m_lerpEnd = m_lerpPos[1];

	m_trigFactor = 0.0f;
	m_trigStart = Vector3f(150.0f, -300.0f, 0.0f);
	m_trigEnd = Vector3f(-850.0f, 300.0f, 0.0f);
}

void LJMULevelDemo::SetupInitialStates(GeometryActor* actor, Vector3f position)
{
	Vector3f scale(1.0f, 1.0f, 1.0f);
	actor->GetBody()->Scale() = scale;
	actor->GetBody()->Position() = position;
}

void LJMULevelDemo::LoadSharedResource()
{
	//-----Setup Geometry-----------------------------------------
	m_defaultResources.DefaultTexture = m_pRenderer11->LoadTexture(L"blank.png");
	m_defaultResources.DefaultFont = SpriteFontLoaderDX11::LoadFont(std::wstring(L"Calibri"), 32, 0, false);
}

void LJMUDX::LJMULevelDemo::CreatecCelestialObjects()
 {

	//Set up Sun as root
	m_pSun = new Planet();
	m_pSun->Init(L"planets/sun.jpg", 25.0f, nullptr, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), m_pRenderer11);
	m_pSun->UseTexturedMaterial(m_pRenderer11->LoadTexture(L"planets/sun.jpg"));
	m_objects.push_back(m_pSun);
	m_planets.push_back(m_pSun);
	m_objNames.push_back(L"Sun");

	//set up empty node for planets to rotate around
	m_pPlanetNode = new Node3D();
	m_pSun->GetNode()->AttachChild(m_pPlanetNode);
	m_pPlanetNode->Position() = Vector3f(0.0f, 0.0f, 0.0f);

	//Initialise planets
	InitialiseCelestialObject(L"planets/mercury.jpg", 3.0f, m_pPlanetNode, Vector3f(PI * (2.11 / 180), 0.0f, 0.0f), Vector3f(50.0f, 0.0f, 0.0f), L"Mercury");
	InitialiseCelestialObject(L"planets/venus.jpg", 7.0f, m_pPlanetNode, Vector3f(PI * (177 / 180), 0.0f, 0.0f), Vector3f(-100.0f, 0.0f, -20.0f), L"Venus");
	InitialiseCelestialObject(L"planets/earth_daymap.jpg", 7.0f, m_pPlanetNode, Vector3f(PI * (23.5 / 180), 0.0f, -PI * 0.2), Vector3f(150.0f, 0.0f, 160.0f), L"Earth");
	InitialiseCelestialObject(L"planets/mars.jpg", 4.0f, m_pPlanetNode, Vector3f(PI * (25 / 180), 0.0f, 0.0f), Vector3f(200.0f, 0.0f, -650.0f), L"Mars");
	InitialiseCelestialObject(L"planets/jupiter.jpg", 15.0f, m_pPlanetNode, Vector3f(PI * (3.13 / 180), 0.0f, 0.0f), Vector3f(-400.0f, 0.0f, -150.0f), L"Jupiter");
	InitialiseCelestialObject(L"planets/saturn.jpg", 12.0f, m_pPlanetNode, Vector3f(PI * (26.7 / 180), 0.0f, 0.0f), Vector3f(600.0f, 0.0f, 30.0f), L"Saturn");
	InitialiseCelestialObject(L"planets/uranus.jpg", 10.0f, m_pPlanetNode, Vector3f(PI * (98 / 180), 0.0f, 0.0f), Vector3f(-700.0f, 0.0f, 300.0f), L"Uranus");
	InitialiseCelestialObject(L"planets/neptune.jpg", 10.0f, m_pPlanetNode, Vector3f(PI * (28 / 180), 0.0f, 0.0f), Vector3f(900.0f, 0.0f, -70.0f), L"Neptune");
	InitialiseCelestialObject(L"planets/pluto.jpg", 1.0f, m_pPlanetNode, Vector3f(PI * (120 / 180), 0.0f, 0.0f), Vector3f(-1000.0f, 0.0f, 900.0f), L"Pluto");

	//Set up empty node for moon rotation
	m_pMoonNode = new Node3D();
	m_planets[3]->GetNode()->AttachChild(m_pMoonNode);

	InitialiseCelestialObject(L"planets/moon.jpg", 2.0f, m_pMoonNode, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), L"Moon");
	m_planets[10]->GetNode()->Position() = Vector3f(-20.0f, 0.0f, 0.0f);

	//saturns rings
	ResourcePtr pRingsTex = RendererDX11::Get()->LoadTexture(L"planets/saturn_ring.png");
	GeometryActor* pRings = new GeometryActor();
	pRings->UseTexturedMaterial(pRingsTex);
	pRings->SetColor(Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
	pRings->DrawCylinder(Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), 20.0f, 20.0f, 20U);
	m_planets[6]->GetNode()->AttachChild(pRings->GetNode());
	pRings->GetNode()->Position() = Vector3f(0.0f, 00.0f, 0.0f);

	m_pScene->AddActor(m_pSun);
}

void LJMUDX::LJMULevelDemo::InitialiseCelestialObject(const std::wstring& filename, float radius, Node3D* parent, Vector3f tiltAngle, Vector3f tiltPos, const std::wstring& name)
{
	Planet* obj = new Planet();
	obj->Init(filename, radius, parent, tiltAngle, tiltPos, m_pRenderer11);
	m_objects.push_back(obj);
	m_objNames.push_back(name);
	m_planets.push_back(obj);
}

void LJMUDX::LJMULevelDemo::CreateAllObjs()
{
	Actor* pShipParent = CreateParentActor(Vector3f(500.0f, 100.0f, 0.0f));
	Actor* pUFOParent = CreateParentActor(Vector3f(-200.0f, 100.0f, 0.0f));
	Actor* pUFOParent2 = CreateParentActor(Vector3f(450.0f, -30.0f, -100.0f));

	CreateObj(L"SF_Fighter.obj", L"SF_Fighter/SF_Fighter-Albedo.jpg", pShipParent->GetNode()->Position(), Vector3f(1.0f, 1.0f, 1.0f), pShipParent, L"Spaceship");
	CreateObj(L"ufo.obj", L"ufo/ufo_diffuse.png", pUFOParent->GetNode()->Position(), Vector3f(0.25f, 0.25f, 0.25f), pUFOParent, L"UFO");
	CreateObj(L"ufo.obj", L"ufo/ufo_diffuse.png", Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.25f, 0.25f, 0.25f), pUFOParent2, L"UFO 2");
	CreateObj(L"messenger.obj", L"foil_gold_ramp.png", Vector3f(150.0f, 0.0f, 0.0f), Vector3f(2.0f, 2.0f, 2.0f), nullptr, L"Messenger");

	CreateRotatingAsteroids();
	CreateRepeatingAteroids();
}

void LJMUDX::LJMULevelDemo::CreateObj(std::wstring obj, std::wstring texture, Vector3f position, Vector3f scale, Actor* parent, const std::wstring& name)
{
	WorldObject* object = new WorldObject();
	object->LoadObject(obj, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), m_pRenderer11->LoadTexture(texture), m_pRenderer11);
	object->GetNode()->Position() = position;
	object->GetNode()->Scale() = scale;

	m_objects.push_back(object);
	m_objNames.push_back(name);
	m_worldObjects.push_back(object);
	
	if (parent != nullptr)
	{
		parent->GetNode()->AttachChild(object->GetNode());
		m_pScene->AddActor(parent);
	}
	else
		m_pScene->AddActor(object);
}

void LJMUDX::LJMULevelDemo::CreateAsteroid(std::wstring obj, std::wstring texture, Vector3f position, Vector3f scale, Actor* parent)
{
	WorldObject* object = new WorldObject();
	object->LoadObject(obj, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), m_pRenderer11->LoadTexture(texture), m_pRenderer11);
	object->GetNode()->Position() = position;
	object->GetNode()->Scale() = scale;

	m_asteroids.push_back(object);

	parent->GetNode()->AttachChild(object->GetNode());
}

void LJMUDX::LJMULevelDemo::CreateRotatingAsteroids()
{
	Actor* pAsterRotNode = CreateParentActor(Vector3f(-700.0f, 200.0f, -320.0f));
	Actor* pAsterParent = CreateParentActor(Vector3f(pAsterRotNode->GetNode()->Position()));
	Actor* pRAsterParent = CreateParentActor(Vector3f(pAsterRotNode->GetNode()->Position()));

	pAsterRotNode->GetNode()->AttachChild(pAsterParent->GetNode());
	pAsterRotNode->GetNode()->AttachChild(pRAsterParent->GetNode());

	for (int i = 0; i <= 6; i++)
	{
		Vector3f asterpos = Vector3f::Random();
		asterpos.Clamp();
		asterpos *= 100.0f * 3;

		if (i % 2)
			CreateAsteroid(L"Asteroid_01.obj", L"Asteroids/asteroid_1_color.png", asterpos, Vector3f(15.0f, 15.0f, 15.0f), pAsterParent);
		else
			CreateAsteroid(L"Asteroid_02.obj", L"Asteroids/asteroid_2_color.png", asterpos, Vector3f(15.0f, 15.0f, 15.0f), pRAsterParent);
	}

	m_pScene->AddActor(pAsterRotNode);

	PushBackLerpVectors(Vector3f(pAsterParent->GetNode()->Position()));
}

void LJMUDX::LJMULevelDemo::CreateRepeatingAteroids()
{
	Actor* pAsterParent = CreateParentActor(Vector3f(0.0f, 0.0f, 999.0f));

	for (int i = 0; i <= 10; i++)
	{
		if (i % 2)
			CreateAsteroid(L"Asteroid_01.obj", L"Asteroids/asteroid_1_color.png", pAsterParent->GetNode()->Position(), Vector3f(0.0f, 0.0f, 0.0f), pAsterParent);
		else
			CreateAsteroid(L"Asteroid_02.obj", L"Asteroids/asteroid_2_color.png", pAsterParent->GetNode()->Position(), Vector3f(15.0f, 15.0f, 15.0f), pAsterParent);
	}
	m_pScene->AddActor(pAsterParent);
}

Glyph3::Actor* LJMUDX::LJMULevelDemo::CreateParentActor(Vector3f position)
{
	Actor* parent = new Actor;
	parent->GetNode()->Position() = position;

	m_parents.push_back(parent);

	return parent;
}

void LJMUDX::LJMULevelDemo::PushBackLerpVectors(Vector3f start)
{
	m_lerpPos.push_back(Vector3f(start));
	m_lerpPos.push_back(Vector3f(500, -300, 450));
	m_lerpPos.push_back(Vector3f(500, 300, 450));
	m_lerpPos.push_back(Vector3f(-750, -20, -200));
	m_lerpPos.push_back(Vector3f(750, 10, 600));
	m_lerpPos.push_back(Vector3f(100, 400, -300));
}

void LJMUDX::LJMULevelDemo::AsterTransforms(float runtime)
{
	RotatingAsterTransforms(runtime);
	RepeatingAsterTransforms();
}

void LJMUDX::LJMULevelDemo::RotatingAsterTransforms(float runtime)
{
	Matrix3f asterRot;
	asterRot.Identity();
	asterRot.RotationY(-runtime);
	asterRot.RotationX(runtime);

	Matrix3f rAsterRot;
	rAsterRot.Identity();
	rAsterRot.RotationY(runtime);
	rAsterRot.RotationX(-runtime);
	//Rotation
	for (int i = 0; i <= 6; i++)
	{
		//Scale
		m_asteroids[i]->GetNode()->Scale() = Vector3f(15.0, 15.0f, 15.0f) + (sinf(runtime) * 20);

		if (i % 2)
		{
			m_asteroids[i]->GetNode()->Rotation() = asterRot;
			m_parents[4]->GetNode()->Rotation() = asterRot;
		}
		else
		{
			m_asteroids[i]->GetNode()->Rotation() = rAsterRot;
			m_parents[5]->GetNode()->Rotation() = rAsterRot;
		}

	}

	//Lerp
	if (m_asteroidTime >= 1.0f)
	{
		m_asteroidTime = 0.0f;
		m_lerpIndex++;
		m_lerpStart = m_lerpPos[m_lerpIndex % m_lerpPos.size()];
		m_lerpEnd = m_lerpPos[(m_lerpIndex + 1) % m_lerpPos.size()];
	}

	if (m_asteroidTime <= 1.0f)
	{
		m_parents[3]->GetNode()->Position() = Linear<Vector3f>(m_lerpStart, m_lerpEnd, m_asteroidTime);
		m_asteroidTime += (m_pTimer->Elapsed() * 0.2f);
	}
}

void LJMUDX::LJMULevelDemo::RepeatingAsterTransforms()
{
	float index = 0.0f;

	if (m_asteroidTime < 1.0f)
	{
		m_parents[6]->GetNode()->Position() = Linear<Vector3f>(Vector3f(0.0f, 100.0f, 999.0f), Vector3f(0.0f, 100.0f, -999.0f), m_asteroidTime * 2   );
		
		for (int i = 7; i < m_asteroids.size(); i++)
		{
			m_asteroids[i]->GetNode()->Scale() = Linear<Vector3f>(Vector3f(0.0f, 0.0f, 0.f), Vector3f(10.0f, 10.0f, 10.0f), m_asteroidTime);
			
			if (i % 2)
				m_asteroids[i]->GetNode()->Position() = Linear<Vector3f>(Vector3f(0.0f, 0.0f, 0.0f), Vector3f((100.0 * index), 0.0f, 0.0f), m_asteroidTime);
			else
				m_asteroids[i]->GetNode()->Position() = Linear<Vector3f>(Vector3f(0.0f, 0.0f, 0.0f), Vector3f((100.0 * -index), 0.0f, 0.0f), m_asteroidTime);

			index++;
		}
	}
}

void LJMUDX::LJMULevelDemo::MessengerTransform(float timeperframe)
{
	Vector3f temp;
	//Messenger
	if (m_trigFactor >= HALF_PI)
	{
		m_trigFactor = 0.0f;
		temp = m_trigStart;
		m_trigStart = m_trigEnd;
		m_trigEnd = temp;
	}

	if (m_trigFactor <= HALF_PI)
	{
		m_objects[14]->GetNode()->Position() = TrigonometricInterpolation<Vector3f>(m_trigStart, m_trigEnd, m_trigFactor);
		m_trigFactor += (timeperframe * 0.01);
	}

}

void LJMUDX::LJMULevelDemo::ToggleShading()
{
	if (m_bLShadingOn)
	{
		for (int i = 0; i < m_worldObjects.size(); i++)
			m_worldObjects[i]->Update(true, m_pRenderer11);

		for (int i = 0; i < m_asteroids.size(); i++)
			m_asteroids[i]->Update(true, m_pRenderer11);

		for (int i = 1; i < m_planets.size(); i++)
			m_planets[i]->Update(true, m_pRenderer11);
	}
	else
	{
		for (int i = 0; i < m_worldObjects.size(); i++)
			m_worldObjects[i]->Update(false, m_pRenderer11);

		for (int i = 0; i < m_asteroids.size(); i++)
			m_asteroids[i]->Update(false, m_pRenderer11);

		for (int i = 1; i < m_planets.size(); i++)
			m_planets[i]->Update(false, m_pRenderer11);
	}
}

void LJMUDX::LJMULevelDemo::TogglePlanetTransforms(Matrix3f rotation, float runtime)
{
	if (m_bRotPlanets)
	{
		//Rotate planets around sun (around node);
		m_pPlanetNode->Rotation() = rotation;
	}

	if (m_bSpinPlanets)
	{
		for (int i = 1; i < 9; i++)
		{
			m_objects[i]->GetNode()->Rotation() = rotation;
			i++;
		}
	}

	if (m_bRotMoon)
	{
		//Moon rotation around earth
		Matrix3f moonRotation;
		moonRotation.RotationY(runtime * 5);
		m_pMoonNode->Rotation() = moonRotation;
	}
}

void LJMUDX::LJMULevelDemo::ToggleShipTransform(float runtime)
{
	//Ship translation
	if (m_bTransShip)
	{
		m_parents[0]->GetNode()->Position().y = sinf(runtime * 2) * 50.0f;
		m_parents[0]->GetNode()->Position().z = cosf(runtime * 0.5f) * 250.0f;

		if (m_parents[0]->GetNode()->Position().z < -249.0f || m_parents[0]->GetNode()->Position().z > 250.0f)
			m_parents[0]->GetNode()->Scale().z = -m_parents[0]->GetNode()->Scale().z;
	}
}

template <typename TValue>
TValue LJMUDX::LJMULevelDemo::TrigonometricInterpolation(const TValue& start, const TValue& end, float t)
{
	return (start * (cosf(t) * cosf(t)) + (end * (sinf(t) * sinf(t))));
}
