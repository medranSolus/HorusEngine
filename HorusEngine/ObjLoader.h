#pragma once
#include "IndexedTriangleList.h"
#include "BasicTypes.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace FileService
{
	class ObjLoader
	{
	public:
		ObjLoader() = delete;
		ObjLoader(ObjLoader &) = delete;
		ObjLoader & operator=(ObjLoader &) = delete;
		~ObjLoader() = delete;

		static GFX::Primitive::IndexedTriangleList<GFX::Primitive::Vertex> LoadMesh(const std::string & filename);
		static GFX::Primitive::IndexedTriangleList<GFX::Primitive::VertexTexture> LoadTextured(const std::string & filename);
	};
}
