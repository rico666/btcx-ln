# Backporting Upstream Core Lightning Features

BTCX Lightning is a fork of [Core Lightning (CLN)](https://github.com/ElementsProject/lightning) adapted for the Bitcoin-PoCX blockchain. This document describes the process for incorporating upstream CLN changes.

## Repository Setup

### Add Upstream Remote (One-Time)

```bash
cd btcx-ln
git remote add upstream https://github.com/ElementsProject/lightning.git
git fetch upstream
```

The upstream data is stored in `.git/` - no separate directory needed.

### Verify Setup

```bash
git remote -v
# origin    https://github.com/rico666/btcx-ln.git (fetch)
# origin    https://github.com/rico666/btcx-ln.git (push)
# upstream  https://github.com/ElementsProject/lightning.git (fetch)
# upstream  https://github.com/ElementsProject/lightning.git (push)
```

## Branch Strategy

| Branch | Purpose |
|--------|---------|
| `main` | Stable BTCX Lightning releases |
| `develop` | Integration branch for testing |
| `backport/*` | Individual upstream backports |
| `feature/*` | BTCX-specific features |

## Backporting Methods

### Method 1: Cherry-Pick (Recommended for Most Cases)

Use for: bug fixes, security patches, isolated features.

```bash
# Fetch latest upstream
git fetch upstream

# Create backport branch from develop
git checkout -b backport/feature-name develop

# Cherry-pick specific commit(s)
git cherry-pick <commit-hash>

# Or cherry-pick a range
git cherry-pick <start-hash>^..<end-hash>

# Resolve any conflicts (see below)
# Test the changes
# Push and create PR
git push origin backport/feature-name
```

### Method 2: Merge Release Tag

Use for: major version upgrades, catching up after long gaps.

```bash
git fetch upstream --tags
git checkout -b backport/v25.12 develop
git merge v25.12 --no-commit

# Review all changes carefully
git diff --cached

# Resolve conflicts, test thoroughly
git commit -m "Merge upstream CLN v25.12"
```

## Known Conflict Points

These files are customized for Bitcoin-PoCX and will almost always conflict:

| File | Reason |
|------|--------|
| `bitcoin/chainparams.c` | PoCX network definitions (genesis, ports, HRPs) |
| `README.md` | BTCX-specific documentation |
| `SECURITY.md` | PoCX security contacts |
| `CHANGELOG.md` | Fork header |
| `doc/getting-started/home.md` | BTCX branding |
| `doc/contribute-to-core-lightning/security-policy.md` | PoCX contacts |
| `contrib/config-example` | PoCX network example |

### Resolving Conflicts

For the files above, **always keep the BTCX version** of:
- Network names (`pocx`, `tpocx`, `rpocx`)
- Genesis hashes
- Address versions and Bech32 HRPs
- Port numbers
- Security contacts
- Discord/GitHub links

Example conflict resolution for `chainparams.c`:
```bash
# After conflict, keep our PoCX networks, accept upstream structural changes
git checkout --ours bitcoin/chainparams.c
# Then manually merge any new fields/features from upstream
```

## Testing Backported Changes

Before merging any backport:

1. **Build test**: `./configure && make`
2. **Unit tests**: `make check`
3. **Regtest**: Test basic operations on regtest
4. **Integration**: If touching consensus-critical code, test against Bitcoin-PoCX testnet

## Documentation Policy

Most upstream documentation (build instructions, API docs, plugin guides) applies unchanged to BTCX Lightning since:
- The `bitcoind` binary name is the same
- Core Lightning functionality is identical
- Only network parameters differ

**Do not modify** upstream docs unless they contain Bitcoin-specific information that would confuse BTCX users (like block explorer links or network-specific instructions).

## Upstream Release Tracking

Monitor upstream releases:
- GitHub: https://github.com/ElementsProject/lightning/releases
- Changelog: https://github.com/ElementsProject/lightning/blob/master/CHANGELOG.md

For security-critical updates, prioritize immediate backporting.

## Quick Reference

```bash
# Fetch upstream
git fetch upstream

# See upstream tags
git tag -l

# See what changed between versions
git log --oneline v25.08..v25.12 --

# See commits touching a specific file
git log upstream/master -- bitcoin/chainparams.c

# Preview a merge without committing
git merge upstream/v25.12 --no-commit --no-ff
git merge --abort  # if you want to cancel
```

## Questions?

Join our [Discord](https://discord.gg/fwPwvG9n73) or open an issue on [GitHub](https://github.com/rico666/btcx-ln).
