cmake_minimum_required(VERSION ${CMAKE_VERSION})

project(JAMESENGINE)

include_directories(
	src
	contrib/include
	contrib/cl-windows-x84_64/include
)

link_directories(
	contrib/cl-windows-x84_64/lib
)

add_library(JamesEngine
	src/JamesEngine/JamesEngine.h

	src/JamesEngine/Core.h
	src/JamesEngine/Core.cpp

	src/JamesEngine/Component.h
	src/JamesEngine/Component.cpp

	src/JamesEngine/Entity.h
	src/JamesEngine/Entity.cpp

	src/JamesEngine/Window.h
	src/JamesEngine/Window.cpp

	src/JamesEngine/TriangleRenderer.h
	src/JamesEngine/TriangleRenderer.cpp

	src/JamesEngine/ModelRenderer.h
	src/JamesEngine/ModelRenderer.cpp

	src/JamesEngine/Transform.h
	src/JamesEngine/Transform.cpp

	src/JamesEngine/Resource.h
	src/JamesEngine/Resource.cpp

	src/JamesEngine/Resources.h

	src/JamesEngine/Texture.h
	src/JamesEngine/Texture.cpp

	src/JamesEngine/Model.h

	src/JamesEngine/Shader.h

	src/JamesEngine/Font.h

	src/JamesEngine/Input.h
	src/JamesEngine/Input.cpp

	src/JamesEngine/Keyboard.h
	src/JamesEngine/Keyboard.cpp

	src/JamesEngine/Mouse.h
	src/JamesEngine/Mouse.cpp

	src/JamesEngine/Controller.h
	src/JamesEngine/Controller.cpp

	src/JamesEngine/Audio.h
	src/JamesEngine/Audio.cpp

	src/JamesEngine/AudioSource.h
	src/JamesEngine/AudioSource.cpp

	src/JamesEngine/Sound.h
	src/JamesEngine/Sound.cpp

	src/JamesEngine/Collider.h

	src/JamesEngine/BoxCollider.h
	src/JamesEngine/BoxCollider.cpp

	src/JamesEngine/SphereCollider.h
	src/JamesEngine/SphereCollider.cpp

	src/JamesEngine/ModelCollider.h
	src/JamesEngine/ModelCollider.cpp

	src/JamesEngine/RayCollider.h
	src/JamesEngine/RayCollider.cpp

	src/JamesEngine/Rigidbody.h
	src/JamesEngine/Rigidbody.cpp

	src/JamesEngine/Timer.h
	src/JamesEngine/Timer.cpp

	src/JamesEngine/Camera.h
	src/JamesEngine/Camera.cpp

	src/JamesEngine/GUI.h
	src/JamesEngine/GUI.cpp

	src/JamesEngine/SkyboxTexture.h
	src/JamesEngine/SkyboxTexture.cpp

	src/JamesEngine/Skybox.h
	src/JamesEngine/Skybox.cpp

	src/JamesEngine/MathsHelper.h
	src/JamesEngine/MathsHelper.cpp

	src/JamesEngine/LightManager.h
	src/JamesEngine/LightManager.cpp

	src/JamesEngine/Suspension.h
	src/JamesEngine/Suspension.cpp

	src/JamesEngine/Tire.h
	src/JamesEngine/Tire.cpp

	src/JamesEngine/RaycastSystem.h
	src/JamesEngine/RaycastSystem.cpp
)

target_link_libraries(JamesEngine Renderer openal32)

add_library(Renderer
	src/Renderer/Font.h
	src/Renderer/Font.cpp

	src/Renderer/Mesh.h
	src/Renderer/Mesh.cpp

	src/Renderer/RenderTexture.h
	src/Renderer/RenderTexture.cpp

	src/Renderer/Shader.h
	src/Renderer/Shader.cpp

	src/Renderer/Texture.h
	src/Renderer/Texture.cpp

	src/Renderer/Model.h
)

target_link_libraries(Renderer SDL2 OpenGL32 glew32 freetype)

add_executable(RacingGame
	src/RacingGame/main.cpp
)

target_link_libraries(RacingGame JamesEngine)