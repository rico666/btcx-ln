# BTCX Lightning: Lightning Network for Bitcoin-PoCX

BTCX Lightning is a fork of [Core Lightning (CLN)](https://github.com/ElementsProject/lightning) adapted for **Bitcoin-PoCX** (Proof of Capacity neXt generation consensus).

This implementation provides a lightweight, highly customizable, and [BOLT-compliant][std] Lightning Network for the Bitcoin-PoCX blockchain.

* [Getting Started](#getting-started)
    * [Prerequisites](#prerequisites)
    * [Installation](#installation)
    * [Starting lightningd](#starting-lightningd)
    * [Using the JSON-RPC Interface](#using-the-json-rpc-interface)
    * [Opening A Channel](#opening-a-channel)
    * [Sending and Receiving Payments](#sending-and-receiving-payments)
    * [Configuration File](#configuration-file)
* [PoCX-Specific Information](#pocx-specific-information)
    * [Supported Networks](#supported-networks)
    * [Block Time Considerations](#block-time-considerations)
    * [Address Formats](#address-formats)
* [Further Information](#further-information)
    * [Pruning](#pruning)
    * [HD wallet encryption](#hd-wallet-encryption)
    * [Developers](#developers)

## Project Status

This is an adaptation of Core Lightning for Bitcoin-PoCX. The upstream CLN implementation has been in production use since early 2018.

**Current Status**: Testnet phase - we recommend testing on `regtest` or `testnet` before mainnet use.

## Community

Join our [Discord server](https://discord.gg/fwPwvG9n73) for discussion, support, and updates.

## Getting Started

### Prerequisites

BTCX Lightning requires:

- **Bitcoin-PoCX node** (v30.0 or above) fully synced with the network
- RPC access enabled on your Bitcoin-PoCX node (with `server=1`)
- Linux or macOS operating system

Your Bitcoin-PoCX node should relay transactions (do not use `blocksonly=1`).

### Installation

Build from source:

```bash
git clone https://github.com/rico666/btcx-ln.git
cd btcx-ln
./configure
make
sudo make install
```

### Starting `lightningd`

#### Regtest (Development/Testing)

For development and testing, use regtest mode with a local Bitcoin-PoCX node:

```bash
# Start Bitcoin-PoCX in regtest mode
bitcoind -regtest -daemon -miningserver

# Start lightningd
lightningd --network=regtest --log-level=debug
```

There's also a helper script for setting up a test network:

```bash
. contrib/startup_regtest.sh
```

#### Testnet

For testing with real (but valueless) coins:

```bash
# Start Bitcoin-PoCX testnet node
bitcoind -testnet -daemon

# Wait for sync, then start lightningd
lightningd --network=testnet --log-level=debug
```

#### Mainnet

For production use with real funds:

```bash
# Start Bitcoin-PoCX mainnet node
bitcoind -daemon

# Wait for full sync, then start lightningd
lightningd --network=pocx --log-level=debug
```

This creates a `.lightning/` subdirectory in your home directory. See `man lightningd` for more options.

### Using The JSON-RPC Interface

BTCX Lightning exposes a [JSON-RPC 2.0][jsonrpcspec] interface over a Unix Domain socket. Use `lightning-cli` to access it:

```bash
lightning-cli help              # List all commands
lightning-cli help <command>    # Help for specific command
```

Useful commands:

* `newaddr`: Get a Bitcoin-PoCX address to deposit funds
* `listfunds`: See your on-chain and channel funds
* `connect`: Connect to another lightning node
* `fundchannel`: Open a channel to a connected node
* `invoice`: Create an invoice to receive payment
* `pay`: Pay someone else's invoice
* `plugin`: Manage plugins

### Opening A Channel

First, deposit funds to your lightning node:

```bash
# Get a deposit address (returns pocx1q... address on mainnet)
lightning-cli newaddr

# Or get a taproot address
lightning-cli newaddr p2tr
```

Send Bitcoin-PoCX to this address and wait for confirmation. Check your funds:

```bash
lightning-cli listfunds
```

Connect to a peer and open a channel:

```bash
lightning-cli connect <node_id> <ip> [<port>]
lightning-cli fundchannel <node_id> <amount_in_satoshis>
```

The channel requires 3 confirmations to be usable (~6 minutes on PoCX), and 6 confirmations to be publicly announced (~12 minutes).

### Sending and Receiving Payments

Create an invoice to receive payment:

```bash
lightning-cli invoice <amount_msat> <label> <description>
```

This returns a `bolt11` invoice string. The payer uses:

```bash
lightning-cli pay <bolt11>
```

## Configuration File

Create `~/.lightning/config` or `~/.lightning/pocx/config`:

```ini
# Network (pocx, testnet, or regtest)
network=pocx

# Bitcoin-PoCX RPC connection
bitcoin-rpcuser=your_username
bitcoin-rpcpassword=your_password
bitcoin-rpcconnect=127.0.0.1
bitcoin-rpcport=8332

# Logging
log-level=info

# Recommended for PoCX's 2-minute blocks (see below)
cltv-delta=180
cltv-final=90
```

A sample configuration is at `contrib/config-example`.

## PoCX-Specific Information

### Supported Networks

| Network | Command | Bech32 HRP | Address Example | LN Port |
|---------|---------|------------|-----------------|---------|
| Mainnet | `--network=pocx` | `pocx` | `pocx1q...` | 9735 |
| Testnet | `--network=testnet` | `tpocx` | `tpocx1q...` | 19735 |
| Regtest | `--network=regtest` | `rpocx` | `rpocx1q...` | 19846 |

### Block Time Considerations

Bitcoin-PoCX has **2-minute blocks** (vs Bitcoin's 10-minute blocks). This affects timing parameters:

| Blocks | Bitcoin Time | PoCX Time | Notes |
|--------|-------------|-----------|-------|
| 6 | ~60 min | ~12 min | Channel announcement |
| 144 | ~1 day | ~4.8 hours | - |
| 720 | ~5 days | ~1 day | - |
| 1008 | ~1 week | ~1.4 days | - |

**Recommended adjustments** for equivalent security windows:

```ini
# CLTV delta: blocks between incoming/outgoing HTLCs
# Default 34 = ~68 min on PoCX; use 180 for ~6 hours
cltv-delta=180

# Final CLTV: minimum blocks before HTLC timeout
# Default 18 = ~36 min on PoCX; use 90 for ~3 hours
cltv-final=90
```

### Address Formats

| Network | P2PKH Version | P2SH Version | Bech32 Prefix |
|---------|---------------|--------------|---------------|
| Mainnet | 85 (P...) | 90 (R...) | pocx1 |
| Testnet | 127 | 132 | tpocx1 |
| Regtest | 111 (m/n) | 196 (2) | rpocx1 |

### Port Reference

| Network | Bitcoin-PoCX P2P | Bitcoin-PoCX RPC | Lightning P2P |
|---------|------------------|------------------|---------------|
| Mainnet | 8888 | 8332 | 9735 |
| Testnet | 18888 | 18332 | 19735 |
| Regtest | 18444 | 18443 | 19846 |

## Further Information

### Pruning

BTCX Lightning requires JSON-RPC access to a synchronized Bitcoin-PoCX node. ZeroMQ is not required, and `txindex` is not needed.

If using a pruned node, monitor the gap between Lightning's blockheight (`lightning-cli getinfo`) and Bitcoin-PoCX's blockheight (`bitcoin-cli getblockchaininfo`). If they drift apart, Lightning may not be able to sync.

### HD wallet encryption

Encrypt your `hsm_secret` for additional security:

```bash
lightningd --encrypted-hsm
```

Or use `lightning-hsmtool` in the `tools/` directory. **Warning**: Without your password, you cannot access your funds.

### Developers

This is a fork of Core Lightning. For development:

- Upstream CLN: https://github.com/ElementsProject/lightning
- BOLT specifications: https://github.com/lightning/bolts
- CLN documentation: https://docs.corelightning.org/docs

## Changes from Upstream CLN

Key modifications for Bitcoin-PoCX:

1. **`bitcoin/chainparams.c`**: PoCX network definitions with correct genesis hashes, address versions, and Bech32 HRPs
2. **Minimum bitcoind version**: v30.0 (Bitcoin-PoCX)

All other functionality (channels, payments, routing, plugins) works unchanged.

## License

BSD-MIT - see [LICENSE](LICENSE).

## Upstream Credits

Based on [Core Lightning](https://github.com/ElementsProject/lightning) by Blockstream and contributors.

[std]: https://github.com/lightning/bolts
[jsonrpcspec]: https://www.jsonrpc.org/specification
