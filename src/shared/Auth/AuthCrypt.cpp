
#include "AuthCrypt.h"
#include "HMACSHA1.h"
#include "Logging/Log.h"
#include "BigNumber.h"

const static size_t CRYPTED_SEND_LEN = 4;
const static size_t CRYPTED_RECV_LEN = 6;

AuthCrypt::AuthCrypt()
{
    _initialized = false;
}

void AuthCrypt::DecryptRecv(uint8* data, size_t len)
{
    if (!_initialized)
        {
            return;
        }
    if (len < CRYPTED_RECV_LEN) { return; }

    for (size_t t = 0; t < CRYPTED_RECV_LEN; t++)
    {
        _recv_i %= _key.size();
        uint8 x = (data[t] - _recv_j) ^ _key[_recv_i];
        ++_recv_i;
        _recv_j = data[t];
        data[t] = x;
    }
}

void AuthCrypt::EncryptSend(uint8* data, size_t len)
{
    if (!_initialized)
        {
            return;
        }

    if (len < CRYPTED_SEND_LEN) { return; }

    for (size_t t = 0; t < CRYPTED_SEND_LEN; t++)
    {
        _send_i %= _key.size();
        uint8 x = (data[t] ^ _key[_send_i]) + _send_j;
        ++_send_i;
        data[t] = _send_j = x;
    }
}

void AuthCrypt::Init()
{
    _send_i = _send_j = _recv_i = _recv_j = 0;
    _initialized = true;
}

void AuthCrypt::SetKey(uint8* key, size_t len)
{
    _key.resize(len);
    std::copy(key, key + len, _key.begin());
}

AuthCrypt::~AuthCrypt()
{
}
