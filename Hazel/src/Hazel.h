#pragma once
// 供Hazel应用程序使用

// clang-format off
#include <Hazel/Core/Application.h>
#include <Hazel/Core/Layer.h>
#include <Hazel/Core/Log.h>
#include <Hazel/Core/Input.h>
#include <Hazel/Core/KeyCodes.h>
#include <Hazel/Core/MouseCodes.h>
#include <Hazel/Core/Timestep.h>
#include <Hazel/Core/Ref.h>

#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/TextureAsset.h>
#include <Hazel/Asset/ShaderAsset.h>
#include <Hazel/Asset/AssetSerializer.h>
#include <Hazel/Asset/AssetRegistry.h>
#include <Hazel/Asset/TextureSerializer.h>
#include <Hazel/Asset/AssetManager.h>

#include <Hazel/Project/Project.h>

#include <Hazel/Events/Event.h>
#include <Hazel/Events/ApplicationEvent.h>
#include <Hazel/Events/KeyEvent.h>
#include <Hazel/Events/MouseEvent.h>

#include <Hazel/ImGui/ImGuiLayer.h>

// ---渲染器-------------------
#include <Hazel/Renderer/Shader.h>
#include <Hazel/Renderer/Buffer.h>
#include <Hazel/Renderer/Texture.h>
#include <Hazel/Renderer/VertexArray.h>

#include <Hazel/Renderer/OrthographicCamera.h>

#include <Hazel/Renderer/SceneRenderer.h>
#include <Hazel/Renderer/RenderCommand.h>
// ---------------------------
#include <glm/gtc/matrix_transform.hpp>

#include <Platform/OpenGL/OpenGLShader.h>

// ---入口点-------------------
#include <Hazel/Core/EntryPoint.h>
// ---------------------------
// clang-format on