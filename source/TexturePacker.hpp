#pragma once

#include "../mustache.hpp"
#include <SFML/Graphics.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class TexturePacker
{
public:
    TexturePacker() = default;

    std::vector<std::string> files;
    std::vector<std::string> directories;
    std::string output;
    unsigned int size;
    unsigned int border = 0;
    std::string extension = "json";

    bool keep_extensions = false;
    bool recursive = false;

    void run();

private:
    struct TextureFile
    {
        TextureFile() = default;
        std::string name;
        sf::Texture texture;
        sf::Sprite sprite;
    };
    std::vector<std::unique_ptr<TextureFile>> textures;

    void loadTextures();
    void pack();
    void writeDataFile();
};
