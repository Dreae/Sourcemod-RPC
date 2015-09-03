StringMap servers;

public void loadServers() {
  servers = new StringMap();
  Handle serverFile = json_load_file("addons/sourcemod/configs/rpc/servers.json");
  Handle serverIter = json_object_iter(serverFile);
  while(serverIter != INVALID_HANDLE) {
    decl String:key[128];
    json_object_iter_key(serverIter, key, sizeof(key));
    Handle value = json_object_iter_value(serverIter);

    servers.SetValue(key, value);
    serverIter = json_object_iter_next(serverFile, serverIter);
  }

  serverFile.Close();
}

public void CallRemoteOnServer(const char[] server, const char[] method, RPCArguments args) {
  Handle info;
  bool exists = servers.GetValue(server, info);
  if(exists) {
    decl String:address[32];
    json_object_get_string(info, "ip", address, sizeof(address));
    int port = json_object_get_int(info, "port");

    JsonObject request = new JsonObject();
    request.putString("jsonrpc", "2.0");
    request.putString("method", method);
    request.put("params", args);

    SendTo(address, port, request);
  }
}
