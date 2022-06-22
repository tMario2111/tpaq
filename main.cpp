#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Image
{
	Image() = default;
	std::string name;
	sf::Texture texture;
	sf::Sprite sprite;
};

void loadImages(
        std::vector<std::unique_ptr<Image>>& images, 
        std::vector<std::string>& files,
        std::vector<std::string>& directories,
        bool remove_extensions,
        bool recursive)
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

        images.push_back(std::make_unique<Image>());
        if (!images.back()->texture.loadFromFile(file))
        {
            std::cout << "The file with path \"" << file << "\" cannot be opened\n";
            exit(-1);
        }

        auto name = file;
        if (remove_extensions)
            name = name.substr(0, name.find_last_of('.'));
        images.back()->name = name;

        images.back()->sprite.setTexture(images.back()->texture);
    }
}

void pack(std::vector<std::unique_ptr<Image>>& images, unsigned int atlas_size, const std::string& output)
{
	stbrp_context context;
	std::vector<stbrp_rect> rects;
	for (unsigned int i = 0; i < images.size(); i++)
	{
		rects.emplace_back(stbrp_rect{});
		rects[i].x = 0;
		rects[i].y = 0;
		rects[i].w = images[i]->texture.getSize().x;
		rects[i].h = images[i]->texture.getSize().y;
		rects[i].id = static_cast<int>(i);
		rects[i].was_packed = 0;
	}

	std::vector<stbrp_node> nodes;
	nodes.resize(atlas_size * 2);
	stbrp_init_target(&context, static_cast<int>(atlas_size), static_cast<int>(atlas_size),
            nodes.data(), static_cast<int>(atlas_size) * 2);
	stbrp_pack_rects(&context, rects.data(), images.size());

    sf::Vector2i size{};
	for (unsigned int i = 0; i < images.size(); i++)
	{
		if (rects[i].x + rects[i].w > size.x)
			size.x = rects[i].x + rects[i].w;
		if (rects[i].y + rects[i].h > size.y)
			size.y = rects[i].y + rects[i].h;
	}

	bool success = true;
	for (unsigned int i = 0; i < images.size(); i++)
	{
		if (rects[i].was_packed == 1)
			images[i]->sprite.setPosition(static_cast<float>(rects[i].x), static_cast<float>(rects[i].y));
		else
		{
			success = false;
			std::cout << "Image with name \"" << images[i]->name << "\" did not fit\n";
		}
	}

	if (!success)
	{
		std::cout << "Increase atlas size or remove some textures and try again\n";
		exit(-1);
	}

	sf::RenderTexture texture{};
	texture.create(size.x, size.y);

	texture.clear(sf::Color{ 0, 0, 0, 0 });
	for (auto& image : images)
		texture.draw(image->sprite);
	texture.display();
	texture.getTexture().copyToImage().saveToFile(output + ".png");
}

void writeJson(std::vector<std::unique_ptr<Image>>& images, const std::string& output)
{
	nlohmann::json json{};
	for (auto& i : images)
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

int main(int argc, char *argv[])
{
    CLI::App app{ "What" };
    app.set_version_flag("--version", "tpaq v1.0.0");

    std::vector<std::string> files;
    app.add_option("-f,--files", files, "Image files");

    std::vector<std::string> directories;
    app.add_option("-d,--directories", directories, "Directories containing image files (non-recursive by default)");

    std::string output;
    app.add_option("-o,--output", output, "Output file")->required();

    unsigned int size{};
    app.add_option("-s,--size", size, "Output image maximum size")->required();

    bool remove_extensions = true;
    app.add_flag("--remove_extensions", remove_extensions, "Remove file extensions in json");

    bool recursive = false;
    app.add_flag("--recursive", recursive, "Recursive directory search");

    CLI11_PARSE(app, argc, argv);

    sf::Context context{};
    std::vector<std::unique_ptr<Image>> images{};

    loadImages(images, files, directories, remove_extensions, recursive);
    pack(images, size, output);
    writeJson(images, output);

    return 0;
}
