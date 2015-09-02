#include <sourcemod>

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
  CreateNative("RegisterRPC", Native_RegisterRPC);
  CreateNative("CallRemoteProcedure", Native_CallRemoteProcedure);

  CreateNative("CreateArgumentPack", Native_CreateArgumentPack);
  CreateNative("ArgPackPushInt", Native_ArgPackPushInt);
  CreateNative("ArgPackPushFloat", Native_ArgPackPushFloat);
  CreateNative("ArgPackPushString", Native_ArgPackPushString);
  CreateNative("ArgPackPushBool", Native_ArgPackPushBool);
}

public int Native_RegisterRPC(Handle plugin, int numParams) {
  char fName[129];
  GetNativeString(1, fName, sizeof(fName));
  RegisterNewMethod(plugin, fName);
}

public int Native_CallRemoteProcedure(Handle plugin, int numParams) {
  char server[129];
  GetNativeString(1, server, sizeof(server));
  char fName[129];
  GetNativeString(2, fName, sizeof(fName));
  PrintToServer("Calling method %s on server %s", fName, server);
  RPCArguments argPack = view_as<RPCArguments>(GetNativeCell(3));
  if(argPack != INVALID_HANDLE) {
    argPack = view_as<RPCArguments>(CloneHandle(argPack));
  }

  char argString[2048];
  argPack.dump(argString, sizeof(argString));
  argPack.Close();

  PrintToServer(argString);
}

public int Native_CreateArgumentPack(Handle plugin, int numParams) {
  JsonArray array = new JsonArray();

  int res = view_as<int>(CloneHandle(view_as<Handle>(array), plugin));
  array.Close();
  return res;
}

public int Native_ArgPackPushInt(Handle plugin, int numParams) {
  RPCArguments argPack = view_as<RPCArguments>(GetNativeCell(1));
  int value = GetNativeCell(2);
  if(argPack != INVALID_HANDLE) {
    argPack = view_as<RPCArguments>(CloneHandle(argPack));
  }
  argPack.appendInt(value);
  argPack.Close();
}

public int Native_ArgPackPushFloat(Handle plugin, int numParams) {
  RPCArguments argPack = view_as<RPCArguments>(GetNativeCell(1));
  if(argPack != INVALID_HANDLE) {
    argPack = view_as<RPCArguments>(CloneHandle(argPack));
  }
  float value = view_as<float>(GetNativeCell(2));
  argPack.appendFloat(value);
  argPack.Close();
}

public int Native_ArgPackPushString(Handle plugin, int numParams) {
  RPCArguments argPack = view_as<RPCArguments>(GetNativeCell(1));
  if(argPack != INVALID_HANDLE) {
    argPack = view_as<RPCArguments>(CloneHandle(argPack));
  }
  int length = 0;
  GetNativeStringLength(2, length);
  char[] value = new char[length + 1];
  GetNativeString(2, value, length + 1);

  argPack.appendString(value);
  argPack.Close();
}

public int Native_ArgPackPushBool(Handle plugin, int numParams) {
  RPCArguments argPack = view_as<RPCArguments>(GetNativeCell(1));
  if(argPack != INVALID_HANDLE) {
    argPack = view_as<RPCArguments>(CloneHandle(argPack));
  }
  bool value = view_as<bool>(GetNativeCell(2));
  argPack.appendBool(value);
  argPack.Close();
}
