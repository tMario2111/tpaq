#include "TexturePacker.hpp"

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

int main(int argc, char* argv[])
{
    TexturePacker tp{};

    CLI::App app{};
    app.set_version_flag("--version", "tpaq v1.0.0");

    app.add_option("-f,--files", tp.files, "Texture files");
    app.add_option("-d,--directories",tp.directories, "Directories containing texture files (non-recursive by default)");
    app.add_option("-o,--output", tp.output, "Output file (.png & .json)")->required();
    app.add_option("-s,--size", tp.size, "Output texture maximum size")->required();

    app.add_flag("--remove_extensions", tp.remove_extensions, "Remove texture extensions in .json file");
    app.add_flag("--recursive", tp.recursive, "Recursive directory search");

    CLI11_PARSE(app, argc, argv);

    tp.run();

    return 0;
}
