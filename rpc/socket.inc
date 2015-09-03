#include <socket>
#include <smjansson>

public Handle listenSocket;

public void Sock_OpenRPCSocket() {
  listenSocket = SocketCreate(SOCKET_TCP, OnSocketError);

  char interface_[18];
  CVar_RPC_Interface.GetString(interface_, 18);
  SocketBind(listenSocket, interface_, CVar_RCP_Port.IntValue);
  SocketListen(listenSocket, OnSocketIncoming);
}

public void Sock_CloseRPCSocket() {
  listenSocket.Close();
}

public OnSocketError(Handle:socket, const errorType, const errorNum, any:arg) {
  LogError("socket error %d (errno %d)", errorType, errorNum);
  socket.Close();
}

public OnSocketIncoming(Handle socket, Handle newSocket, String:remoteIp[], remotePort, any:arg) {
  SocketSetReceiveCallback(newSocket, OnChildSocketReceive);
  SocketSetDisconnectCallback(newSocket, OnChildSocketDisconnected);
  SocketSetErrorCallback(newSocket, OnChildSocketError);
  DataPack pack = new DataPack();
  pack.WriteCell(0);
  pack.WriteCell(new ArrayStack(512));
  SocketSetArg(newSocket, pack);
}

public OnChildSocketReceive(Handle socket, String:receiveData[], const dataSize, any:arg) {
  int endMaker = StrContains(receiveData, "#!end");
  if(endMaker == -1) {
    DataPack pack = view_as<DataPack>(arg);
    pack.Reset();
    int stringCount = pack.ReadCell();
    ArrayStack stack = view_as<ArrayStack>(pack.ReadCell());
    stack.PushString(receiveData);

    pack.Reset();
    pack.WriteCell(stringCount + 1);
  } else {
    decl String:data[dataSize - 5];
    substr(receiveData, data, 0, endMaker);

    DataPack pack = view_as<DataPack>(arg);
    pack.Reset();
    int stringCount = pack.ReadCell();
    ArrayStack stack = view_as<ArrayStack>(pack.ReadCell());
    stack.PushString(data);

    pack.Reset();
    pack.WriteCell(stringCount + 1);

    FireMessage(view_as<Socket>(socket), pack);
  }
}

public OnChildSocketDisconnected(Handle socket, any hFile) {
  socket.Close();
  if(arg != INVALID_HANDLE) {
    DataPack pack = view_as<DataPacK>(arg);
    pack.ReadCell();
    ArrayStack stack = view_as<ArrayStack>(pack.ReadCell());
    pack.Close();
    if(stack != INVALID_HANDLE) {
      stack.Close();
    }
  }
}

public OnChildSocketError(Handle socket, const int errorType, const int errorNum, any arg) {
  LogError("child socket error %d (errno %d)", errorType, errorNum);
  CloseHandle(socket);
}

public void FireMessage(Socket socket, DataPack pack) {
  pack.Reset();
  int stringCount = pack.ReadCell();
  int bufSize = 512 * stringCount + 1;
  ArrayStack stack = view_as<ArrayStack>(pack.ReadCell());
  decl String:buffers[stringCount][512];
  decl String:buffer[bufSize];
  buffer[0] = '\0';

  for(int c = stringCount - 1; c >= 0; c--) {
    stack.PopString(buffers[c], 512);
  }
  stack.Close();

  for(int c = 0; c < stringCount; c++) {
    StrCat(buffer, bufSize, buffers[c]);
  }

  pack.Reset();
  pack.WriteCell(0);
  pack.WriteCell(new ArrayStack(512));
  NewMessage(socket, buffer, bufSize);
}

public OnSendSocketConnected(Handle socket, any arg) {
  Socket sock = view_as<Socket>(socket);
  JsonObject request = view_as<JsonObject>(arg);
  decl String:buffer[2048];
  request.dump(buffer, sizeof(buffer), 0);
  request.Close();

  DataPack pack = new DataPack();
  pack.WriteCell(0);
  pack.WriteCell(new ArrayStack(512));
  sock.SetArg(pack);

  decl String:sMessage[4096];
  int length = Sign(buffer, sMessage, sizeof(sMessage));

  sock.Send(sMessage, length);
}

public Socket SendTo(const char[] address, int port, JsonObject request) {
  Socket socket = view_as<Socket>(SocketCreate(SOCKET_TCP, OnSocketError));
  socket.SetArg(request);
  SocketConnect(socket, OnSendSocketConnected, OnChildSocketReceive, OnChildSocketDisconnected, address, port);
}

public void substr(const char[] source, char[] dest, int startIndex, int endIndex) {
  for(int c = startIndex; c < endIndex; c++) {
    dest[c] = source[c];
  }
  dest[endIndex] = '\0';
}
