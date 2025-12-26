#include "config.h"
#include <assert.h>
#include <bitcoin/chainparams.h>
#include <ccan/array_size/array_size.h>
#include <ccan/tal/str/str.h>

/* Version codes for BIP32 extended keys in libwally-core.
 * Stolen from wally_bip32.h in libwally-core*/
#define BIP32_VER_MAIN_PUBLIC  0x0488B21E
#define BIP32_VER_MAIN_PRIVATE 0x0488ADE4
#define BIP32_VER_TEST_PUBLIC  0x043587CF
#define BIP32_VER_TEST_PRIVATE 0x04358394
#define BIP32_VER_SIGT_PUBLIC  0x043587CF
#define BIP32_VER_SIGT_PRIVATE 0x04358394

static u8 liquid_fee_asset[] = {
    0x01, 0x6d, 0x52, 0x1c, 0x38, 0xec, 0x1e, 0xa1, 0x57, 0x34, 0xae,
    0x22, 0xb7, 0xc4, 0x60, 0x64, 0x41, 0x28, 0x29, 0xc0, 0xd0, 0x57,
    0x9f, 0x0a, 0x71, 0x3d, 0x1c, 0x04, 0xed, 0xe9, 0x79, 0x02, 0x6f,
};

static u8 liquid_regtest_fee_asset[] = {
    0x01, 0x5c, 0xe7, 0xb9, 0x63, 0xd3, 0x7f, 0x8f, 0x2d, 0x51, 0xca,
    0xfb, 0xba, 0x92, 0x8a, 0xaa, 0x9e, 0x22, 0x0b, 0x8b, 0xbc, 0x66,
    0x05, 0x71, 0x49, 0x9c, 0x03, 0x62, 0x8a, 0x38, 0x51, 0xb8, 0xce,
};

const struct chainparams networks[] = {
    /* Bitcoin-PoCX Mainnet
     * Genesis: 23ce719791691b22527f148bc454fdaf77228168d7757891ada83c893dd3259a
     * Block time: 120 seconds, Initial subsidy: 10 BTC, Halving: 1,050,000 blocks
     */
    {.network_name = "pocx",
     .onchain_hrp = "pocx",
     .lightning_hrp = "pocx",
     .bip70_name = "main",
     .genesis_blockhash = {{{.u.u8 = {0x9a, 0x25, 0xd3, 0x3d, 0x89, 0x3c, 0xa8,
				      0xad, 0x91, 0x78, 0x75, 0xd7, 0x68, 0x81,
				      0x22, 0x77, 0xaf, 0xfd, 0x54, 0xc4, 0x8b,
				      0x14, 0x7f, 0x52, 0x22, 0x1b, 0x69, 0x91,
				      0x97, 0x71, 0xce, 0x23}}}},
     .rpc_port = 8332,
     .ln_port = 9735,
     .cli = "bitcoin-cli",
     .cli_args = NULL,
     .cli_min_supported_version = 300000,
     .dust_limit = { 546 },
     /* BOLT #2:
      *
      * The sending node:
      *...
      *   - MUST set `funding_satoshis` to less than 2^24 satoshi.
      */
     .max_funding = AMOUNT_SAT_INIT((1 << 24) - 1),
     .max_payment = AMOUNT_MSAT_INIT(0xFFFFFFFFULL),
     .max_supply = AMOUNT_SAT_INIT(2100000000000000),
     .when_lightning_became_cool = 1,
     .p2pkh_version = 85,
     .p2sh_version = 90,
     .testnet = false,
     .fee_asset_tag = NULL,
     .bip32_key_version = {.bip32_pubkey_version = BIP32_VER_MAIN_PUBLIC,
			   .bip32_privkey_version = BIP32_VER_MAIN_PRIVATE},
     .is_elements = false},
    /* Bitcoin-PoCX Regtest
     * Genesis: c67c27870117080a961530416a25c1ba7da1cd593f7f2101c261222a74d5a51e
     * Block time: 1 second (instant mining for testing)
     */
    {.network_name = "regtest",
     .onchain_hrp = "rpocx",
     .lightning_hrp = "rpocx",
     .bip70_name = "regtest",
     .genesis_blockhash = {{{.u.u8 = {0x1e, 0xa5, 0xd5, 0x74, 0x2a, 0x22, 0x61,
				      0xc2, 0x01, 0x21, 0x2f, 0x3f, 0x59, 0xcd,
				      0xa1, 0x7d, 0xba, 0xc1, 0x25, 0x6a, 0x41,
				      0x30, 0x15, 0x96, 0x0a, 0x08, 0x17, 0x01,
				      0x87, 0x27, 0x7c, 0xc6}}}},
     .rpc_port = 18443,
     .ln_port = 19846,
     .cli = "bitcoin-cli",
     .cli_args = "-regtest",
     .cli_min_supported_version = 300000,
     .dust_limit = { 546 },
     .max_funding = AMOUNT_SAT_INIT((1 << 24) - 1),
     .max_payment = AMOUNT_MSAT_INIT(0xFFFFFFFFULL),
     .max_supply = AMOUNT_SAT_INIT(2100000000000000),
     .when_lightning_became_cool = 1,
     .p2pkh_version = 111,
     .p2sh_version = 196,
     .testnet = true,
     .fee_asset_tag = NULL,
     .bip32_key_version = {.bip32_pubkey_version = BIP32_VER_TEST_PUBLIC,
			   .bip32_privkey_version = BIP32_VER_TEST_PRIVATE},
     .is_elements = false},
    /* Bitcoin-PoCX Testnet
     * Genesis: 181c51a172fe20c203e463f6f203b7d9be388fa0f1282e507192f94d24a57e81
     * Block time: 120 seconds
     */
    {.network_name = "testnet",
     .onchain_hrp = "tpocx",
     .lightning_hrp = "tpocx",
     .bip70_name = "test",
     .genesis_blockhash = {{{.u.u8 = {0x81, 0x7e, 0xa5, 0x24, 0x4d, 0xf9, 0x92,
				      0x71, 0x50, 0x2e, 0x28, 0xf1, 0xa0, 0x8f,
				      0x38, 0xbe, 0xd9, 0xb7, 0x03, 0xf2, 0xf6,
				      0x63, 0xe4, 0x03, 0xc2, 0x20, 0xfe, 0x72,
				      0xa1, 0x51, 0x1c, 0x18}}}},
     .rpc_port = 18332,
     .ln_port = 19735,
     .cli = "bitcoin-cli",
     .cli_args = "-testnet",
     .cli_min_supported_version = 300000,
     .dust_limit = { 546 },
     .max_funding = AMOUNT_SAT_INIT((1 << 24) - 1),
     .max_payment = AMOUNT_MSAT_INIT(0xFFFFFFFFULL),
     .max_supply = AMOUNT_SAT_INIT(2100000000000000),
     .when_lightning_became_cool = 1,
     .p2pkh_version = 127,
     .p2sh_version = 132,
     .testnet = true,
     .fee_asset_tag = NULL,
     .bip32_key_version = {.bip32_pubkey_version = BIP32_VER_TEST_PUBLIC,
			   .bip32_privkey_version = BIP32_VER_TEST_PRIVATE},
     .is_elements = false},
    {.network_name = "liquid-regtest",
     .onchain_hrp = "ert",
     .lightning_hrp = "ert",
     .bip70_name = "liquid-regtest",
     .genesis_blockhash = {{{.u.u8 = {0x9f, 0x87, 0xeb, 0x58, 0x0b, 0x9e, 0x5f,
				      0x11, 0xdc, 0x21, 0x1e, 0x9f, 0xb6, 0x6a,
				      0xbb, 0x36, 0x99, 0x99, 0x90, 0x44, 0xf8,
				      0xfe, 0x14, 0x68, 0x01, 0x16, 0x23, 0x93,
				      0x36, 0x42, 0x86, 0xc6}}}},
     .rpc_port = 19332,
     .ln_port = 20735,
     .cli = "elements-cli",
     .cli_args = "-chain=liquid-regtest",
     .dust_limit = {546},
     .max_funding = AMOUNT_SAT_INIT((1 << 24) - 1),
     .max_payment = AMOUNT_MSAT_INIT(0xFFFFFFFFULL),
     .max_supply = AMOUNT_SAT_INIT(2100000000000000),
     .when_lightning_became_cool = 1,
     .p2pkh_version = 91,
     .p2sh_version = 75,
     .testnet = true,
     .fee_asset_tag = liquid_regtest_fee_asset,
     .bip32_key_version = {.bip32_pubkey_version = BIP32_VER_TEST_PUBLIC,
			   .bip32_privkey_version = BIP32_VER_TEST_PRIVATE},
     .is_elements = true},
    {.network_name = "liquid",
     .onchain_hrp = "ex",
     .lightning_hrp = "ex",
     .bip70_name = "liquidv1",
     .genesis_blockhash = {{{.u.u8 = {0x14, 0x66, 0x27, 0x58, 0x36, 0x22, 0x0d,
				      0xb2, 0x94, 0x4c, 0xa0, 0x59, 0xa3, 0xa1,
				      0x0e, 0xf6, 0xfd, 0x2e, 0xa6, 0x84, 0xb0,
				      0x68, 0x8d, 0x2c, 0x37, 0x92, 0x96, 0x88,
				      0x8a, 0x20, 0x60, 0x03}}}},
     .rpc_port = 7041,
     .ln_port = 9735,
     .cli = "elements-cli",
     .cli_args = "-chain=liquidv1",
     .dust_limit = {546},
     .max_funding = AMOUNT_SAT_INIT((1 << 24) - 1),
     .max_payment = AMOUNT_MSAT_INIT(0xFFFFFFFFULL),
     .max_supply = AMOUNT_SAT_INIT(2100000000000000),
     .when_lightning_became_cool = 1,
     .p2pkh_version = 57,
     .p2sh_version = 39,
     .testnet = false,
     .fee_asset_tag = liquid_fee_asset,
     .bip32_key_version = {.bip32_pubkey_version = BIP32_VER_MAIN_PUBLIC,
			   .bip32_privkey_version = BIP32_VER_MAIN_PRIVATE},
     .is_elements = true},
};

const struct chainparams *chainparams_for_network(const char *network_name)
{
	for (size_t i = 0; i < ARRAY_SIZE(networks); i++) {
		if (streq(network_name, networks[i].network_name)) {
			return &networks[i];
		}
	}
	return NULL;
}

const struct chainparams *chainparams_by_chainhash(const struct bitcoin_blkid *chain_hash)
{
	for (size_t i = 0; i < ARRAY_SIZE(networks); i++) {
		if (bitcoin_blkid_eq(chain_hash, &networks[i].genesis_blockhash)) {
			return &networks[i];
		}
	}
	return NULL;
}

const struct chainparams *chainparams_by_lightning_hrp(const char *lightning_hrp)
{
	for (size_t i = 0; i < ARRAY_SIZE(networks); i++) {
		if (streq(lightning_hrp, networks[i].lightning_hrp)) {
			return &networks[i];
		}
	}
	return NULL;
}

const char *chainparams_get_network_names(const tal_t *ctx)
{
    char *networks_string = tal_strdup(ctx, networks[0].network_name);
    for (size_t i = 1; i < ARRAY_SIZE(networks); ++i)
        tal_append_fmt(&networks_string, ", %s", networks[i].network_name);
    return networks_string;
}

int chainparams_get_ln_port(const struct chainparams *params)
{
	assert(params);
	return params->ln_port;
}
