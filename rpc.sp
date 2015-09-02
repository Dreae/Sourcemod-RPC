#pragma semicolon 1

#include <sourcemod>
#include <rpc>

ConVar CVar_RCP_Enabled;
ConVar CVar_RPC_Interface;
ConVar CVar_RCP_Port;
ConVar CVar_RPC_SigningKey;

#include "rpc/types"
#include "rpc/crypto"
#include "rpc/jsonrpc"
#include "rpc/socket"
#include "rpc/methods"
#include "rpc/natives"

#pragma newdecls optional

public Plugin myinfo = {
	name = "RPC",
	author = "Dreae",
	description = "Library for making RPCs",
	version = "0.0.1",
	url = ""
};

public void OnPluginStart() {
  CVar_RCP_Enabled = CreateConVar("sm_rpc_enabled", "true", "Should this server accept RCPs", FCVAR_PLUGIN | FCVAR_PROTECTED);
  CVar_RPC_Interface = CreateConVar("sm_rpc_interface", "0.0.0.0", "Interface to listen for RPCs on", FCVAR_PLUGIN | FCVAR_PROTECTED);
  CVar_RCP_Port = CreateConVar("sm_rpc_port", "30001", "Port to listen for RPCs on", FCVAR_PLUGIN | FCVAR_PROTECTED);
  CVar_RPC_SigningKey = CreateConVar("sm_rpc_signing_key", "secretkey", "Used to sign and verify messages", FCVAR_PLUGIN | FCVAR_PROTECTED);
  AutoExecConfig(true, "sm_rpc");

  CVar_RCP_Enabled.AddChangeHook(EnabledCVarChanged);

  CVar_RPC_Interface.AddChangeHook(ConfigCVarChanged);
  CVar_RCP_Port.AddChangeHook(ConfigCVarChanged);

  Sock_OpenRPCSocket();
}

public void EnabledCVarChanged(ConVar cvar, const char[] oldValue, const char[] newValue) {
  if(strcmp(oldValue, newValue) != 0) {
    if(!cvar.BoolValue) {
      Sock_CloseRPCSocket();
    } else {
      Sock_OpenRPCSocket();
    }
  }
}

public void ConfigCVarChanged(ConVar cvar, const char[] oldValue, const char[] newValue) {
  Sock_CloseRPCSocket();
  Sock_OpenRPCSocket();
}
