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
/** @file
 * Ethash class testing.
 */

#include <libethash/internal.h>
#include <libethashseal/Ethash.h>

#include <test/tools/libtesteth/TestOutputHelper.h>

#include <ethash/ethash.hpp>

#include <boost/test/unit_test.hpp>

using namespace dev;
using namespace dev::eth;
using namespace dev::test;

BOOST_FIXTURE_TEST_SUITE(EthashTests, TestOutputHelperFixture)

// FIXME: Add a helper function here, because the test cases are almost identical.
// TODO: Add tests for Homestead difficulty change.

BOOST_AUTO_TEST_CASE(calculateDifficultyByzantiumWithoutUncles)
{
    ChainOperationParams params;
    params.homesteadForkBlock = 0;
    params.byzantiumForkBlock = u256(0x1000);

    Ethash ethash;
    ethash.setChainParams(params);

    BlockHeader parentHeader;
    parentHeader.clear();
    parentHeader.setNumber(0x2000);
    parentHeader.setTimestamp(100);
    parentHeader.setDifficulty(1000000);

    BlockHeader header;
    header.clear();
    header.setNumber(0x2001);
    header.setTimestamp(130);

    BOOST_CHECK_EQUAL(ethash.calculateDifficulty(header, parentHeader), 999024);
}

BOOST_AUTO_TEST_CASE(calculateDifficultyByzantiumWithUncles)
{
    ChainOperationParams params;
    params.homesteadForkBlock = 0;
    params.byzantiumForkBlock = u256(0x1000);

    Ethash ethash;
    ethash.setChainParams(params);

    BlockHeader parentHeader;
    parentHeader.clear();
    parentHeader.setNumber(0x2000);
    parentHeader.setTimestamp(100);
    parentHeader.setDifficulty(1000000);
    parentHeader.setSha3Uncles(
        h256("0x949d991d685738352398dff73219ab19c62c06e6f8ce899fbae755d5127ed1ef"));

    BlockHeader header;
    header.clear();
    header.setNumber(0x2001);
    header.setTimestamp(130);

    BOOST_CHECK_EQUAL(ethash.calculateDifficulty(header, parentHeader), 999512);
}

BOOST_AUTO_TEST_CASE(calculateDifficultyByzantiumMaxAdjustment)
{
    ChainOperationParams params;
    params.homesteadForkBlock = 0;
    params.byzantiumForkBlock = u256(0x1000);

    Ethash ethash;
    ethash.setChainParams(params);

    BlockHeader parentHeader;
    parentHeader.clear();
    parentHeader.setNumber(0x2000);
    parentHeader.setTimestamp(100);
    parentHeader.setDifficulty(1000000);

    BlockHeader header;
    header.clear();
    header.setNumber(0x2001);
    header.setTimestamp(1100);

    BOOST_CHECK_EQUAL(ethash.calculateDifficulty(header, parentHeader), 951688);
}

BOOST_AUTO_TEST_CASE(ethash_light_cache)
{
    struct ItemTest
    {
        size_t index;
        const char* hash;
    };

    struct EpochTest
    {
        uint64_t epoch;
        std::vector<ItemTest> test;
    };

    // clang-format off
    EpochTest testCases[] = {
        {0, {
            {0,      "5e493e76a1318e50815c6ce77950425532964ebbb8dcf94718991fa9a82eaf37658de68ca6fe078884e803da3a26a4aa56420a6867ebcd9ab0f29b08d1c48fed"},
            {1,      "47bcbf5825d4ba95ed8d9074291fa2242cd6aabab201fd923565086c6d19b93c6936bba8def3c8e40f891288f9926eb87318c30b22611653439801d2a596a78c"},
            {52459,  "a1cfe59d6fcb93fe54a090f4d440472253b1d18962acc64a949c955ee5785c0a577bbbe8e02acdcc2d829df28357ec6ba7bb37d5026a6bd2c43e2f990b416fa9"},
            {176162, "b67128c2b592f834aaa17dad7990596f62cb85b11b3ee7072fc9f8d054366d30bed498a1d2e296920ce01ec0e32559c434a6a20329e23020ecce615ff7938c23"},
            {194590, "05ed082c5cb2c511aceb8ecd2dc785b46f77122e788e3dacbc76ecee5050b83be076758ca9aa6528e384edbf7c3ccb96ca0622fcbdd643380bd9d4dd2e293a59"},
            {262137, "4615b9c6e721f9c7b36ab317e8f125cc4edc42fabf391535da5ef15090182c2523a007d04aeca6fd1d3f926f5ae27a65400404fcdc80dd7da1d46fdaac030d06"},
            {262138, "724f2f86c24c487809dc3897acbbd32d5d791e4536aa1520e65e93891a40dde5887899ffc556cbd174f426e32ae2ab711be859601c024d1514b29a27370b662e"},
        }},
        {171, {
            {0,      "bb7f72d3813a9a9ad14e6396a3be9edba40197f8c6b60a26ca188260227f5d287616e9c93da7de35fd237c191c36cdcc00abd98dfcacd11d1f2544aa52000917"},
            {1,      "460036cc618bec3abd406c0cb53182c4b4dbbcc0266995d0f7dd3511b5a4d599405cb6cc3aff5874a7cfb87e71688ba0bb63c4aa756fd36676aa947d53c9a1b1"},
            {173473, "d5cd1e7ced40ad920d977ff3a28e11d8b42b556c44e3cf48e4cce0a0854a188182f971db468756f951ed9b76c9ecf3dbbdd0209e89febe893a88785e3c714e37"},
            {202503, "609227c6156334b0633526c032ab68d82414aee92461e60e75c5751e09e5c4d645e13e1332d2cddd993da5b5b872b18b5c26eabef39acf4fc610120685aa4b24"},
            {227478, "fb5bd4942d7da4c80dc4075e5dc121a1fb1d42a7feae844d5fe321cda1db3dbdd4035dadc5b69585f77580a61260dc36a75549995f6bcba5539da8fa726c1dd5"},
            {612347, "7a3373b5c50b8950de5172b0f2b7565ecaf00e3543de972e21122fb31505085b196c6be11738d6fce828dac744159bbd62381beddfcbd00586b8a84c6c4468c8"},
            {612348, "ec45073bd7820fe58ea29fa89375050cfb1da7bdb17b79f20f8e427bef1cdc0976d1291597fece7f538e5281a9d8df3f0b842bb691ade89d3864dfa965c7e187"},
        }},
        {2047, {
            {0,       "e887f74775c5ac3f2ed928d74dde3d8b821e9b9f251f6bb66ccc817c3c52e01b319d5cf394b0af867c871acca3375c0118ffbdafe4d7b7d23d7361cf14ed1458"},
            {1,       "9e6132d61fab636a6b95207c9d4437beabaa683d50a03a49e0fc79c1d375a067f2ced6d3be3f26e7e864a156035ad80d1a5295bf5e2be71722ed8f2708c50997"},
            {574988,  "d5f763c03e8343d4e9e55fffb45045bb25d3f9d50175453bec652e469f29419058e33bcddf8a39ba2cc84a8bf52ed58d48bcbe052b41827ed3c14b281bf088af"},
            {2778440, "d1eafd2b51d3c4485d1959ed49434c5e0684bd14e58e8b77244fca33939f64f396d205df1fb19b342f9b8605e4098ae6244d06a0a564cdd150f8e3a06681b5fb"},
            {3292172, "004aa90c2ffae1ac6c5573222a214c05a22c5d4d4db35747a945d7903aa1867ae57783cafd350fdaf23441da020403810213c3724be37e9353909d9a5283819d"},
            {4454397, "5a74ae8affebf2df925265b8467ae3f71b61814636d34bee677eeb308f28c7c18a0e6394c2c20b3026ac33386c0a2af38f92a1cc1633ea66ceefbf1e9090da57"},
            {4454398, "0504239bf95d918c1f09b9f05659376a4dd461f3b5025ab05fefd1fb6c41f08da90e38dcff6861bfe28ab3e09997b9f373f017c13f30818670c92267d5b91d55"},
        }},
    };
    // clang-format on

    for (auto& a : testCases)
    {
        const uint64_t blockNumber = a.epoch * 30000;
        const auto light = ethash_light_new(blockNumber);
        const auto items = static_cast<const h512*>(light->cache);
        const auto numItems = light->cache_size / sizeof(h512);

        for (auto& b : a.test)
        {
            BOOST_REQUIRE_LT(b.index, numItems);
            BOOST_TEST_INFO(b.index);
            BOOST_CHECK_EQUAL(toHex(items[b.index]), b.hash);
        }
    }
}

BOOST_AUTO_TEST_CASE(ethash_dag_items)
{
    // clang-format off
    std::pair<uint32_t, const char*> testCases[] = {
        {0,          "4764f1f61f71532de83e1231658fce600c5bec86ab02815caaf08f51d7217262af084f8708dcec54388404d47d88626b7d3e565efa043ac5a2bd18aeab2dd118"},
        {1,          "a629c1513c01cd6eba6db3c3f836ea94c5905615c8d3d2a608105b38affe78b39059efca6ac97e98eb2a05106fe488ea0ba7eb18e1b1ebc5c43d11241010d8e0"},
        {13,         "efce2a208a0beb57fb1789a9158da9a4eafbb5a50aa17c8ef352cf339ae1db0ddeed32e41d7a74a7a870d255714d93f906c5a6a8ff3129931d47e04ce2da5a32"},
        {171,        "973cdf0cbb58c21352d9aaf384141af3946f17016ad5b02bc1ab581e262534dc1bad6bebf91cf874733b798b3d2098dafeeb55629a6bc93cc1ec25fa6ca78927"},
        {571,        "9d9688735fb83b4f1efec9a2863bcd494f97c337b72cbbb35a86484ff3013417751547cdbc641851a02240745f671aae943654d5b54aafb258cb440fea107aa5"},
        {620,        "cf121f8cdc19828904c0ff761ee1515cc231f250dfff206b25067f597418921f1b63455376121171fb657ad21a8135d8da26ee7baf5bfc302a77e89b57e3a56a"},
        {514079,     "38f5350e9b28531f80355e47260c09bfc3f6d6da07050343fed9c08902e2347065584dc30a08228b0d2f780e4c157fffbf0b2177c546192f824ca3531632a4b7"},
        {852881,     "992879fea4525e8c44b1c2eb6a885b32a24efd4eacf6353604d4c688ba56d15c5b50c04672d583f35ac24a8509b8f925a96f5078d41e3675149742da2e992dee"},
        {6133492,    "713275b1fa7dc300093efa48b8a6047776832ee747cb80784669157d8b35d52665f28eb57f3148e9b6e46b60e47eb587d4495febf4734e6ae8387aa3e80db94e"},
        {6133493,    "009b0b6154046fc82c78d3e712924227415b0663e9c5fbb5310e3580e36861cc381dd135b24b8e746018fc9a4dcc2962fb432bbe2f5abfabc09b182f15df6b50"},
        {211322105,  "95aee5086722951be65ae6440aaebd0dda6d17cdb3057cdb99f917cfae36c6fba7be47084d90224239cce7c763196eec27dfda31bb786e284b07e540ae3f4162"},
        {740620450,  "cc65a8f1ad9d6ca3c8df787a2ff0191e05b1307fc2dbe5aff7ec2d2549ba0d4f5416f82fccb005245fef876643c840f66d9df7d596826bd22a2dc7c4952901c2"},
        {4294967295, "b47377a0c48b74caad00ea4c7e486cab7e0ffc5b7e08b67f61e93b68f3907b8a8e09b3ff57c1f7a0dc886ebb32a868926e0a066c44a53be707affaeddf7f039d"},
    };
    // clang-format on

    // Create fake light cache.
    // Number of items: 6133493 what is a prime number bigger than 4454399
    // (the num of items for 2047th light cache.
    static constexpr size_t n = 6133493;
    static constexpr size_t size = n * sizeof(h512);

    std::vector<h512> data(n);
    std::memset(data.data(), 0b1010101, size);
    ethash_light cache{data.data(), size, 0};

    for (const auto& t : testCases)
    {
        uint32_t i = t.first;
        BOOST_TEST_INFO(i);

        h512 h;
        ethash_calculate_dag_item(reinterpret_cast<node*>(&h), i, &cache);
        BOOST_CHECK_EQUAL(toHex(h), t.second);
    }
}

BOOST_AUTO_TEST_CASE(ethash_single_hash)
{
    const uint64_t n = 2683077;
    const h256 headerHash{"0313d03c5ed78694c90ecb3d04190b82d5b222c75ba4cab83383dde4d11ed512"};
    const uint64_t nonce = 0x8c5eaec000788d41;

    auto cache = ethash_light_new(n);


    auto r = ethash_light_compute(cache, *(ethash_h256*)&headerHash, nonce);
    auto mixHash = *(h256*)&r.mix_hash;
    auto resultHash = *(h256*)&r.result;

    BOOST_CHECK_EQUAL(
        mixHash.hex(), "93e85c97b34ccd8091e09ddb513fdc9e680fa8898d4a0737205e60af710a3dcb");
    BOOST_CHECK_EQUAL(
        resultHash.hex(), "00000000000204882a6213f68fe89bc368df25c1ad999f82532a7433e99bc48e");
}

BOOST_AUTO_TEST_CASE(epochSeed)
{
    BlockHeader header;
    header.setNumber(0);
    h256 seed = Ethash::seedHash(header);
    BOOST_CHECK_EQUAL(seed, h256{});

    header.setNumber(30000);
    seed = Ethash::seedHash(header);
    BOOST_CHECK_EQUAL(
        seed, h256{"290decd9548b62a8d60345a988386fc84ba6bc95484008f6362f93160ef3e563"});

    header.setNumber(2048 * 30000);
    seed = Ethash::seedHash(header);
    BOOST_CHECK_EQUAL(
        seed, h256{"20a7678ca7b50829183baac2e1e3c43fa3c4bcbc171b11cf5a9f30bebd172920"});
}

BOOST_AUTO_TEST_CASE(etashQuickVerify)
{
    BlockHeader header;
    header.setParentHash(h256{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"});
    header.setDifficulty(h256{"0000000000000000000000000000000000000000000000000000000000ffffff"});
    header.setGasLimit(1000000);
    Ethash::setMixHash(header, {});

    Ethash etash;
    Ethash::setNonce(header, Nonce{3272400});
    etash.verify(QuickNonce, header, {}, {});
    Ethash::setNonce(header, Nonce{3272401});
    BOOST_CHECK_THROW(etash.verify(QuickNonce, header, {}, {}), InvalidBlockNonce);
}

BOOST_AUTO_TEST_CASE(etashVerify)
{
    BlockHeader header;
    header.setParentHash(h256{"aff00eb20f8a48450b9ea5307e2737287854f357c9022280772e995cc22affd3"});
    header.setAuthor(Address{"8888f1f195afa192cfee860698584c030f4c9db1"});
    header.setRoots(h256{"fcfe9f2203bd98342867117fa3de299a09578371efd04fc9e76a46f7f1fda4bb"},
        h256{"1751f772ba1fdb3ad31fa04c39144ea3b523f10604a5a09a19cb4c1d0b56992c"},
        h256{"1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347"},
        h256{"1cd69d76c84ea914e746833b7a31d9bfe210f75929893f1da0748efaeb31fe27"});
    header.setLogBloom({});
    header.setDifficulty(h256{131072});
    header.setNumber(1);
    header.setGasLimit(3141562);
    header.setGasUsed(55179);
    header.setTimestamp(1507291743);

    BOOST_CHECK_EQUAL(header.hash(WithoutSeal),
        h256{"57c5cfb8fe8a70a24ea81f12398e8a074ac25dd32b6dba8cd1f2bf85680fbfce"});

    Ethash::setMixHash(
        header, h256{"d8ada7ff7720ebc9700c170c046352b8ee6fb4630cf6a285489896daac7a40eb"});

    Ethash etash;
    Ethash::setNonce(header, Nonce{"81c3f9bfae230a8e"});
    etash.verify(CheckEverything, header, {}, {});

    // Break nonce.
    Ethash::setNonce(header, Nonce{"71c3f9bfae230a8e"});
    try
    {
        etash.verify(CheckEverything, header, {}, {});
        BOOST_CHECK(false);
    }
    catch (InvalidBlockNonce const& ex)
    {
        std::tuple<h256, h256> ethashResult = *boost::get_error_info<errinfo_ethashResult>(ex);
        BOOST_CHECK_EQUAL(std::get<0>(ethashResult),
            h256{"07a4017237d933aa1ff4f62650f68ea2118c8bd741575e97c2867fb41d5b832d"});
        BOOST_CHECK_EQUAL(std::get<1>(ethashResult),
            h256{"a842d613f0b8ad1266e507bb1845b2db75673caf593596d1de4951ecd9620a93"});
    }
    Ethash::setNonce(header, Nonce{"81c3f9bfae230a8e"});

    // Break mix hash.
    Ethash::setMixHash(
        header, h256{"e8ada7ff7720ebc9700c170c046352b8ee6fb4630cf6a285489896daac7a40eb"});
    BOOST_CHECK_THROW(etash.verify(CheckEverything, header, {}, {}), InvalidBlockNonce);

    // Break nonce & mix hash.
    Ethash::setNonce(header, Nonce{"71c3f9bfae230a8e"});
    BOOST_CHECK_THROW(etash.verify(CheckEverything, header, {}, {}), InvalidBlockNonce);
    Ethash::setNonce(header, Nonce{"81c3f9bfae230a8e"});
    Ethash::setMixHash(
        header, h256{"d8ada7ff7720ebc9700c170c046352b8ee6fb4630cf6a285489896daac7a40eb"});

    // Recheck the valid header.
    etash.verify(CheckEverything, header, {}, {});
}

namespace
{
struct EthashTestCase
{
    char const* nonce;
    char const* mixHash;
    char const* header;
    char const* seed;
    char const* result;
    int cacheSize;
    int fullSize;
    char const* headerHash;
    char const* cacheHash;
};

EthashTestCase ethashTestCases[] = {
    {
        "4242424242424242",
        "58f759ede17a706c93f13030328bcea40c1d1341fb26f2facd21ceb0dae57017",
        "f901f3a00000000000000000000000000000000000000000000000000000000000000000a01dcc4de8dec75d7a"
        "ab85b567b6ccd41ad312451b948a7413f0a142fd40d49347940000000000000000000000000000000000000000"
        "a09178d0f23c965d81f0834a4c72c6253ce6830f4022b1359aaebfc1ecba442d4ea056e81f171bcc55a6ff8345"
        "e692c0f86e5b48e01b996cadc001622fb5e363b421a056e81f171bcc55a6ff8345e692c0f86e5b48e01b996cad"
        "c001622fb5e363b421b90100000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000008302"
        "000080830f4240808080a058f759ede17a706c93f13030328bcea40c1d1341fb26f2facd21ceb0dae570178842"
        "42424242424242",
        "0000000000000000000000000000000000000000000000000000000000000000",
        "dd47fd2d98db51078356852d7c4014e6a5d6c387c35f40e2875b74a256ed7906",
        16776896,
        1073739904,
        "2a8de2adf89af77358250bf908bf04ba94a6e8c3ba87775564a41d269a05e4ce",
        "35ded12eecf2ce2e8da2e15c06d463aae9b84cb2530a00b932e4bbc484cde353",
    },
    {
        "307692cf71b12f6d",
        "e55d02c555a7969361cf74a9ec6211d8c14e4517930a00442f171bdb1698d175",
        "f901f7a01bef91439a3e070a6586851c11e6fd79bbbea074b2b836727b8e75c7d4a6b698a01dcc4de8dec75d7a"
        "ab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794ea3cb5f94fa2ddd52ec6dd6eb75cf824f4058ca1"
        "a00c6e51346be0670ce63ac5f05324e27d20b180146269c5aab844d09a2b108c64a056e81f171bcc55a6ff8345"
        "e692c0f86e5b48e01b996cadc001622fb5e363b421a056e81f171bcc55a6ff8345e692c0f86e5b48e01b996cad"
        "c001622fb5e363b421b90100000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000008302"
        "004002832fefd880845511ed2a80a0e55d02c555a7969361cf74a9ec6211d8c14e4517930a00442f171bdb1698"
        "d17588307692cf71b12f6d",
        "0000000000000000000000000000000000000000000000000000000000000000",
        "ab9b13423cface72cbec8424221651bc2e384ef0f7a560e038fc68c8d8684829",
        16776896,
        1073739904,
        "100cbec5e5ef82991290d0d93d758f19082e71f234cf479192a8b94df6da6bfe",
        "35ded12eecf2ce2e8da2e15c06d463aae9b84cb2530a00b932e4bbc484cde353",
    },
};
}  // namespace

BOOST_AUTO_TEST_CASE(ethashEvalHeader)
{
    // FIXME: Drop this test as ethash library has this test cases in its test suite.

    for (auto& t : ethashTestCases)
    {
        BlockHeader header{fromHex(t.header), HeaderData};
        h256 headerHash{t.headerHash};
        eth::Nonce nonce{t.nonce};
        BOOST_REQUIRE_EQUAL(headerHash, header.hash(WithoutSeal));
        BOOST_REQUIRE_EQUAL(nonce, Ethash::nonce(header));

        ethash::result result = ethash::hash(
            ethash::get_global_epoch_context(ethash::get_epoch_number(header.number())),
            ethash::hash256_from_bytes(header.hash(WithoutSeal).data()), (uint64_t)(u64)nonce);

        h256 mix{result.mix_hash.bytes, h256::ConstructFromPointer};
        h256 final{result.final_hash.bytes, h256::ConstructFromPointer};

        BOOST_REQUIRE_EQUAL(final, h256{t.result});
        BOOST_REQUIRE_EQUAL(mix, Ethash::mixHash(header));
    }
}

BOOST_AUTO_TEST_SUITE_END()
