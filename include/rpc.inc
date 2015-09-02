#if defined _rpc_included
 #endinput
#endif
#define _rpc_included

#include <smjansson>

native void RegisterRPC(const char[] fname);

native void CallRemoteProcedure(const char[] server, const char[] method, RPCArguments args);

methodmap JsonValue < Handle {
  public dump() = json_dump;
}

methodmap JsonObject < JsonValue {
  public JsonObject() {
    return view_as<JsonObject>(json_object());
  }
  public put() = json_object_set_new;
  public void putString(const char[] key, const char[] value) {
    this.put(key, json_string(value));
  }
  public void putInt(const char[] key, int value) {
    this.put(key, json_integer(value));
  }
}

methodmap JsonArray < JsonValue {
  public JsonArray() {
    return view_as<JsonArray>(json_array());
  }
  public append() = json_array_append;
  public appendNew() = json_array_append_new;
  public void appendString(const char[] value) {
    this.appendNew(json_string(value));
  }
  public void appendInt(int value) {
    this.appendNew(json_integer(value));
  }
  public void appendFloat(float value) {
    this.appendNew(json_real(value));
  }
  public void appendBool(bool value) {
    this.appendNew(json_boolean(value));
  }
}


native RPCArguments CreateArgumentPack();
native void ArgPackPushInt(RPCArguments argPack, int value);
native void ArgPackPushFloat(RPCArguments argPack, float value);
native void ArgPackPushString(RPCArguments argPack, const char[] value);
native void ArgPackPushBool(RPCArguments argPack, bool value);

methodmap RPCArguments < JsonArray {
  public native RPCArguments() = CreateArgumentPack;

  public native void PushInt(int value) = ArgPackPushInt;

  public native void PushFloat(float value) = ArgPackPushFloat;

  public native void PushString(const char[] value) = ArgPackPushString;

  public native void PushBool(bool value) = ArgPackPushBool;
}
