// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_WALLETDB_H
#define BITCOIN_WALLET_WALLETDB_H
//#define loop                for (;;)

#include "amount.h"
#include "primitives/transaction.h"
#include "primitives/zerocoin.h"
#include "hdmint/hdmint.h"
#include "hdmint/mintpool.h"
#include "wallet/db.h"
#include "key.h"

#include "../secp256k1/include/GroupElement.h"
#include "../secp256k1/include/Scalar.h"

#include <list>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>
#include "libzerocoin/Zerocoin.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>


static const bool DEFAULT_FLUSHWALLET = true;

class CAccount;
class CAccountingEntry;
struct CBlockLocator;
class CKeyPool;
class CMasterKey;
class CScript;
class CWallet;
class CWalletTx;
class uint160;
class uint256;
class CZerocoinEntry;
class CSigmaEntry;
class CZerocoinSpendEntry;
class CSigmaSpendEntry;

/** Error statuses for the wallet database */
enum DBErrors
{
    DB_LOAD_OK,
    DB_CORRUPT,
    DB_NONCRITICAL_ERROR,
    DB_TOO_NEW,
    DB_LOAD_FAIL,
    DB_NEED_REWRITE
};

/** IDs for components in the HD Keypath */
enum class HDKeypathID {
    nMaster,
    nPurpose,
    nCoinType,
    nAccount,
    nChange,
    nExternalChainChild,
    nChild
};

//BIP32 Component
class CComponent {
    public:
        CComponent(std::string nComponent, int64_t nDepth) : nDepth{nDepth}, value{SetValue(nComponent)}, isHardened{SetHardened(nComponent)} {
            this->nComponent = nComponent;
        }

        void SetID(HDKeypathID nID){
            this->nID = nID;
        }

        int64_t GetValue(){
            return value;
        }

        bool IsHardened(){
            return isHardened;
        }

        HDKeypathID GetID(){
            return nID;
        }

    private:

        std::string nComponent;
        int64_t value = -1;
        bool isHardened = false;
        HDKeypathID nID;
        int64_t nDepth = -1;

        bool SetHardened(std::string nComponent){
            if(nComponent.size()==1) return false;
            //std::string hardened = nComponent[nComponent.size()-1];
            //const char newHardened = '\'';
            return (nComponent.at(nComponent.size()-1) == '\'');
        }

        int64_t SetValue(std::string nComponent){
            std::string nComponentValue = nComponent;
            if(isHardened)
                boost::erase_all(nComponentValue, "'");
            if(!(nComponent.find_first_not_of("0123456789") == std::string::npos))
                return boost::lexical_cast<int64_t>(nComponentValue);
            return -1;
        }
};

//HD Keypath. Layout according to a particular BIP (eg. BIP44)
class CHDKeypath {
    public:
        void AddComponent(CComponent cComponent){
            nComponents.push_back(cComponent);
        }

        CComponent& operator[](HDKeypathID nID){
            return nComponents[nID];
        }

    private:
        vector<CComponent> nComponents;
        int nSize;
};

//Legacy Keypath - m/0'/0'/<n>
class CLegacyHDKeypath : public CHDKeypath {
    static const int LEGACY_DEPTH = 4;

    public:
        CLegacyHDKeypath(std::vector<std::string> vComponents) {
            SetComponents(vComponents);
        }

    private:
        void SetComponents(std::vector<std::string> vComponents){
            if(vComponents.size()!=LEGACY_DEPTH)
                return;

            for(size_t nDepth=0; nDepth<LEGACY_DEPTH; nDepth++){
                CComponent cComponent(vComponents[nDepth], nDepth);
                switch(nDepth) {
                    case 0:
                        cComponent.SetID(HDKeypathID::nMaster);
                        break;
                    case 1:
                        cComponent.SetID(HDKeypathID::nAccount);
                        break;
                    case 2:
                        cComponent.SetID(HDKeypathID::nExternalChainChild);
                        break;
                    case 3:
                        cComponent.SetID(HDKeypathID::nChild);
                        break;
                }
                AddComponent(cComponent);
            }    
        }
};

//BIP44 Keypath - m/44'/<1/136>'/0'/<c>/<n>
class CBIP44HDKeypath : public CHDKeypath {
    static const int BIP44_DEPTH = 6;

    public:
        CBIP44HDKeypath(std::vector<std::string> vComponents) {
            SetComponents(vComponents);
        }

    private:
        void SetComponents(std::vector<std::string> vComponents){
            if(vComponents.size()!=BIP44_DEPTH)
                return;

            for(size_t nDepth=0; nDepth<BIP44_DEPTH; nDepth++){
                CComponent cComponent(vComponents[nDepth], nDepth);
                switch(nDepth) {
                    case 0:
                        cComponent.SetID(HDKeypathID::nMaster);
                        break;
                    case 1:
                        cComponent.SetID(HDKeypathID::nPurpose);
                        break;
                    case 2:
                        cComponent.SetID(HDKeypathID::nCoinType);
                        break;
                    case 3:
                        cComponent.SetID(HDKeypathID::nAccount);
                        break;
                    case 4:
                        cComponent.SetID(HDKeypathID::nChange);
                        break;
                    case 5:
                        cComponent.SetID(HDKeypathID::nChild);
                        break;
                }
                AddComponent(cComponent);
            }    
        }
};

/* simple HD chain data model */
class CHDChain
{
public:
    uint32_t nExternalChainCounter; // VERSION_BASIC
    vector<uint32_t> nExternalChainCounters; // VERSION_WITH_BIP44: vector index corresponds to account value
    CKeyID masterKeyID; //!< master key hash160

    static const int VERSION_BASIC = 1;
    static const int VERSION_WITH_BIP44 = 10;
    static const int CURRENT_VERSION = VERSION_WITH_BIP44;
    static const int N_CHANGES = 3; // standard = 0/1, mint = 2
    int nVersion;

    CHDChain() { SetNull(); }
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {

        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nExternalChainCounter);
        READWRITE(masterKeyID);
        if(this->nVersion >= VERSION_WITH_BIP44){
            READWRITE(nExternalChainCounters);
        }
    }

    void SetNull()
    {
        nVersion = CHDChain::CURRENT_VERSION;
        masterKeyID.SetNull();
        nExternalChainCounter = 0;
        for(int index=0;index<N_CHANGES;index++){
            nExternalChainCounters.push_back(0);
        }
    }
};

class CKeyMetadata
{
public:
    static const int VERSION_BASIC=1;
    static const int VERSION_WITH_HDDATA=10;
    static const int CURRENT_VERSION=VERSION_WITH_HDDATA;
    int nVersion;
    int64_t nCreateTime; // 0 means unknown
    CHDKeypath nHdKeypath; // optional HD/bip32 keypath (object)
    std::string hdKeypath; //optional HD/bip32 keypath (string)

    CKeyID hdMasterKeyID; //id of the HD masterkey used to derive this key

    CKeyMetadata()
    {
        SetNull();
    }
    CKeyMetadata(int64_t nCreateTime_)
    {
        SetNull();
        nCreateTime = nCreateTime_;
    }

    bool ParseComponents(){
        std::vector<std::string> nComponents;
        if(hdKeypath=="m")
            return false;
        boost::split(nComponents, hdKeypath, boost::is_any_of("/"), boost::token_compress_on);

        if(nComponents[1].compare("44'") == 0){
            nHdKeypath = CBIP44HDKeypath(nComponents);
        }else{
            nHdKeypath = CLegacyHDKeypath(nComponents);
        }
        return true;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nCreateTime);
        if (this->nVersion >= VERSION_WITH_HDDATA)
        {
            READWRITE(hdKeypath);
            READWRITE(hdMasterKeyID);
        }
    }

    void SetNull()
    {
        nVersion = CKeyMetadata::CURRENT_VERSION;
        nCreateTime = 0;
        hdKeypath.clear();
        hdMasterKeyID.SetNull();
    }
};

/** Access to the wallet database */
class CWalletDB : public CDB
{
public:
    CWalletDB(const std::string& strFilename, const char* pszMode = "r+", bool fFlushOnClose = true) : CDB(strFilename, pszMode, fFlushOnClose)
    {
    }

    bool WriteName(const std::string& strAddress, const std::string& strName);
    bool EraseName(const std::string& strAddress);

    bool WritePurpose(const std::string& strAddress, const std::string& purpose);
    bool ErasePurpose(const std::string& strAddress);

    bool WriteTx(const CWalletTx& wtx);
    bool EraseTx(uint256 hash);

    bool WriteKey(const CPubKey& vchPubKey, const CPrivKey& vchPrivKey, const CKeyMetadata &keyMeta);
    bool WriteCryptedKey(const CPubKey& vchPubKey, const std::vector<unsigned char>& vchCryptedSecret, const CKeyMetadata &keyMeta);
    bool WriteMasterKey(unsigned int nID, const CMasterKey& kMasterKey);

    bool WriteCScript(const uint160& hash, const CScript& redeemScript);

    bool WriteWatchOnly(const CScript &script);
    bool EraseWatchOnly(const CScript &script);

    bool WriteBestBlock(const CBlockLocator& locator);
    bool ReadBestBlock(CBlockLocator& locator);

    bool WriteOrderPosNext(int64_t nOrderPosNext);

    bool WriteDefaultKey(const CPubKey& vchPubKey);

    bool ReadPool(int64_t nPool, CKeyPool& keypool);
    bool WritePool(int64_t nPool, const CKeyPool& keypool);
    bool ErasePool(int64_t nPool);

    bool WriteMinVersion(int nVersion);

    /// This writes directly to the database, and will not update the CWallet's cached accounting entries!
    /// Use wallet.AddAccountingEntry instead, to write *and* update its caches.
    bool WriteAccountingEntry_Backend(const CAccountingEntry& acentry);
    bool ReadAccount(const std::string& strAccount, CAccount& account);
    bool WriteAccount(const std::string& strAccount, const CAccount& account);

    /// Write destination data key,value tuple to database
    bool WriteDestData(const std::string &address, const std::string &key, const std::string &value);
    /// Erase destination data tuple from wallet database
    bool EraseDestData(const std::string &address, const std::string &key);

    CAmount GetAccountCreditDebit(const std::string& strAccount);
    void ListAccountCreditDebit(const std::string& strAccount, std::list<CAccountingEntry>& acentries);

    bool WriteZerocoinEntry(const CZerocoinEntry& zerocoin);
    bool WriteZerocoinEntry(const CSigmaEntry& zerocoin);
    bool ReadZerocoinEntry(const Bignum& pub, CZerocoinEntry& entry);
    bool ReadZerocoinEntry(const secp_primitives::GroupElement& pub, CSigmaEntry& entry);
    bool HasZerocoinEntry(const Bignum& pub);
    bool HasZerocoinEntry(const secp_primitives::GroupElement& pub);
    bool EraseZerocoinEntry(const CZerocoinEntry& zerocoin);
    bool EraseZerocoinEntry(const CSigmaEntry& zerocoin);
    void ListPubCoin(std::list<CZerocoinEntry>& listPubCoin);
    void ListSigmaPubCoin(std::list<CSigmaEntry>& listPubCoin);
    void ListCoinSpendSerial(std::list<CZerocoinSpendEntry>& listCoinSpendSerial);
    void ListCoinSpendSerial(std::list<CSigmaSpendEntry>& listCoinSpendSerial);
    bool WriteCoinSpendSerialEntry(const CZerocoinSpendEntry& zerocoinSpend);
    bool WriteCoinSpendSerialEntry(const CSigmaSpendEntry& zerocoinSpend);
    bool HasCoinSpendSerialEntry(const Bignum& serial);
    bool HasCoinSpendSerialEntry(const secp_primitives::Scalar& serial);
    bool EraseCoinSpendSerialEntry(const CZerocoinSpendEntry& zerocoinSpend);
    bool EraseCoinSpendSerialEntry(const CSigmaSpendEntry& zerocoinSpend);
    bool WriteZerocoinAccumulator(libzerocoin::Accumulator accumulator, libzerocoin::CoinDenomination denomination, int pubcoinid);
    bool ReadZerocoinAccumulator(libzerocoin::Accumulator& accumulator, libzerocoin::CoinDenomination denomination, int pubcoinid);
    // bool EraseZerocoinAccumulator(libzerocoin::Accumulator& accumulator, libzerocoin::CoinDenomination denomination, int pubcoinid);

    bool ReadCalculatedZCBlock(int& height);
    bool WriteCalculatedZCBlock(int height);

    DBErrors ReorderTransactions(CWallet* pwallet);
    DBErrors LoadWallet(CWallet* pwallet);
    DBErrors FindWalletTx(CWallet* pwallet, std::vector<uint256>& vTxHash, std::vector<CWalletTx>& vWtx);
    DBErrors ZapWalletTx(CWallet* pwallet, std::vector<CWalletTx>& vWtx);
    DBErrors ZapSelectTx(CWallet* pwallet, std::vector<uint256>& vHashIn, std::vector<uint256>& vHashOut);
    DBErrors ZapSigmaMints(CWallet* pwallet);
    static bool Recover(CDBEnv& dbenv, const std::string& filename, bool fOnlyKeys);
    static bool Recover(CDBEnv& dbenv, const std::string& filename);

    bool ReadZerocoinCount(int32_t& nCount);
    bool WriteZerocoinCount(const int32_t& nCount);

    bool ReadZerocoinSeedCount(int32_t& nCount);
    bool WriteZerocoinSeedCount(const int32_t& nCount);

    bool ArchiveMintOrphan(const CZerocoinEntry& zerocoin);
    bool ArchiveDeterministicOrphan(const CHDMint& dMint);
    bool UnarchiveZerocoinMint(const uint256& hashPubcoin, CSigmaEntry& zerocoin);
    bool UnarchiveHDMint(const uint256& hashPubcoin, CHDMint& dMint);

    bool WriteHDMint(const CHDMint& dMint);
    bool ReadHDMint(const uint256& hashPubcoin, CHDMint& dMint);
    bool EraseHDMint(const CHDMint& dMint);
    bool HasHDMint(const secp_primitives::GroupElement& pub);

    std::list<CHDMint> ListHDMints();
    bool WritePubcoin(const uint256& hashSerial, const GroupElement& hashPubcoin);
    bool ReadPubcoin(const uint256& hashSerial, GroupElement& hashPubcoin);
    std::vector<std::pair<uint256, GroupElement>> ListSerialPubcoinPairs();
    bool WriteMintPoolPair(const uint256& hashPubcoin, const std::tuple<uint160, CKeyID, int32_t>& hashSeedMintPool);
    bool ReadMintPoolPair(const uint256& hashPubcoin, uint160& hashSeedMaster, CKeyID& seedId, int32_t& nCount);
    std::vector<std::pair<uint256, MintPoolEntry>> ListMintPool();

    //! write the hdchain model (external chain child index counter)
    bool WriteHDChain(const CHDChain& chain);

private:
    CWalletDB(const CWalletDB&);
    void operator=(const CWalletDB&);

    bool WriteAccountingEntry(const uint64_t nAccEntryNum, const CAccountingEntry& acentry);
};

void ThreadFlushWalletDB(const std::string& strFile);
bool AutoBackupWallet (CWallet* wallet, std::string strWalletFile, std::string& strBackupWarning, std::string& strBackupError);

#endif // BITCOIN_WALLET_WALLETDB_H
