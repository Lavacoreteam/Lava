// Copyright (c) 2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_ZMQ_ZMQABSTRACT_H
#define BITCOIN_ZMQ_ZMQABSTRACT_H

#include "zmqconfig.h"

class CBlockIndex;
class CZMQAbstract;

typedef CZMQAbstract* (*CZMQFactory)();




class CZMQAbstract
{
public:
    enum KeyType {
        Server,
        Client
    };

    CZMQAbstract() : psocket(0),pcontext(0) { }
    virtual ~CZMQAbstract();

    template <typename T>
    static CZMQAbstract* Create()
    {
        return new T();
    }

    std::string GetType() const { return type; }
    void SetType(const std::string &t) { type = t; }
    std::string GetPort() const { return port; }
    void SetPort(const std::string &p) { port = p; }
    std::string GetAddress() const { return address; }
    void SetAddress(const std::string &a) { address = a; }
    // address + port combination
    std::string GetAuthority() const { return address + port; }
    void SetAuthority(const std::string &a) { authority = a; }

    static vector<string> readCert(KeyType type);
    static bool writeCert(string publicKey, string privateKey, KeyType type);
    static bool createCerts();
    static std::string GetAuthType(KeyType type);

    // define pure virtual functions (implemented by every derived instance)
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    // to be implemented by publishers
    virtual bool NotifyBlock(const CBlockIndex *pindex);
    virtual bool NotifyTransaction(const CTransaction &transaction);

protected:
    void *psocket;
    void *pcontext;
    std::string type;
    std::string address;
    std::string port;
    std::string authority;
};

#endif // BITCOIN_ZMQ_ZMQABSTRACT_H
