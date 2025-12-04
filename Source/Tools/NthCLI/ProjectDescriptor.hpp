/// @author: Jake Rieger
/// @created: 12/4/2025.
///

#pragma once

#include <pugixml.hpp>
#include <regex>

namespace Nth {
    class XmlMacroExpander {
        unordered_map<string, string> mMacros;

    public:
        inline void AddMacro(const string& name, const string& value) {
            mMacros[name] = value;
        }

        inline string ExpandMacros(const string& input) {
            std::string result = input;
            const std::regex macro_pattern(R"(\$\{([^}]+)\})");
            std::smatch match;

            while (std::regex_search(result, match, macro_pattern)) {
                std::string macro_name = match[1].str();

                auto it = mMacros.find(macro_name);
                if (it != mMacros.end()) {
                    result.replace(match.position(), match.length(), it->second);
                } else {
                    throw std::runtime_error("Macro not found");
                }
            }

            return result;
        }

        inline void ExpandNode(pugi::xml_node node) {
            if (node.type() == pugi::node_pcdata || node.type() == pugi::node_cdata) {
                const string expanded = ExpandMacros(node.value());
                node.set_value(expanded.c_str());
            }

            for (pugi::xml_attribute attr : node.attributes()) {
                string expanded = ExpandMacros(attr.value());
                attr.set_value(expanded.c_str());
            }

            for (pugi::xml_node child : node.children()) {
                ExpandNode(child);
            }
        }
    };

    struct ProjectDescriptor {
        string name;
        u32 engineVersion;
        string startupScene;
        string contentPath;
        string engineContentPath;

        inline static void Deserialize(const fs::path& projectFile, ProjectDescriptor& out) {
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(projectFile.string().c_str());
            if (!result) { throw std::runtime_error("Failed to load project file"); }

            const auto projectNode = doc.child("NthProject");
            if (!projectNode) { throw std::runtime_error("Failed to parse NthProject"); }

            out.name          = projectNode.attribute("name").as_string();
            out.engineVersion = projectNode.attribute("engine_version").as_int();

            XmlMacroExpander expander;
            expander.AddMacro("ProjectRoot", projectFile.parent_path().string());

            expander.ExpandNode(projectNode);

            out.startupScene      = projectNode.child("StartupScene").child_value();
            out.contentPath       = projectNode.child("ContentPath").child_value();
            out.engineContentPath = projectNode.child("EngineContentPath").child_value();
        }

        inline void Serialize(pugi::xml_node& outNode) {}
    };
}  // namespace Nth