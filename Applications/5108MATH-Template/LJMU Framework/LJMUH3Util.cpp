#include "LJMUH3Util.h"

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

#include "RendererDX11.h"
#include "SpriteFontLoaderDX11.h"

using namespace LJMUDX;
using namespace Glyph3;

void LJMUResourceHelper::LoadTexture(int id, const std::wstring& filename)
{
	ResourcePtr resource; 
	resource = RendererDX11::Get()->LoadTexture(filename);
	if (resource == nullptr)
	{
		std::cout << "Error Loading " << filename.c_str();
	}

	// If loading successful, insert resource to map
	InsertResource(id, std::move(resource));
}

void LJMUResourceHelper::LoadFont(int id,std::wstring& name, int size)
{
	SpriteFontPtr resource;
	resource = SpriteFontLoaderDX11::LoadFont(name, size, 0, false);
	if (resource == nullptr)
	{
		std::cout << "Error Loading " << name.c_str();
	}

	// If loading successful, insert resource to map
	InsertResource(id, std::move(resource));
}

ResourcePtr LJMUResourceHelper::GetTexture(int id)
{
	auto found = m_resourceMap.find(id);
	assert(found != m_resourceMap.end());
	return found->second;
}

SpriteFontPtr LJMUResourceHelper::GetFont(int id)
{
	auto found = m_resourceFont.find(id);
	assert(found != m_resourceFont.end());
	return found->second;
}

void LJMUResourceHelper::InsertResource(int id,ResourcePtr resource) 
{
	// Insert and check success
	auto inserted = m_resourceMap.insert(std::make_pair(id, std::move(resource)));
	assert(inserted.second);
}

void LJMUResourceHelper::InsertResource(int id, SpriteFontPtr resource)
{
	// Insert and check success
	auto inserted = m_resourceFont.insert(std::make_pair(id, std::move(resource)));
	assert(inserted.second);
}