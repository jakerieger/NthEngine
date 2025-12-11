#include <CLI/CLI.hpp>
#include <Engine/Asset.hpp>
#include <openssl/sha.h>
#include <pugixml.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <vector>

using std::filesystem::path;

struct ProjectSubcommand {
    inline static std::string arg;

    static void Create() {
        printf("ProjectSubcommand::Create()\n");
    }

    static void Info() {
        printf("ProjectSubcommand::Info()\n");
    }

    static void Run() {
        printf("ProjectSubcommand::Run()\n");
    }
};

struct AssetSubcommand {
    inline static std::string arg;

    static void GenerateAssetDescriptorForFile(const path& filename) {
        using namespace Astera;

        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            fprintf(stderr, "Failed to open file %s\n", filename.string().c_str());
            return;
        }

        SHA256_CTX sha256;
        SHA256_Init(&sha256);

        constexpr size_t bufferSize = 8192;
        std::vector<char> buffer(bufferSize);

        while (file.read(buffer.data(), bufferSize) || file.gcount() > 0) {
            SHA256_Update(&sha256, buffer.data(), file.gcount());
        }

        std::vector<u8> hash(SHA256_DIGEST_LENGTH);
        SHA256_Final(hash.data(), &sha256);

        // Take the first 8 bytes and convert to u65
        u64 id = 0;
        for (int i = 0; i < 8 && i < hash.size(); ++i) {
            id = (id << 8) | hash[i];
        }

        // Zero out the least significant 8 bits
        id = id & 0xFFFFFFFFFFFFFF00ULL;

        std::vector<string> audioExtensions  = {".wav", ".ogg"};
        std::vector<string> imageExtensions  = {".png", ".jpg", ".jpeg", ".bmp"};
        std::vector<string> scriptExtensions = {".lua"};
        std::vector<string> sceneExtensions  = {".scene", ".xml"};
        std::vector<string> textExtensions   = {".txt"};
        std::vector<string> shaderExtensions = {".glsl", ".frag", ".vert", ".comp", ".fs", ".vs", ".cs"};
        auto type                            = AssetType::BinaryData;
        const auto fileExt                   = filename.extension().string();
        if (std::ranges::find(audioExtensions, fileExt) != audioExtensions.end()) {
            type = AssetType::Audio;
        } else if (std::ranges::find(imageExtensions, fileExt) != imageExtensions.end()) {
            type = AssetType::Sprite;
        } else if (std::ranges::find(scriptExtensions, fileExt) != scriptExtensions.end()) {
            type = AssetType::Script;
        } else if (std::ranges::find(sceneExtensions, fileExt) != sceneExtensions.end()) {
            type = AssetType::Scene;
        } else if (std::ranges::find(textExtensions, fileExt) != textExtensions.end()) {
            type = AssetType::TextData;
        } else if (fileExt == "spritesheet") {
            type = AssetType::SpriteSheet;
        } else if (std::ranges::find(shaderExtensions, fileExt) != shaderExtensions.end()) {
            type = AssetType::Shader;
        }

        id |= (u64)type;  // combine type and ID

        pugi::xml_document doc;
        auto root = doc.append_child("Asset").append_attribute("id").set_value(id);

        path outPath = filename.parent_path() / (filename.filename().string() + ".asset");
        if (!doc.save_file(outPath.string().c_str())) {
            fprintf(stderr, "Failed to save asset descriptor to %s\n", outPath.string().c_str());
        }

        printf("Saved asset descriptor: %s\n", outPath.string().c_str());
    }

    static void GenerateAssetDescriptor() {
        if (!exists(path(arg))) { throw std::runtime_error("File does not exist"); }

        GenerateAssetDescriptorForFile(path(arg));
    }

    static void GenerateAssetDescriptorsForDirectory() {
        if (!exists(path(arg))) { throw std::runtime_error("Directory does not exist"); }

        const auto it = std::filesystem::recursive_directory_iterator(path(arg));
        for (const auto& entry : it) {
            if (entry.is_regular_file()) {
                if (entry.path().extension() != ".asset") { GenerateAssetDescriptorForFile(entry.path()); }
            }
        }
    }
};

int main(int argc, char* argv[]) {
    CLI::App app {"AsteraCLI - Project management tool"};
    app.require_subcommand(1);

    // Project subcommand
    {
        CLI::App* project = app.add_subcommand("project", "Manage projects");
        project->require_subcommand(1);

        CLI::App* create = project->add_subcommand("create", "Creates a new project in the current directory");
        create->add_option("name", ProjectSubcommand::arg, "Project name")->required();
        create->callback([&]() { ProjectSubcommand::Create(); });

        CLI::App* info = project->add_subcommand("info", "Prints info on the given project");
        info->add_option("path", ProjectSubcommand::arg, "Project path")->required();
        info->callback([&]() { ProjectSubcommand::Info(); });

        CLI::App* run = project->add_subcommand("run", "Runs the project");
        run->add_option("path", ProjectSubcommand::arg, "Project path")->required();
        run->callback([&]() { ProjectSubcommand::Run(); });
    }

    // Asset subcommand
    {
        CLI::App* asset = app.add_subcommand("asset", "Manage assets");
        asset->require_subcommand(1);

        // Generate a single descriptor for an asset file
        CLI::App* generateAssetDescriptor =
          asset->add_subcommand("generate", "Generates an asset descriptor for the given asset file");
        generateAssetDescriptor->add_option("filename", AssetSubcommand::arg, "File to generate descriptor for")
          ->required();
        generateAssetDescriptor->callback([&]() { AssetSubcommand::GenerateAssetDescriptor(); });

        // Generate descriptors for all assets in a directory
        CLI::App* generateAssetDescriptors =
          asset->add_subcommand("generate-all", "Generates asset descriptors for all assets in the given directory");
        generateAssetDescriptors->add_option("directory", AssetSubcommand::arg, "Directory to generate descriptors for")
          ->required();
        generateAssetDescriptors->callback([&]() { AssetSubcommand::GenerateAssetDescriptorsForDirectory(); });
    }

    CLI11_PARSE(app, argc, argv);
}