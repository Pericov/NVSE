#include "nvse/PluginAPI.h"
#include "nvse/CommandTable.h"
#include "nvse/GameAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/GameObjects.h"
#include <string>
//NoGore is unsupported in xNVSE

IDebugLog		gLog("nvse_plugin_example.log");
PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

NVSEMessagingInterface* g_messagingInterface;
NVSEInterface* g_nvseInterface;
NVSECommandTableInterface* g_cmdTable;
const CommandInfo* g_TFC;
NVSEScriptInterface* g_script;
NVSEStringVarInterface* g_stringInterface;
NVSEArrayVarInterface* g_arrayInterface; 

bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);

/****************
 * Here we include the code + definitions for our script functions,
 * which are packed in header files to avoid lengthening this file.
 * Notice that these files don't require #include statements for globals/macros like ExtractArgsEx.
 * This is because the "fn_.h" files are only used here,
 * and they are included after such globals/macros have been defined.
 ***************/
#include "fn_intro_to_script_functions.h" 
#include "fn_typed_functions.h"


// Shortcut macro to register a script command (assigning it an Opcode).
#define RegisterScriptCommand(name) 	nvse->RegisterCommand(&kCommandInfo_ ##name)

// Short version of RegisterScriptCommand.
#define REG_CMD(name) RegisterScriptCommand(name)

// Use this when the function's return type is not a number (when registering array/form/string functions).
//Credits: taken from JohnnyGuitarNVSE.
#define REG_TYPED_CMD(name, type)	nvse->RegisterTypedCommand(&kCommandInfo_##name,kRetnType_##type)


// This is a message handler for nvse events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case NVSEMessagingInterface::kMessage_LoadGame:
		_MESSAGE("Received load game message with file path %s", msg->data);
		break;
	case NVSEMessagingInterface::kMessage_SaveGame:
		_MESSAGE("Received save game message with file path %s", msg->data);
		break;
	case NVSEMessagingInterface::kMessage_PreLoadGame:
		_MESSAGE("Received pre load game message with file path %s", msg->data);
		break;
	case NVSEMessagingInterface::kMessage_PostLoadGame:
		_MESSAGE("Received post load game message", msg->data ? "Error/Unkwown" : "OK");
		break;
	case NVSEMessagingInterface::kMessage_PostLoad:
		_MESSAGE("Received post load plugins message");
		break;
	case NVSEMessagingInterface::kMessage_PostPostLoad:
		_MESSAGE("Received post post load plugins message");
		break;
	case NVSEMessagingInterface::kMessage_ExitGame:
		_MESSAGE("Received exit game message");
		break;
	case NVSEMessagingInterface::kMessage_ExitGame_Console:
		_MESSAGE("Received exit game via console qqq command message");
		break;
	case NVSEMessagingInterface::kMessage_ExitToMainMenu:
		_MESSAGE("Received exit game to main menu message");
		break;
	case NVSEMessagingInterface::kMessage_Precompile:
		_MESSAGE("Received precompile message with script");
		break;
	case NVSEMessagingInterface::kMessage_RuntimeScriptError:
		_MESSAGE("Received runtime script error message %s", msg->data);
		break;
	default:
		break;
	}
}

bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	_MESSAGE("query");

	// fill out the info structure
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "MyFirstPlugin";
	info->version = 2;

	// version checks
	if (nvse->nvseVersion < PACKED_NVSE_VERSION)
	{
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
		return false;
	}

	if (!nvse->isEditor)
	{
		if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
		{
			_ERROR("incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion, RUNTIME_VERSION_1_4_0_525);
			return false;
		}

		if (nvse->isNogore)
		{
			_ERROR("NoGore is not supported");
			return false;
		}
	}
	else
	{
		if (nvse->editorVersion < CS_VERSION_1_4_0_518)
		{
			_ERROR("incorrect editor version (got %08X need at least %08X)", nvse->editorVersion, CS_VERSION_1_4_0_518);
			return false;
		}
	}

	// version checks pass
	// any version compatibility checks should be done here
	return true;
}

bool NVSEPlugin_Load(const NVSEInterface* nvse)
{
	_MESSAGE("load");

	if (!nvse->isEditor)
	{
		g_pluginHandle = nvse->GetPluginHandle();

		// save the NVSEinterface in cas we need it later
		g_nvseInterface = (NVSEInterface*)nvse;

		// register to receive messages from NVSE
		g_messagingInterface = (NVSEMessagingInterface*)nvse->QueryInterface(kInterface_Messaging);
		g_messagingInterface->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);

		// script and function-related interfaces
		g_script = (NVSEScriptInterface*)nvse->QueryInterface(kInterface_Script);
		g_stringInterface = (NVSEStringVarInterface*)nvse->QueryInterface(kInterface_StringVar);
		g_arrayInterface = (NVSEArrayVarInterface*)nvse->QueryInterface(kInterface_ArrayVar);
		ExtractArgsEx = g_script->ExtractArgsEx;
	}

	

	/***************************************************************************
	 *
	 *	READ THIS!
	 *
	 *	Before releasing your plugin, you need to request an opcode range from
	 *	the NVSE team and set it in your first SetOpcodeBase call. If you do not
	 *	do this, your plugin will create major compatibility issues with other
	 *	plugins, and will not load in release versions of NVSE. See
	 *	nvse_readme.txt for more information.
	 *
	 *	See https://geckwiki.com/index.php?title=NVSE_Opcode_Base
	 *
	 **************************************************************************/

	// Do NOT use this value when releasing your plugin; request your own opcode range.
	UInt32 const examplePluginOpcodeBase = 0x2000;
	
	 // register commands
	nvse->SetOpcodeBase(examplePluginOpcodeBase);
	
	/*************************
	 * The hexadecimal Opcodes are written as comments to the left of their respective functions.
	 * It's important to keep track of how many Opcodes are being used up,
	 * as each plugin is given a limited range which may need to be expanded at some point.
	 *
	 * === How Opcodes Work ===
	 * Each function is associated to an Opcode,
	 * which the game uses to look-up where to find your function's code.
	 * It is CRUCIAL to never change a function's Opcode once it is released to the public.
	 * This is because when compiling a script, each function being called are represented as Opcodes.
	 * So changing a function's Opcode will invalidate previously compiled scripts,
	 * as they will fail to look up that function properly, instead probably finding some other function.
	 *
	 * Example: say we compile a script that uses ExamplePlugin_IsNPCFemale.
	 * The compiled script will check for the Opcode 0x2002 to call that function, and should work fine.
	 * If we remove /REG_CMD(ExamplePlugin_CrashScript);/, and don't register a new function to replace it,
	 * then `REG_CMD(ExamplePlugin_IsNPCFemale);` now registers with Opcode #0x2001.
	 * When we test the script now, a bug/crash is bound to happen,
	 * since the script is looking for an Opcode which is no longer bound to the expected function.
	 ************************/
	
	/*2000*/ RegisterScriptCommand(ExamplePlugin_PluginTest);
	/*2001*/ REG_CMD(ExamplePlugin_CrashScript);
	/*2002*/ REG_CMD(ExamplePlugin_IsNPCFemale);
	/*2003*/ REG_CMD(ExamplePlugin_FunctionWithAnAlias);
	/*2004*/ REG_TYPED_CMD(ExamplePlugin_ReturnForm, Form);
	/*2005*/ REG_TYPED_CMD(ExamplePlugin_ReturnString, String);	// ignore the highlighting for String class, that's not being used here.
	/*2006*/ REG_TYPED_CMD(ExamplePlugin_ReturnArray, Array);
	
	return true;
}
