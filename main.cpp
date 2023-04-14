#include "source/TexturePacker.hpp"

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

int main(int argc, char* argv[])
{
    TexturePacker tp{};

    CLI::App app{};
    app.set_version_flag("--version", "tpaq v1.2.0");
    app.add_option("-f,--files", tp.files, "Texture files");
    app.add_option("-d,--directories", tp.directories, 
        "Directories containing texture files (non-recursive by default)");
    app.add_option("-o,--output", tp.output, "Output file (.png & .json)")->required();
    app.add_option("-s,--size", tp.size, "Output texture maximum size")->required();
    app.add_option("-b,--border", tp.border, "Border between textures (0 by default)");
    app.add_option("-e,--extension", tp.extension, "Data file extension (json by default)");
    app.add_option("--backend", tp.backend, "Rect packing backend: stb / rp2d (stb by default)")
        ->transform(
            CLI::CheckedTransformer(std::map<std::string, TexturePacker::Backend>
                {{"stb", TexturePacker::Backend::stb}, {"rp2d", TexturePacker::Backend::rp2d}},
            CLI::ignore_case
        ));

    app.add_flag("--keep_extensions", tp.keep_extensions, "Keep texture extensions in .json file");
    app.add_flag("--recursive", tp.recursive, "Recursive directory search");

    CLI11_PARSE(app, argc, argv);

    tp.run();

    return 0;
}
