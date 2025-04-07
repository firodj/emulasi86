#pragma once

#include <map>
#include <set>
#include <string>
#include <emulasi/core/ms_windows.h>
#include "gameemu_interface.hpp"

#define API__HKEY_CLASSES_ROOT         0x80000000
#define API__HKEY_CURRENT_USER         0x80000001
#define API__HKEY_LOCAL_MACHINE        0x80000002
#define API__HKEY_USERS                0x80000003
#define API__HKEY_PERFORMANCE_DATA     0x80000004
#define API__HKEY_CURRENT_CONFIG       0x80000005
#define API__HKEY_DYN_DATA             0x80000006
#define API__HKEY_PERFORMANCE_TEXT     0x80000050
#define API__HKEY_PERFORMANCE_NLSTEXT  0x80000060

namespace emulasi
{
////////////////////////////////////////////////////////////////////////////////
struct RegistryNode {
	using id_type = uint32_t;
	using mapOfRegistryKeys = std::map<RegistryNode::id_type, std::unique_ptr<RegistryNode>>;
	using listOfIDs = std::set<RegistryNode::id_type>;

	id_type id{0};
	id_type parentId{0};
	std::string name;
	bool special{false};
	listOfIDs subKeysId;

	static const id_type nullId{0};
};

class RegistryManager {
public:
	RegistryManager(GameEmuInterface *gameEmu);
	virtual ~RegistryManager();

	RegistryNode::mapOfRegistryKeys registryKeys;
	RegistryNode::listOfIDs rootKeysId;
	RegistryNode::id_type currentId{0};

	bool IsSpecialHKEY(API__HANDLE hKey, std::string *alias);
	std::vector<std::string> SplitKeys(std::string & keyPaths);
	bool StrCaseCmp(const std::string& a, const std::string& b);
	RegistryNode::id_type Traverse(std::string keyPaths, bool create);
	const RegistryNode * GetNode(API__HANDLE hKey);

	void InitSpecialKeys();
	void Dump(RegistryNode::id_type node, std::string path);
	std::string FullPath(RegistryNode::id_type node) ;
private:
	GameEmuInterface *m_gameEmu;
};

////////////////////////////////////////////////////////////////////////////////
};
