#include "SkyboxTexture.h"

#include <string>
#include <vector>

namespace JamesEngine
{

	void SkyboxTexture::OnLoad()
	{
		std::string basePath = GetPath();
		std::vector<std::string> paths = {
			basePath + "/px.png",
			basePath + "/nx.png",
			basePath + "/py.png",
			basePath + "/ny.png",
			basePath + "/pz.png",
			basePath + "/nz.png"
		};

		mTexture = std::make_shared<Renderer::Texture>(paths);
	}

}