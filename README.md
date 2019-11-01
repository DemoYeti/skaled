<div align="center">
  <img src="https://uploads-ssl.webflow.com/5be05ae542686c4ebf192462/5be2f8beb08f6d0fbd2ea797_Skale_Logo_Blue-p-500.png"><br><br>
</div>

-----------------

# SKALED – SKALE C++ Client

[![Discord](https://img.shields.io/discord/534485763354787851.svg)](https://discord.gg/vvUtWJB)

Skaled is SKALE Proof-Of-Stake blockchain client, compatible with ETH ecocystem, including EVM, Solidity, Metamask and Truffle. It uses [SKALE BFT Consensus engine](https://github.com/skalenetwork/skale-consensus).  It is currently actively developed and maintained by SKALE Labs, and intended to be used for SKALE chains (elastic sidechains).

## Forklessness

Skaled is forkless, meaning that blockchain a linear chain (and not a tree of forks as with ETH 1.0). Every block is provably finalized within finite time.


## Asynchronous block production

Skaled is asynchronous, meaning that the consensus on the next block starts immediately after the previous block is finalized.  There is no set block time interval. This allows for subsecond block production in case of a fast network, enabling interactive Dapps.

## Provable security

Skaled is the only provably secure ETH compatible PoS client. Security is proven under assumption of maximum t malicious nodes, where the total number of nodes N is more or equal 3t + 1.

## Survivability

The network is assumed to bef fully asynchronous meaning that there is no upper limit for the packet delivery time. In case of a temporarily network split, the protocol can wait indefinitely long until the split is resolved and then resume normal block production.

##  Historic origins

Historically skaled started by forking [Aleth](https://github.com/ethereum/aleth) (formerly known as the [cpp-ethereum](http://www.ethdocs.org/en/latest/ethereum-clients/cpp-ethereum/) project). We are thankful to the original cpp-ethereum team for their contributions.


## Building from source


### OS requirements

Skaled builds and runs on Ubuntu 16.04 and 18.04

### Clone repository

```
git clone --recurse-submodules https://github.com/skalenetwork/skaled.git
cd skaled
```

⚠️ Note: Because this repository depends on additional submodules, it is important to pass`--recurse-submodules` to the `git clone` command.

If you have already cloned the repo and forgot to pass `--recurse-submodules`, execute `git submodule update --init --recursive`

### Install required Ubuntu packages

```
sudo apt-get update
sudo apt-get install autoconf build-essential cmake libprocps-dev libtool texinfo wget yasm flex bison
```

### Build dependencies

```
cd SkaleDeps
./build.sh
```

### Configure and build skaled


```shell
# Configure the project and create a build directory.
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
# Build all default targets using all cores.
cmake --build build -- -j$(nproc)
```

Note: Currently only Debug build is supported.


## Testing

To run the tests:

```
cd build/test
./testeth -- --all
```

## Documentation

_in process_

## Contributing

We are actively looking for contributors and have great bounties!

**Please read [CONTRIBUTING](CONTRIBUTING.md) and [CODING_STYLE](CODING_STYLE.md) thoroughly before making alterations to the code base. This project adheres to SKALE's code of conduct. By participating, you are expected to uphold this code.**

**We use GitHub issues for tracking requests and bugs, so please see our general development questions and discussion on [Discord](https://discord.gg/vvUtWJB).**

All contributions are welcome! We try to keep a list of tasks that are suitable for newcomers under the tag [help wanted](https://github.com/skalenetwork/skaled/labels/help%20wanted). If you have any questions, please just ask.

[![Discord](https://img.shields.io/discord/534485763354787851.svg)](https://discord.gg/vvUtWJB)


All development goes in develop branch.

## Note on mining

The SKALE Network uses Proof-of-Stake, therefore this project is **not suitable for Ethereum mining**.


## For more information
* [SKALE Labs Website](https://skalelabs.com)
* [SKALE Labs Twitter](https://twitter.com/skalelabs)
* [SKALE Labs Blog](https://medium.com/skale)

Learn more about the SKALE community over on [Discord](https://discord.gg/vvUtWJB).


## License

[![License](https://img.shields.io/github/license/skalenetwork/skaled.svg)](LICENSE)

All contributions are made under the [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0.en.html). See [LICENSE](LICENSE).

All original cpp-ethereum code Copyright (C) Aleth Authors.  
All cpp-ethereum modifications Copyright (C) SKALE Labs.  
All skaled code Copyright (C) SKALE Labs.
