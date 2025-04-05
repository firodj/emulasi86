#include <iostream>
#include <map>
#include <cassert>

#include "advapi32.hpp"
#include <emulasi/core/registry.hpp>
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <typeinfo>
#include <functional>
#include <array>

using namespace emulasi;

// The fixture for testing class AdvApi32Test.
class AdvApi32Test : public testing::Test, public GameEmuInterface {
protected:
	// You can remove any or all of the following functions if their bodies would
	// be empty.

	AdvApi32Test(): m_registryMgr(this) {
		// You can do set-up work for each test here.
		AddDll<AdvApi32Dll>()->Register();
		//GetDll<AdvApi32Dll>()->impl()->Dump(RegistryNode::nullId, "");
	}

	~AdvApi32Test() override {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	void SetUp() override {
		// Code here will be called immediately after the constructor (right
		// before each test).
		memories_.clear();
	}

	void TearDown() override {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	void * Memory(Address address) override
	{
		void *data = nullptr;
		fmt::print(stderr, "-- Access Memory {:#x}", address);
		if (memories_.find(address) == memories_.end()) {
			fmt::print(stderr, " not found\n");
		} else {
			data = memories_.at(address).data();
			fmt::print(stderr, " size {}\n", memories_.at(address).size());
		}
		return data;
	}

	Export& exportAddNew(const char* name, ExportCallbackT callback_t, Address address) override {
		//std::cout << "--- Register " << name << std::endl;
		Export export_handler;

		export_handler.name = name;
		export_handler.callback_t = callback_t;
		export_handlers[name] =export_handler;
		return export_handlers[name];
	}

	RegistryManager * GetRegistryManager() override {
		return &m_registryMgr;
	}

	// Class members declared here can be used by all tests in the test suite
	// for AdvApi32Test.

	std::map<std::string, Export> export_handlers;
	std::map<Address, std::vector<uint8_t>> memories_;
	RegistryManager m_registryMgr;
};

TEST_F(AdvApi32Test, RegCloseKey) {
	std::array<uint32_t, 2> stack_data;
	stack_data[0] = 0x00410000;
	stack_data[1] = 0x80000010;

	ExportStackParam * p = (ExportStackParam *)stack_data.data();
	auto ret = export_handlers["RegCloseKey"].callback_t(this, p);

	ASSERT_EQ(API__ERROR_SUCCESS, ret.retVal());
	ASSERT_EQ(true, ret.isReturn());
	ASSERT_EQ(1, ret.argNum());
}

TEST_F(AdvApi32Test, RegCreateKeyExA) {
	std::array<uint32_t, 10> stack_data;
	stack_data[0] = 0x00410000;	// returnAddress
	stack_data[1] = 0x80000001; // hKey
	stack_data[2] = 0x00600010; // lpSubKey
	stack_data[4] = 0x0;        // lpClass
	stack_data[5] = 0x00000020; // dwOptions
	stack_data[6] = 0x00000040; // samDesired
	stack_data[7] = 0x0;        // lpSecurityAttributes
	stack_data[8] = 0x00600100; // phkResult
	stack_data[9] = 0x00600110; // lpdwDisposition

	const char *sub_key_str = "Software\\CurrentVersion";
	auto &sub_key = memories_[0x00600010];
	sub_key.resize(strlen(sub_key_str) + 1);
	strcpy((char*)sub_key.data(), sub_key_str);

	auto &key_result = memories_[0x00600100];
	key_result.resize(sizeof(uint32_t));

	auto &disposition = memories_[0x00600110];
	disposition.resize(sizeof(uint32_t));

	ExportStackParam * p = (ExportStackParam *)stack_data.data();
	auto ret = export_handlers["RegCreateKeyExA"].callback_t(this, p);

	ASSERT_EQ(0, ret.retVal());
	ASSERT_EQ(true, ret.isReturn());
	ASSERT_EQ(9, ret.argNum());
}

TEST_F(AdvApi32Test, RegQueryValueExA) {
	std::array<uint32_t, 10> stack_data;
	stack_data[0] = 0x00410000;	// returnAddress
	stack_data[1] = 0x80000010; // hKey
	stack_data[2] = 0x00600010; // lpValueName
	stack_data[4] = 0x00600020; // lpType
	stack_data[5] = 0x00600030; // lpData
	stack_data[6] = 0x00600040; // lpcbData

	const char *value_name_str = "CD Path";
	auto &value_name = memories_[0x00600010];
	value_name.resize(strlen(value_name_str) + 1);
	strcpy((char*)value_name.data(), value_name_str);

	auto &type = memories_[0x00600020];
	type.resize(sizeof(uint32_t));

	auto &data = memories_[0x00600030];
	data.resize(10);

	auto &cb_data = memories_[0x00600040];
	cb_data.resize(sizeof(uint32_t));
	*(uint32_t*)cb_data.data() = 10;

	ExportStackParam * p = (ExportStackParam *)stack_data.data();
	auto ret = export_handlers["RegQueryValueExA"].callback_t(this, p);

	ASSERT_EQ(0, ret.retVal());
	ASSERT_EQ(true, ret.isReturn());
	ASSERT_EQ(6, ret.argNum());
}

TEST_F(AdvApi32Test, RegSetValueExA) {
	std::array<uint32_t, 10> stack_data;
	stack_data[0] = 0x00410000;	// returnAddress
	stack_data[1] = 0x80000010; // hKey
	stack_data[2] = 0x00600010; // lpValueName
	stack_data[4] = 0x0;        // dwType
	stack_data[5] = 0x00000020; // lpData
	stack_data[6] = 0x00000040; // lpcbData

	ExportStackParam * p = (ExportStackParam *)stack_data.data();
	auto ret = export_handlers["RegSetValueExA"].callback_t(this, p);

	ASSERT_EQ(0, ret.retVal());
	ASSERT_EQ(true, ret.isReturn());
	ASSERT_EQ(6, ret.argNum());
}

TEST_F(AdvApi32Test, RegOpenKeyExA) {
	std::array<uint32_t, 10> stack_data;
	stack_data[0] = 0x00410000;	// returnAddress
	stack_data[1] = 0x80000003; // hKey
	stack_data[2] = 0x00600010; // lpSubKey
	stack_data[3] = 0x00000010; // ulOptions
	stack_data[4] = 0x00000020; // samDesired
	stack_data[5] = 0x00600100; // phkResult

	const char *sub_key_str = "Default";
	auto &sub_key = memories_[0x00600010];
	sub_key.resize(strlen(sub_key_str) + 1);
	strcpy((char*)sub_key.data(), sub_key_str);

	ExportStackParam * p = (ExportStackParam *)stack_data.data();
	auto ret = export_handlers["RegOpenKeyExA"].callback_t(this, p);

	ASSERT_EQ(0, ret.retVal());
	ASSERT_EQ(true, ret.isReturn());
	ASSERT_EQ(5, ret.argNum());
}

TEST_F(AdvApi32Test, GetUserNameA) {
	std::array<uint32_t, 2> stack_data;
	stack_data[0] = 0x00410000;
	stack_data[1] = 0x00600010;
	stack_data[2] = 0x00600040;

	auto &name = memories_[0x00600010];
	name.resize(10);

	auto &size = memories_[0x00600040];
	size.resize(sizeof(uint32_t));
	*(uint32_t*)size.data() = 10;

	ExportStackParam * p = (ExportStackParam *)stack_data.data();
	auto ret = export_handlers["GetUserNameA"].callback_t(this, p);

	ASSERT_EQ(1, ret.retVal());
	ASSERT_EQ(true, ret.isReturn());
	ASSERT_EQ(2, ret.argNum());
	ASSERT_STREQ("DummyUser", (const char*)name.data());
}
