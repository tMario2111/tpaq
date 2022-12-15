#include "../../source/TexturePacker.hpp"

#include <doctest/doctest.h>

TEST_CASE("image")
{
    {
        TexturePacker tp{};

        tp.directories.push_back("sample");
        tp.size = 2048u;
        tp.border = 1u;	
        tp.output = "current_result";

        tp.run();
    }

    sf::Image desired_result{};
    desired_result.loadFromFile("result.png");

    sf::Image current_result{};
    current_result.loadFromFile("current_result.png");

    SUBCASE("size")
    {
        CHECK(desired_result.getSize().x == current_result.getSize().x);
        CHECK(desired_result.getSize().y == current_result.getSize().y);
    }

    SUBCASE("pixels")
    {
        auto desired_image_ptr = desired_result.getPixelsPtr();
        auto current_image_ptr = current_result.getPixelsPtr();
        
        bool ok = true;
        for (auto i = 0u; i < desired_result.getSize().x * desired_result.getSize().y * 4u && ok; i++)
            ok = *(desired_image_ptr + i) == *(current_image_ptr + i);
        CHECK(ok);
    }
}