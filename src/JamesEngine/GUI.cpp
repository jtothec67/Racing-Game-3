#include "GUI.h"

#include "Core.h"
#include "Input.h"
#include "Texture.h"
#include "Font.h"

namespace JamesEngine
{

	GUI::GUI(std::shared_ptr<Core> _core)
	{
        mCore = _core;

        Renderer::Face face;
        face.a.m_position = glm::vec3(1.0f, 0.0f, 0.0f);
        face.b.m_position = glm::vec3(0.0f, 1.0f, 0.0f);
        face.c.m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        face.a.m_texcoords = glm::vec2(1.0f, 0.0f);
        face.b.m_texcoords = glm::vec2(0.0f, 1.0f);
        face.c.m_texcoords = glm::vec2(0.0f, 0.0f);
        mRect->add(face);

        Renderer::Face face2;
        face2.a.m_position = glm::vec3(1.0f, 0.0f, 0.0f);
        face2.b.m_position = glm::vec3(1.0f, 1.0f, 0.0f);
        face2.c.m_position = glm::vec3(0.0f, 1.0f, 0.0f);
        face2.a.m_texcoords = glm::vec2(1.0f, 0.0f);
        face2.b.m_texcoords = glm::vec2(1.0f, 1.0f);
        face2.c.m_texcoords = glm::vec2(0.0f, 1.0f);
        mRect->add(face2);
	}

    // Returns 0 if mouse not over button, 1 if mouse over button, 2 if mouse over button and clicked
	int GUI::Button(glm::vec2 _position, glm::vec2 _size, std::shared_ptr<Texture> _texture)
	{
		// Adjust the position to be the center
		glm::vec2 adjustedPosition = _position - (_size * 0.5f);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(adjustedPosition.x, adjustedPosition.y, 0));
		model = glm::scale(model, glm::vec3(_size.x, _size.y, 1));
		mShader->uniform("u_Model", model);

		int width, height;
		mCore.lock()->GetWindow()->GetWindowSize(width, height);

		glm::mat4 uiProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.0f, 1.0f);
		mShader->uniform("u_Projection", uiProjection);

		mShader->uniform("u_View", glm::mat4(1.0f));

		mShader->draw(mRect.get(), _texture->mTexture.get());

		glm::ivec2 mousePos = mCore.lock()->GetInput()->GetMouse()->GetPosition();

		// x,y mouse coordinates are from top left, so we need to flip the y
		mousePos.y = height - mousePos.y;

		// Mouse over button
		if (mousePos.x > adjustedPosition.x && mousePos.x < adjustedPosition.x + _size.x &&
			mousePos.y > adjustedPosition.y && mousePos.y < adjustedPosition.y + _size.y)
		{
			// Mouse clicked button
			if (mCore.lock()->GetInput()->GetMouse()->IsButtonDown(1))
			{
				return 2;
			}
			else
			{
				return 1;
			}
		}

		return 0;
	}


	void GUI::Image(glm::vec2 _position, glm::vec2 _size, std::shared_ptr<Texture> _texture)
	{
		// Adjust the position to be the center
		glm::vec2 adjustedPosition = _position - (_size * 0.5f);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(adjustedPosition.x, adjustedPosition.y, 0));
		model = glm::scale(model, glm::vec3(_size.x, _size.y, 1));
		mShader->uniform("u_Model", model);

		int width, height;
		mCore.lock()->GetWindow()->GetWindowSize(width, height);

		glm::mat4 uiProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.0f, 1.0f);
		mShader->uniform("u_Projection", uiProjection);

		mShader->uniform("u_View", glm::mat4(1.0f));

		mShader->draw(mRect.get(), _texture->mTexture.get());
	}

    void GUI::Text(glm::vec2 _position, float _size, glm::vec3 _colour, std::string _text, std::shared_ptr<Font> _font)
	{
		// By trial and error, 140 means capital letters are _size amount of pixels tall (pretty much)
		float adjustedSize = _size / 140.f;

        int width, height;
        mCore.lock()->GetWindow()->GetWindowSize(width, height);

        glm::mat4 uiProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.0f, 1.0f);
        mFontShader->uniform("u_Projection", uiProjection);


		float currentLineWidth = 0.0f;
		float widestLineWidth = 0.0f;

		float currentLineHeight = 0.0f;
		float totalHeight = 0.0f;

		bool firstLine = true;

		int numLines = 1;

		std::string::const_iterator c;
		for (c = _text.begin(); c != _text.end(); c++)
		{
			Renderer::Character* ch = _font->mFont->GetCharacter(c);

			if (*c == '\n')
			{
				firstLine = false;
				currentLineWidth = 0.0f;
				totalHeight += currentLineHeight;
				numLines++;
			}
			else
			{
				// Top to bottom of letter
				float height = ch->Size.y * adjustedSize;

				// If multiple lines, encounter for spacing
				if (!firstLine)
					height = ch->Size.y * 1.3 * adjustedSize; // 1.3 IS NEWLINE SPACING, NEEDS TO BE CHANGED IN SHADER.CPP TOO

				// If letter goes below the line
				if (ch->Size.y - ch->Bearing.y != 0)
					// Disregard below line
					height -= (ch->Size.y - ch->Bearing.y) * adjustedSize;

				// Update height if letter is taller than current line height
				if (currentLineHeight < ch->Size.y * 1.3 * adjustedSize)
					currentLineHeight = height;

				// Add only width if at the end of line, else add advance (includes spacing)
				if (*c == '\n' || std::next(c) == _text.end())
					currentLineWidth += ch->Size.x * adjustedSize;
				else
					currentLineWidth += (ch->Advance >> 6) * adjustedSize;

				if (currentLineWidth > widestLineWidth)
					widestLineWidth = currentLineWidth;
			}
		}

		if (numLines == 1)
			totalHeight = currentLineHeight;

		float XPos = _position.x - (widestLineWidth / 2);
		float YPos = 0;

		// Only going to use scale in x
		if (numLines == 1)
		{
			YPos = _position.y - (totalHeight / 2);
		}
		else
		{
			float averageHeight = totalHeight / numLines;
			YPos = _position.y + (((numLines - 2) * 0.75) * averageHeight) + (averageHeight / 2);
			// Not too sure how this works but took me a while to figure out.
			// Still slightly off but it's close enough.
		}


        mFontShader->uniform("u_TextColour", _colour);

		mFontShader->drawText(*mTextRect, *_font->mFont.get(), _text, XPos, YPos, adjustedSize);
	}

	void GUI::BlendImage(glm::vec2 _position, glm::vec2 _size, std::shared_ptr<Texture> _texture1, std::shared_ptr<Texture> _texture2, float _blendFactor)
	{
		// Adjust the position to be the center
		glm::vec2 adjustedPosition = _position - (_size * 0.5f);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(adjustedPosition.x, adjustedPosition.y, 0));
		model = glm::scale(model, glm::vec3(_size.x, _size.y, 1));
		mBlendShader->uniform("u_Model", model);

		int width, height;
		mCore.lock()->GetWindow()->GetWindowSize(width, height);

		glm::mat4 uiProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.0f, 1.0f);
		mBlendShader->uniform("u_Projection", uiProjection);

		mBlendShader->uniform("u_View", glm::mat4(1.0f));
		mBlendShader->uniform("u_BlendFactor", _blendFactor);

		// Bind both textures to different texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture1->mTexture.get()->id());
		mBlendShader->uniform("u_Texture1", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _texture2->mTexture.get()->id());
		mBlendShader->uniform("u_Texture2", 1);

		mBlendShader->draw(mRect.get());
	}


}