/*
    This file is part of cpp-ethereum.

    cpp-ethereum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-ethereum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Ethash.h"
#include "EthashProofOfWork.h"

#include <libethash/ethash.h>

namespace dev
{
namespace eth
{

class BlockHeader;

class EthashAux
{
public:
    ~EthashAux();

    static EthashAux* get();

    struct LightAllocation
    {
        LightAllocation(h256 const& _seedHash);
        ~LightAllocation();
        bytesConstRef data() const;
        ethash_light_t light;
        uint64_t size;
    };

    struct FullAllocation
    {
        FullAllocation(ethash_light_t _light, ethash_callback_t _cb);
        ~FullAllocation();
        bytesConstRef data() const;
        uint64_t size() const { return ethash_full_dag_size(full); }
        ethash_full_t full;
    };

    using LightType = std::shared_ptr<LightAllocation>;
    using FullType = std::shared_ptr<FullAllocation>;

    static LightType light(h256 const& _seedHash);

    static const uint64_t NotGenerating = (uint64_t)-1;
    /// Kicks off generation of DAG for @a _seedHash and @returns false or @returns true if ready.
    static unsigned computeFull(h256 const& _seedHash, bool _createIfMissing = true);
    /// Information on the generation progress.
    static std::pair<uint64_t, unsigned> fullGeneratingProgress() { return std::make_pair(get()->m_generatingFullNumber, get()->m_fullProgress); }
    /// Kicks off generation of DAG for @a _blocknumber and blocks until ready; @returns result or empty pointer if not existing and _createIfMissing is false.
    static FullType full(h256 const& _seedHash, bool _createIfMissing = false, std::function<int(unsigned)> const& _f = std::function<int(unsigned)>());

private:
    EthashAux() {}

    static EthashAux* s_this;

    SharedMutex x_lights;
    std::unordered_map<h256, std::shared_ptr<LightAllocation>> m_lights;

    Mutex x_fulls;
    std::unordered_map<h256, std::weak_ptr<FullAllocation>> m_fulls;
    FullType m_lastUsedFull;
    std::unique_ptr<std::thread> m_fullGenerator;
    uint64_t m_generatingFullNumber = NotGenerating;
    unsigned m_fullProgress;
};

}
}
