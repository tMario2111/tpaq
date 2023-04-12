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
            std::cerr << "\"" << file << "\" is not a file, skipping it\n";
            continue;
        }

        textures.push_back(std::make_unique<TextureFile>());
        if (!textures.back()->texture.loadFromFile(file))
        {
            std::cerr << "The file with path \"" << file << "\" cannot be opened\n";
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
            std::cerr << "Texture with name \"" << textures[i]->name << "\" did not fit\n";
        }
    }

    if (!success)
    {
        std::cerr << "Increase atlas size or remove some textures and try again\n";
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

void TexturePacker::writeDataFile()
{
    std::string format_string{};
    std::ifstream format_file{ "format.mustache" };
    if (!format_file)
    {
        std::cerr << "Could not find mustache format file. Defaulting to json\n";
        std::string default_format_string_dump{ DEFAULT_MUSTACHE_FORMAT };
        format_string = default_format_string_dump;
    }
    else
    {
        std::string format_file_dump{ std::istreambuf_iterator<char>{ format_file }, std::istreambuf_iterator<char>{} };
        format_string = format_file_dump;
        format_file.close();
    }

    using namespace kainjow::mustache;
    mustache format{ format_string };
    if (!format.is_valid())
    {
        std::cerr << "Format error: " << format.error_message() << '\n';
        exit(EXIT_FAILURE);
    }

    data rects{ data::type::list };

    for (auto i = 0u; i < textures.size(); i++)
    {
        auto& texture = *textures[i];

        data rect{};
        rect["name"] = texture.name;
        rect["x"] = std::to_string(static_cast<int>(texture.sprite.getGlobalBounds().left));
        rect["y"] = std::to_string(static_cast<int>(texture.sprite.getGlobalBounds().top));
        rect["w"] = std::to_string(static_cast<int>(texture.sprite.getGlobalBounds().width));
        rect["h"] = std::to_string(static_cast<int>(texture.sprite.getGlobalBounds().height));
        rect["last"] = (i == textures.size() - 1);
        rects << rect;
    }

    std::ofstream output_file{ output + "." + extension };
    output_file << format.render({ "rects", rects });
}

void TexturePacker::run()
{
    sf::Context context{};

    loadTextures();
    pack();
    writeDataFile();
}
