#pragma once

#include <GL/glew.h>

namespace Renderer
{
	class RenderTexture
	{
	public:
		RenderTexture(int _width, int _height);
		~RenderTexture();

		void bind();
		void unbind();
		GLuint getTextureId();

		int getWidth() { return m_width; }
		int getHeight() { return m_height; }

		void clear();

	private:
		GLuint m_fboId = 0;
		GLuint m_texId = 0;
		GLuint m_rboId = 0;

		int m_width;
		int m_height;
	};
}