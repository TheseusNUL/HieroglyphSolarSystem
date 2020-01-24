#include "LJMUTwoDOverlay.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"

using namespace LJMUDX;
using namespace Glyph3;

///////////////////////////
// Constructor for our Sprite
// Overlay Class. 
///////////////////////////
LJMUTwoDOverlay::LJMUTwoDOverlay(RendererDX11& prenderer, ResourcePtr prtt, ResourcePtr pdefsprite, SpriteFontPtr pdeffont)
{
	this->m_objRenderTarget = prtt;
	ResourceDX11* tptr_res = prenderer.GetResourceByIndex(this->m_objRenderTarget->m_iResource);

	if (tptr_res->GetType() == RT_TEXTURE2D)
	{
		Texture2dDX11* ttex = (Texture2dDX11*)tptr_res;
		D3D11_TEXTURE2D_DESC tdesc = ttex->GetActualDescription();
		D3D11_VIEWPORT tviewport;
		tviewport.Width = static_cast<float>(tdesc.Width);
		tviewport.Height = static_cast<float>(tdesc.Height);
		tviewport.MinDepth = 0.0f;
		tviewport.MaxDepth = 1.0f;
		tviewport.TopLeftX = 0;
		tviewport.TopLeftY = 0;
		this->m_objViewport = prenderer.CreateViewPort(tviewport);
	}

	//Create the Sprite Font and the Sprite Renderer (SpriteBatch) for our Sprite Elements
	this->m_objPrtDefSprite	= pdefsprite;
	this->m_objPtrDefFont		= pdeffont;

	this->m_pObjSpriteBatch = new SpriteRendererDX11();
	this->m_pObjSpriteBatch->Initialize();
}

///////////////////////////
// Destructor For our Sprite
// Overlay Class
///////////////////////////
LJMUTwoDOverlay::~LJMUTwoDOverlay()
{
	SAFE_DELETE(this->m_pObjSpriteBatch);
}

///////////////////////////
// Queue the Render Tasks
///////////////////////////
void LJMUTwoDOverlay::QueuePreTasks(RendererDX11* prenderer)
{
	prenderer->QueueTask(this);
}

///////////////////////////
// Execute our Sprite Rendering
// Task
///////////////////////////
void LJMUTwoDOverlay::ExecuteTask(PipelineManagerDX11* ppipelinemanager, IParameterManager* pparamManager)
{
	//If we have any Sprite elements to draw
	if (this->m_spriteList.size() > 0 || this->m_textList.size() > 0)
	{
		// Set the parameters for rendering this view
		ppipelinemanager->ClearRenderTargets();
		ppipelinemanager->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0,this->m_objRenderTarget->m_iResourceRTV);
		ppipelinemanager->ApplyRenderTargets();

		ppipelinemanager->RasterizerStage.DesiredState.ViewportCount.SetState(1);
		ppipelinemanager->RasterizerStage.DesiredState.Viewports.SetState(0,this->m_objViewport);
		ppipelinemanager->RasterizerStage.DesiredState.RasterizerState.SetState(0);

		// Set default states for these stages
		ppipelinemanager->OutputMergerStage.DesiredState.DepthStencilState.SetState(0);
		ppipelinemanager->OutputMergerStage.DesiredState.BlendState.SetState(0);

		//------------DRAW OUR SPRITES AND TEXT ELEMENTS----------------------------------------------------------------

		//Loop over each Sprite element and use the spritebatch to draw them
		for (auto te : m_spriteList)
		{
			ResourcePtr ttexture = te.Sprite == nullptr ? m_objPrtDefSprite : te.Sprite;
			//Use the RenderSprite function of our Sprite Batch to draw the Sprite
			m_pObjSpriteBatch->Render(ppipelinemanager, pparamManager,ttexture,te.xform, te.colour);
		}

		//Loop over each text element and use the spritebatch to draw them
		for (auto te : m_textList)
		{
			SpriteFontPtr tfont = te.font == nullptr ? m_objPtrDefFont : te.font;
			//Use the RenderText function of our Sprite Batch to draw the text
			m_pObjSpriteBatch->RenderText(ppipelinemanager, pparamManager,tfont,te.text.c_str(), te.xform, te.colour);
		}

		//--------------------------------------------------------------------------------------------------------------

		//Clear list of Sprite entries each frame
		m_spriteList.clear();
		m_textList.clear();
	}
}

void LJMUTwoDOverlay::drawSprite(Matrix4f& pxform, Vector4f& pcolour, ResourcePtr ptexture)
{
	this->m_spriteList.push_back(LJMUSpriteEntry(ptexture, pxform, pcolour));
}

void LJMUTwoDOverlay::drawText(std::wstring& ptext, Matrix4f& pxform, Vector4f& pcolour,SpriteFontPtr pfont)
{
	this->m_textList.push_back(LJMUTextEntry(pfont,ptext, pxform, pcolour));
}

void  LJMUTwoDOverlay::drawSprite(LJMUSpriteEntry pentry)
{
	this->m_spriteList.push_back(pentry);
}

void  LJMUTwoDOverlay::drawText(LJMUTextEntry pentry)
{
	this->m_textList.push_back(pentry);
}


//------------INTERFACE METHODS---------------------------------------------------

///////////////////////////
// Get the Name of this Task
///////////////////////////
std::wstring LJMUTwoDOverlay::GetName()
{
	return(L"LJMUTwoDOverlay");
}

///////////////////////////
// Resize the Viewport
///////////////////////////
void LJMUTwoDOverlay::Resize(UINT pwidth, UINT pheight)
{
	RendererDX11::Get()->ResizeViewport(this->m_objViewport, pwidth, pheight);
}

///////////////////////////
//
///////////////////////////
void LJMUTwoDOverlay::SetRenderParams(IParameterManager* pParamManager)
{
	//NO RENDER PARAMS NEEDED
}

///////////////////////////
//
///////////////////////////
void LJMUTwoDOverlay::SetUsageParams(IParameterManager* pParamManager)
{
	//NO USAGE PARAMS NEEDED
}

///////////////////////////
//
///////////////////////////
void LJMUTwoDOverlay::Update(float ptime)
{
	//NOTHING TO UPDATE
}