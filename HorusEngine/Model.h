#pragma once
#include "Object.h"
#include "Mesh.h"
#include "assimp/scene.h"
#include <optional>
#include <unordered_map>

namespace GFX::Shape
{
	class Model : public IObject
	{
		class Node : public Object
		{
			DirectX::XMFLOAT4X4 baseTransform;
			mutable std::shared_ptr<DirectX::XMFLOAT4X4> currentTransform = nullptr;
			mutable std::shared_ptr<DirectX::XMFLOAT4X4> currentScaling = nullptr;
			std::vector<std::unique_ptr<Node>> children;
			std::vector<std::shared_ptr<Mesh>> meshes;

		public:
			Node(const std::string & name, std::vector<std::shared_ptr<Mesh>> && nodeMeshes, const DirectX::FXMMATRIX & nodeTransform) noexcept;
			Node(const Node&) = default;
			Node & operator=(const Node&) = default;
			virtual ~Node() = default;

			constexpr DirectX::XMFLOAT4X4 * GetBaseTransform() noexcept { return &baseTransform; }
			inline void ReserveChildren(size_t capacity) noexcept { children.reserve(capacity); }
			inline void AddChild(std::unique_ptr<Node> child) noexcept(!IS_DEBUG)
			{
				assert(child);
				children.emplace_back(std::move(child));
			}

			inline void Draw(Graphics & gfx) const noexcept override { Draw(gfx, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity()); }

			void Draw(Graphics & gfx, const DirectX::FXMMATRIX & higherTransform, const DirectX::FXMMATRIX & higherScaling) const noexcept;
			void ShowTree(unsigned long long & nodeId, unsigned long long & selectedId, Node * & selectedNode) const noexcept;
		};
		class Window
		{
			Model * parent = nullptr;
			Node * selectedNode = nullptr;
			unsigned long long selectedId = 0;

		public:
			inline Window(Model * parent) noexcept : parent(parent), selectedNode(parent->root.get()) {}
			Window(const Window&) = default;
			Window & operator=(const Window&) = default;
			~Window() = default;

			void Show() noexcept;
		};

		static unsigned long long modelCount;

		std::string name = "";
		std::unique_ptr<Window> window = nullptr;
		std::unique_ptr<Node> root = nullptr;
		std::vector<std::shared_ptr<Mesh>> meshes;

		static std::shared_ptr<Mesh> ParseMesh(Graphics & gfx, const aiMesh & mesh);

		std::unique_ptr<Node> ParseNode(const aiNode & node) noexcept;

	public:
		Model(Graphics & gfx, const std::string & file, const DirectX::XMFLOAT3 & position = { 0.0f,0.0f,0.0f }, const std::string & modelName = "Model_", float scale = 1.0f);
		Model(const Model&) = delete;
		Model & operator=(const Model&) = delete;
		virtual ~Model() = default;

		inline void Draw(Graphics & gfx) const noexcept override { root->Draw(gfx); }

		inline const DirectX::XMFLOAT3 & GetAngle() const noexcept override { return root->GetAngle(); }
		inline void SetAngle(const DirectX::XMFLOAT3 & meshAngle) noexcept override { root->SetAngle(meshAngle); }

		inline float GetScale() const noexcept { return root->GetScale(); }
		inline void SetScale(float newScale) noexcept { root->SetScale(newScale); }

		inline const DirectX::XMFLOAT3 & GetPos() const noexcept override { return root->GetPos(); }
		inline void SetPos(const DirectX::XMFLOAT3 & position) noexcept override { root->SetPos(position); }

		inline const std::string & GetName() const noexcept override { return name; }
		inline void SetName(const std::string & newName) noexcept override { name = newName; }

		inline void Update(const DirectX::XMFLOAT3 & delta, const DirectX::XMFLOAT3 & deltaAngle = { 0.0f,0.0f,0.0f }) noexcept override { root->Update(delta, deltaAngle); }
		inline void ShowWindow() noexcept override { window->Show(); }
	};
}