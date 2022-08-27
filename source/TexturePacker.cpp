#include "TexturePacker.hpp"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

void TexturePacker::loadTextures()
{
	const auto processEntry = [&](const std::filesystem::directory_entry& entry)
	{
		if (!std::filesystem::is_regular_file(entry))
			return;

		const auto extension = entry.path().extension().string();
		if (extension == ".png" || extension == ".jpg")
			files.push_back(entry.path().string());
	};

	for (const auto& dir : directories)
	{
		if (recursive)
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
				processEntry(entry);
		else 
			for (const auto& entry : std::filesystem::directory_iterator(dir))
				processEntry(entry);
	}

	for (const auto& file : files)
	{
		if (!std::filesystem::is_regular_file(file))
		{
			std::cout << "\"" << file << "\" is not a file, skipping it\n";
			continue;
		}

		textures.push_back(std::make_unique<TextureFile>());
		if (!textures.back()->texture.loadFromFile(file))
		{
			std::cout << "The file with path \"" << file << "\" cannot be opened\n";
			exit(EXIT_FAILURE);
		}

		auto name = file.substr(file.find_last_of("/\\") + 1);
		if (!keep_extensions)
			name = name.substr(0, name.find_last_of('.'));
		textures.back()->name = name;

		textures.back()->sprite.setTexture(textures.back()->texture);
	}
}

void TexturePacker::pack()
{
	stbrp_context context{};
	std::vector<stbrp_rect> rects{};
	for (auto i = 0u; i < textures.size(); i++)
	{
		rects.emplace_back(stbrp_rect{});
		rects[i].x = 0;
		rects[i].y = 0;
		rects[i].w = textures[i]->texture.getSize().x + border;
		rects[i].h = textures[i]->texture.getSize().y + border;
		rects[i].id = static_cast<int>(i);
		rects[i].was_packed = 0;
	}

	std::vector<stbrp_node> nodes{};
	nodes.resize(size * 2);
	stbrp_init_target(&context, static_cast<int>(size), static_cast<int>(size),
		nodes.data(), static_cast<int>(size) * 2);
	stbrp_pack_rects(&context, rects.data(), textures.size());

	sf::Vector2i size{};
	for (auto i = 0u; i < textures.size(); i++)
	{
		if (rects[i].x + rects[i].w > size.x)
			size.x = rects[i].x + rects[i].w;
		if (rects[i].y + rects[i].h > size.y)
			size.y = rects[i].y + rects[i].h;
	}

	bool success = true;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		if (rects[i].was_packed == 1)
			textures[i]->sprite.setPosition(static_cast<float>(rects[i].x), static_cast<float>(rects[i].y));
		else
		{
			success = false;
			std::cout << "Texture with name \"" << textures[i]->name << "\" did not fit\n";
		}
	}

	if (!success)
	{
		std::cout << "Increase atlas size or remove some textures and try again\n";
		exit(EXIT_FAILURE);
	}

	sf::RenderTexture texture{};
	texture.create(size.x, size.y);

	texture.clear(sf::Color{ 0, 0, 0, 0 });
	for (const auto& i : textures)
		texture.draw(i->sprite);
	texture.display();
	texture.getTexture().copyToImage().saveToFile(output + ".png");
}

void TexturePacker::writeJson()
{
	nlohmann::json json{};
	for (auto& i : textures)
	{
		auto& root = json["frames"][i->name]["frame"];
		root["x"] = static_cast<int>(i->sprite.getGlobalBounds().left);
		root["y"] = static_cast<int>(i->sprite.getGlobalBounds().top);
		root["w"] = static_cast<int>(i->sprite.getGlobalBounds().width);
		root["h"] = static_cast<int>(i->sprite.getGlobalBounds().height);
	}
	std::ofstream file{ output + ".json" };
	file << std::setw(2) << json;
	file.close();
}

void TexturePacker::run()
{
	sf::Context context{};

	loadTextures();
	pack();
	writeJson();
}
