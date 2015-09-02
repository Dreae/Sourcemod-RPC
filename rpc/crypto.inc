#include <curl>

public bool CheckSignature(const char[] signature, const char[] data, int dataLen) {
  decl String:key[129];
  CVar_RPC_SigningKey.GetString(key, sizeof(key));

  decl String:signatureComputed[129];
  HS512(data, key, signatureComputed, 129);
  return strcmp(signature, signatureComputed) == 0;
}

public void HS512(const char[] message, const char[] key, char[] output, int maxlen) {
  char keyPad[130];
  if(strlen(key) > 128) {
    curl_hash_string(key, strlen(key), Openssl_Hash_SHA512, keyPad, sizeof(keyPad));
  } else {
    strcopy(keyPad, sizeof(keyPad), key);
  }

  //Begin inappropriate type fondling
  char o[129];
  for(int c = 0; c < 128; c++) {
    /*
      Don't XOR values that will result in a 0.
      Doesn't conform to HMAC standard, but what
      do you want from a language that doesn't have
      a byte type
    */
    if(asInt(keyPad[c]) == 92) {
      o[c] = 92;
    } else {
      o[c] = view_as<char>(92 ^ asInt(keyPad[c]));
    }
  }
  char i[129];
  for(int c = 0; c < 128; c++) {
    if(asInt(keyPad[c]) == 54) {
      i[c] = 54; // See above
    } else {
      i[c] = view_as<char>(54 ^ asInt(keyPad[c]));
    }
  }

  int i_pad_len = 129 + strlen(message);
  char[] i_pad_msg = new char[i_pad_len];
  strcopy(i_pad_msg, 128, i);
  StrCat(i_pad_msg, i_pad_len, message);

  char i_pad_hash[129];
  curl_hash_string(i_pad_msg, i_pad_len, Openssl_Hash_SHA512, i_pad_hash, sizeof(i_pad_hash));

  char o_pad_msg[257];
  strcopy(o_pad_msg, 257, o);
  StrCat(o_pad_msg, 257, i_pad_hash);

  curl_hash_string(o_pad_msg, 257, Openssl_Hash_SHA512, output, maxlen);
}

public int asInt(char c) {
  /*
    As odd as this seems it's so that c is copied
    by value, giving us 32 bits as { 0x00, 0x00, 0x00, c }.
    Simply calling view_as<int>(keyPad[c]) was resulting
    in garbage values, presumably because view_as was consuming
    adjacent bytes to create an int.
  */
  return view_as<int>(c);
}
