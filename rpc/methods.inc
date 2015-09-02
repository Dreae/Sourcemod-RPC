#include <sourcemod>

StringMap methods;

public void initStringMap() {
  methods = new StringMap();
}

public void RegisterNewMethod(Handle plugin, const char[] name) {
  methods.SetValue(name, plugin);
}
