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

#ifndef OPENTXS_STORAGE_STORAGEEXAMPLE_HPP
#define OPENTXS_STORAGE_STORAGEEXAMPLE_HPP

#include "opentxs/storage/StoragePlugin.hpp"

namespace opentxs
{

class Storage;
class StorageConfig;

/** \brief An example implementation of \ref Storage which demonstrates how
 *  to implement support for new storage backends.
 *
 *  \ingroup storage
 *
 *  \par Basic Concepts
 *  \ref Storage requires that implementations capable of key-value storage
 *  and retrieval.
 *
 *  \par
 *  Keys must be capable of being stored to, and retrieved from, one of two
 *  specified buckets: primary or secondary.
 *
 *  \par
 *  The parent class guarentees that all key-value pairs accessed via the \ref Load
 *  and \ref Store methods satisfy the following conditions:
 *
 *  \par
 *    * Values are immutable
 *    * Keys are unique
 *
 *  \par
 *  Those guarentees typically mean that an implementation only requires a small
 *  subset of the backend's feature set. In particular, the assumptions are
 *  valid:
 *
 *  \par
 *   * The ability to alter a value after it has been stored is NOT required
 *   * The ability to delete an individual value is NOT required
 *   * \ref Store and \ref Load calls are idempotent.
 *
 *  \par
 *  \note The parent class will rely on the idempotence of \ref Store and \ref Load
 *  methods. Ensure these methods are thread safe.
 *
 *  \par Root Hash
 *  The root hash is the only exception to the general rule of immutable values.
 *  See the description of the \ref LoadRoot and \ref StoreRoot methods for details.
 *: ot_super(config, hash random)
 *  \par Configuration
 *  Define all needed runtime configuration parameters in the
 *  \ref StorageConfig class.
 *
 *  \par
 *  Instantate these parameters in the \ref OT::Init_Storage method,
 *  using the existing sections as a template.
 */
class StorageExample
    : public virtual StoragePlugin_impl
    , public virtual StorageDriver
{
private:
    typedef StoragePlugin_impl ot_super; // Used for constructor delegation

    friend Storage; // Allows access private constructor

    /** The default constructor can not be used because any implementation
    *   of \ref StorageDriver will require arguments.
    */
    StorageExample() = delete;

    /** This is the required parameter profile for an opentx::storage child
     *  class constructor.
     *
     *  \param[in] config An instantiated \ref StorageConfig object
     *                    containing configuration information. Used both by
     *                    the child class and the parent class.
     *  \param[in] hash   A std::function providing digest functionality. Only
     *                    used in the parent class.
     *  \param[in] random A std::function providing random number generation.
     *                    May be useful for some child classes.
     *  \param[in] bucket Reference to bool containing the current bucket
     */
    StorageExample(
        const StorageConfig& config,
        const Digest& hash,
        const Random& random,
        std::atomic<bool>& bucket)
        : ot_super(config, hash random, bucket)
    {
        Init_StorageExample();
    }

    /** The copy constructor for \ref Storage classes must be disabled
     *  because the class will always be instantiated as a singleton.
     */
    StorageExample(const StorageExample&) = delete;

    /** The copy assignment operator for \ref Storage classes must be
     *  disabled because the class will always be instantiated as a singleton.
     */
    StorageExample& operator=(const StorageExample&) = delete;

    /** Polymorphic initialization method. Child class-specific actions go here.
     */
    void Init_StorageExample();

    /** Polymorphic cleanup method. Child class-specific actions go here.
     */
    void Cleanup_StorageExample();

public:
    /** Obtain the most current value of the root hash
     *  \returns The value most recently provided to the \ref StoreRoot method,
     *           or an empty string
     *
     *  \note The parent class guarentees that calls to \ref LoadRoot and
     *        \ref StoreRoot will be properly synchronized.
     *
     *  \warning This method is required to be thread safe
     *
     *  \par Implementation
     *  Child classes should implement this functionality via any method which
     *  achieves the required behavior.
     */
    std::string LoadRoot() const override;

    /** Record a new value for the root hash
     *  \param[in] hash the new root hash
     *  \returns true if the new value has been stored in the backend
     *
     *  \note The parent class guarentees that calls to \ref LoadRoot and
     *        \ref StoreRoot will be properly synchronized.
     *
     *  \par Implementation
     *  Child classes should implement this functionality via any method which
     *  achieves the required behavior.
     */
    bool StoreRoot(const std::string& hash) const override;

    using ot_super::Load; // Needed for overload resolution
    /** Retrieve a previously-stored value
     *
     *  \param[in] key the key of the object to be retrieved
     *  \param[out] value the value of the requested key
     *  \param[in] bucket search for the key in either the primary (true) or
     *                    secondary (false) bucket
     *  \returns true if the key was found in and loaded by the backend
     *
     *  \warning This method is required to be thread safe
     */
    bool Load(
        const std::string& key,
        std::string& value,
        const bool bucket) const override;

    using ot_super::Store; // Needed for overload resolution
    /** Record a new value in the backend
     *
     *  \param[in] key the key of the object to be stored
     *  \param[in] value the value of the specified key
     *  \param[in] bucket save the key in either the primary (true) or
     *                    secondary (false) bucket
     *  \returns true if the value was successfully recorded in the backend
     *
     *  \warning This method is required to be thread safe
     */
    bool Store(
        const std::string& key,
        const std::string& value,
        const bool bucket) const override;

    /** Completely erase the contents of the specified bucket
     *
     *  \param[in] bucket empty either the primary (true) or
     *                    secondary (false) bucket
     *  \returns true if the bucket was emptied and is ready to receive new
     *           objects
     *
     *  \note The parent class guarentees no calls to \ref Store or \ref Load
     *  specifying this bucket will be generated until this method returns.
     *
     *  \par Implementation
     *  Child classes should implement this behavior via the most efficient
     *  technique available for mass deletion of stored objects. For example:
     *
     *    * The filesystem driver uses directories to implement buckets.
     *      Emptying a bucket consists of renaming the directory to a random
     *      name, spawing a detached thread to delete it in the background,
     *      and creating a new empty directory with the correct name.
     *    * SQL-based backends can use tables to implement buckets. Emptying a
     *      bucket can be implemented with a DROP TABLE command followed by a
     *      CREATE TABLE command.
     */
    bool EmptyBucket(const bool bucket) override;

    /** Polymorphic cleanup method.
     */
    void Cleanup() override
    {
        Cleanup_StorageExample();
    }

    /** Polymorphic destructor.
     */
    ~StorageExample()
    {
        Cleanup_StorageExample();
    }
};

}  // namespace opentxs
#endif // OPENTXS_STORAGE_STORAGEEXAMPLE_HPP
