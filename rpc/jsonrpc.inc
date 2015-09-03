#include <smjansson>
#include <base64>

enum RPCError {
  RPCError_ParseError = 0,
  RPCError_InvalidRequest = 1,
  RPCError_SignatureInvalid = 2,
  RPCError_NoSuchMethod = 3,
};

public void NewMessage(Socket socket, const char[] receiveData, const int dataSize) {
 //TODO: Convert this to use json_load_ex()
  Handle wrapper = json_load(receiveData);
  if(wrapper == INVALID_HANDLE) {
    SendError(socket, RPCError_ParseError);
    return;
  }
  Handle signature = json_object_get(wrapper, "signature");
  Handle data = json_object_get(wrapper, "data");
  if(signature == INVALID_HANDLE || data == INVALID_HANDLE) {
    SendError(socket, RPCError_InvalidRequest);
    return;
  }

  decl String:sBuffer[129];
  json_string_value(signature, sBuffer, 129);

  decl String:msgData[dataSize];
  json_string_value(data, msgData, dataSize);
  if(!CheckSignature(sBuffer, msgData, strlen(msgData))) {
    SendError(socket, RPCError_SignatureInvalid);
    return;
  }
  decl String:sJsonData[4096];
  DecodeBase64(sJsonData, sizeof(sJsonData), msgData);
  JsonObject jsonData = view_as<JsonObject>(json_load(sJsonData));
  Handle result = jsonData.get("result");
  if(result != INVALID_HANDLE) {

  } else {
    decl String:method[129];
    jsonData.getString("method", method, sizeof(method));
    JsonArray args = view_as<JsonArray>(jsonData.get("params"));
  }

  SendError(socket, RPCError_NoSuchMethod);
}

public int Sign(const char[] data, char[] buffer, int maxlen) {
  int dataLen = strlen(data);
  int est = RoundToCeil(FloatMul(4.0, FloatDiv(float(dataLen), 3.0)));
  int total = est + (4 - (est % 4)) + 1;

  decl String:b64[total];
  EncodeBase64(b64, total, data, dataLen);

  decl String:key[256];
  CVar_RPC_SigningKey.GetString(key, sizeof(key));

  decl String:signature[129];
  HS512(b64, key, signature, 129);
  JsonObject message = new JsonObject();
  message.putString("data", b64);
  message.putString("signature", signature);

  int length = message.dump(buffer, maxlen, 0);
  message.Close();
  return length;
}

public void SignAndSend(Socket socket, const char[] data) {
  decl String:sMessage[2048];
  int length = Sign(data, sMessage, sizeof(sMessage));

  socket.Send(sMessage, length);
}

public void SendError(Socket socket, RPCError err) {
  JsonObject error;
  switch(err) {
    case RPCError_ParseError: {
      error = errorParseError();
    }
    case RPCError_InvalidRequest: {
      error = errorInvalidRequest();
    }
    case RPCError_SignatureInvalid: {
      error = errorSignatureCheckFailed();
    }
    case RPCError_NoSuchMethod: {
      error = errorNoSuchMethod("foo");
    }
  }
  decl String:res[256];
  error.dump(res, sizeof(res), 0);
  error.Close();

  SignAndSend(socket, res);
}

public JsonObject jrpcWrapper() {
  JsonObject jObj = new JsonObject();
  jObj.putString("jsonrpc", "2.0");
  return jObj;
}

public JsonObject errorNoSuchMethod(const char[] id) {
  JsonObject jObj = jrpcWrapper();
  jObj.putString("id", id);

  JsonObject error = new JsonObject();
  error.putString("message", "Method not found");
  error.putInt("code", -32601);
  jObj.put("error", error);

  return jObj;
}

public JsonObject errorSignatureCheckFailed() {
  JsonObject jObj = jrpcWrapper();
  //ID is intentionally omitted because we can't verify it

  JsonObject error = new JsonObject();
  error.putString("message", "Signature verification failed");
  error.putInt("code", -32000);
  jObj.put("error", error);

  return jObj;
}

public JsonObject errorParseError() {
  JsonObject jObj = jrpcWrapper();

  JsonObject error = new JsonObject();
  error.putString("message", "Unable to parse JSON");
  error.putInt("code", -32700);
  jObj.put("error", error);

  return jObj;
}

public JsonObject errorInvalidRequest() {
  JsonObject jObj = jrpcWrapper();

  JsonObject error = new JsonObject();
  error.putString("message", "Invalid request");
  error.putInt("code", -32600);
  jObj.put("error", error);

  return jObj;
}
