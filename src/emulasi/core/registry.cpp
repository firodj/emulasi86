#include "registry.hpp"
#include <array>
#include <fmt/core.h>
#include <fmt/color.h>

namespace emulasi {
////////////////////////////////////////////////////////////////////////////////

RegistryManager::RegistryManager(GameEmuInterface *gameEmu):
	m_gameEmu(gameEmu)
{
	InitSpecialKeys();
}

RegistryManager::~RegistryManager() {

}

bool RegistryManager::IsSpecialHKEY(API__HANDLE hKey, std::string *alias) {
	static std::array<const char*, 6> specials = {
		"HKEY_CLASSES_ROOT",
		"HKEY_CURRENT_USER",
		"HKEY_LOCAL_MACHINE",
		"HKEY_USERS",
		"HKEY_PERFORMANCE_DATA",
		"HKEY_CURRENT_CONFIG",
	};

	if (hKey != API__HKEY_PERFORMANCE_DATA &&
		API__HKEY_CLASSES_ROOT <= hKey && hKey <= API__HKEY_CURRENT_CONFIG) {
			if (alias) {
				*alias = specials[hKey-API__HKEY_CLASSES_ROOT];
			}
			return true;
		}
	return false;
}
std::vector<std::string> RegistryManager::SplitKeys(std::string & keyPaths) {
	std::vector<std::string> tokens;
	size_t start = 0, end;
	while ((end = keyPaths.find('\\', start)) != std::string::npos) {
		tokens.push_back(keyPaths.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(keyPaths.substr(start)); // Add last part

	if (tokens.size() > 0 && tokens[0].empty()) tokens.erase(tokens.begin());

	return tokens;
}

bool RegistryManager::StrCaseCmp(const std::string& a, const std::string& b) {
	return std::equal(a.begin(), a.end(), b.begin(), b.end(),
						[](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); });
}

RegistryNode::id_type RegistryManager::Traverse(std::string keyPaths, bool create) {
	std::vector<std::string> tokens = SplitKeys(keyPaths);

	if (tokens.size() == 0) return 0;

	RegistryNode::listOfIDs * currentKeysId = &rootKeysId;
	RegistryNode::id_type lastId = 0, parentId = 0;
	bool nextSubKey = false;
	for (std::string token: tokens) {
		nextSubKey = false;
		for (RegistryNode::id_type id: *currentKeysId) {
			if (StrCaseCmp(registryKeys[id]->name, token)) {
				lastId = id;
				nextSubKey = true;
				currentKeysId = &registryKeys[id]->subKeysId;
				parentId = id;
				break;
			}
		}
		if (!nextSubKey) {
			if (!create) break;

			++currentId;

			registryKeys[currentId] = std::make_unique<RegistryNode>();
			registryKeys[currentId]->name = token;
			registryKeys[currentId]->special = false;
			currentKeysId->insert(currentId);
			registryKeys[currentId]->parentId = parentId;

			lastId = currentId;
			nextSubKey = true;
			currentKeysId = &registryKeys[lastId]->subKeysId;
			parentId = lastId;
		}
	}

	if (nextSubKey) {
		return lastId;
	}

	return 0;
}

void RegistryManager::Dump(RegistryNode::id_type node, std::string path) {
	RegistryNode::listOfIDs *ids;
	if (node == RegistryNode::nullId) {
		ids = &rootKeysId;
	} else {
		ids = &registryKeys[node]->subKeysId;
	}
	for (auto id: *ids) {
		const auto &node = registryKeys[id];
		auto fullpath = fmt::format("{}\\{}", path, node->name);
		fmt::println("{:#10x} {}", id, fullpath);
		Dump(id, fullpath);
	}
}

std::string RegistryManager::FullPath(RegistryNode::id_type node) {
	if (node == RegistryNode::nullId) return "";
	auto key = GetNode(node);
	if (!key) {
		return "";
	}

	std::string path = fmt::format("{}\\{}", FullPath(key->parentId), key->name);
	return path;
}

const RegistryNode * RegistryManager::GetNode(API__HANDLE hKey) {
	auto node = (RegistryNode::id_type)hKey;
	if (registryKeys.find(node) == registryKeys.end()) {
		fmt::print(stderr, fmt::fg(fmt::color::orange),
			"WARNING: invalid registry key {:#x}\n", node);
		return nullptr;
	}
	return registryKeys[node].get();
}

void RegistryManager::InitSpecialKeys() {
	fmt::println("InitSpecialKeys...");

	registryKeys[API__HKEY_LOCAL_MACHINE] = std::make_unique<RegistryNode>();
	registryKeys[API__HKEY_LOCAL_MACHINE]->name = "Machine";
	registryKeys[API__HKEY_LOCAL_MACHINE]->special = true;
	rootKeysId.insert(API__HKEY_LOCAL_MACHINE);

	registryKeys[API__HKEY_USERS] = std::make_unique<RegistryNode>();
	registryKeys[API__HKEY_USERS]->name = "Users";
	registryKeys[API__HKEY_USERS]->special = true;
	rootKeysId.insert(API__HKEY_USERS);

	registryKeys[API__HKEY_DYN_DATA] = std::make_unique<RegistryNode>();
	registryKeys[API__HKEY_DYN_DATA]->name = "DynData";
	registryKeys[API__HKEY_DYN_DATA]->special = true;
	rootKeysId.insert(API__HKEY_DYN_DATA);

	registryKeys[API__HKEY_CLASSES_ROOT] = std::make_unique<RegistryNode>();
	registryKeys[API__HKEY_CLASSES_ROOT]->name = "Classes";
	registryKeys[API__HKEY_CLASSES_ROOT]->special = true;

	auto software_key = Traverse("\\Machine\\Software", true);
	registryKeys[software_key]->subKeysId.insert(API__HKEY_CLASSES_ROOT);
	registryKeys[API__HKEY_CLASSES_ROOT]->parentId = software_key;

	registryKeys[API__HKEY_CURRENT_CONFIG] = std::make_unique<RegistryNode>();
	registryKeys[API__HKEY_CURRENT_CONFIG]->name = "Current";
	registryKeys[API__HKEY_CURRENT_CONFIG]->special = true;

	auto hardware_profiles = Traverse("\\Machine\\System\\CurrentControlSet\\Hardware Profiles", true);
	registryKeys[hardware_profiles]->subKeysId.insert(API__HKEY_CURRENT_CONFIG);
	registryKeys[API__HKEY_CURRENT_CONFIG]->parentId = hardware_profiles;

	Traverse("\\Machine\\SAM", true);
	Traverse("\\Machine\\Security", true);

	registryKeys[API__HKEY_CURRENT_USER] = std::make_unique<RegistryNode>();
	registryKeys[API__HKEY_CURRENT_USER]->name = "1";
	registryKeys[API__HKEY_CURRENT_USER]->special = true;

	registryKeys[API__HKEY_USERS]->subKeysId.insert(API__HKEY_CURRENT_USER);
	registryKeys[API__HKEY_CURRENT_USER]->parentId = API__HKEY_USERS;

	Traverse("\\Users\\Default", true);

	// API__HKEY_PERFORMANCE_DATA
}
////////////////////////////////////////////////////////////////////////////////
