/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#ifndef OPENTXS_STORAGE_STORAGESQLITE3_HPP
#define OPENTXS_STORAGE_STORAGESQLITE3_HPP

#include "opentxs/storage/StoragePlugin.hpp"

extern "C"
{
    #include <sqlite3.h>
}

namespace opentxs
{

class Storage;
class StorageConfig;

// SQLite3 implementation of opentxs::storage
class StorageSqlite3
    : public virtual StoragePlugin_impl
    , public virtual StorageDriver
{
private:
    typedef StoragePlugin_impl ot_super;

    friend class Storage;

    std::string folder_;
    sqlite3* db_{nullptr};

    std::string GetTableName(const bool bucket) const;

    bool Select(
        const std::string& key,
        const std::string& tablename,
        std::string& value) const;
    bool Upsert(
        const std::string& key,
        const std::string& tablename,
        const std::string& value) const;
    bool Create(const std::string& tablename) const;
    bool Purge(const std::string& tablename) const;

    void Init_StorageSqlite3();

    StorageSqlite3(
        const StorageConfig& config,
        const Digest& hash,
        const Random& random,
        std::atomic<bool>& bucket);
    StorageSqlite3() = delete;
    StorageSqlite3(const StorageSqlite3&) = delete;
    StorageSqlite3(StorageSqlite3&&) = delete;
    StorageSqlite3& operator=(const StorageSqlite3&) = delete;
    StorageSqlite3& operator=(StorageSqlite3&&) = delete;

public:
    std::string LoadRoot() const override;
    bool StoreRoot(const std::string& hash) const override;
    using ot_super::Load;
    bool Load(
        const std::string& key,
        std::string& value,
        const bool bucket) const override;
    using ot_super::Store;
    bool Store(
        const std::string& key,
        const std::string& value,
        const bool bucket) const override;
    bool EmptyBucket(const bool bucket) const override;

    void Cleanup_StorageSqlite3();
    void Cleanup() override;
    ~StorageSqlite3();
};

}  // namespace opentxs
#endif // OPENTXS_STORAGE_STORAGESQLITE3_HPP
