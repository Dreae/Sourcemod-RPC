#include <smjansson>
#include <socket>

methodmap Socket < Handle {
  public Send() = SocketSend;
  public SetArg() = SocketSetArg;
}
