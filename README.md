Lava
===============

[![Financial Contributors on Open Collective](https://opencollective.com/lava/all/badge.svg?label=financial+contributors)](https://opencollective.com/lava) [![latest-release](https://img.shields.io/github/release/lavaofficial/lava)](https://github.com/Lavacoreteam/Lava/releases)
[![GitHub last-release](https://img.shields.io/github/release-date/lavaofficial/lava)](https://github.com/Lavacoreteam/Lava/releases)
[![GitHub downloads](https://img.shields.io/github/downloads/lavaofficial/lava/total)](https://github.com/Lavacoreteam/Lava/releases)
[![GitHub commits-since-last-version](https://img.shields.io/github/commits-since/lavaofficial/lava/latest/master)](https://github.com/Lavacoreteam/Lava/graphs/commit-activity)
[![GitHub commits-per-month](https://img.shields.io/github/commit-activity/m/lavaofficial/lava)](https://github.com/Lavacoreteam/Lava/graphs/code-frequency)
[![GitHub last-commit](https://img.shields.io/github/last-commit/lavaofficial/lava)](https://github.com/Lavacoreteam/Lava/commits/master)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/lavaofficial/lava.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/lavaofficial/lava/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/lavaofficial/lava.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/lavaofficial/lava/context:cpp)

What is Lava?
--------------

[Lava](https://lava.money) is a privacy focused cryptocurrency that utilizes zero-knowledge proofs which allows users to destroy coins and then redeem them later for brand new ones with no transaction history. It was the first project to implement the Zerocoin protocol and has now transitioned to the [Sigma protocol](https://lava.money/what-is-sigma-and-why-is-it-replacing-zerocoin-in-lava/) which has no trusted setup and small proof sizes. Lava also utilises [Dandelion++](https://arxiv.org/abs/1805.11060) to obscure the originating IP of transactions without relying on any external services such as Tor/i2P.

Lava developed and utilizes [Merkle Tree Proofs (MTP)](https://arxiv.org/pdf/1606.03588.pdf) as its Proof-of-Work algorithm which aims to be memory hard with fast verification.

How Lava’s Privacy Technology Compares to the Competition
--------------
![A comparison chart of Lava’s solutions with other leading privacy technologies can be found below](https://lava.money/wp-content/uploads/2019/04/lava_table_coloured5-01.png) 
read more https://lava.money/lavas-privacy-technology-compares-competition/

Running with Docker
===================

If you are already familiar with Docker, then running Lava with Docker might be the the easier method for you. To run Lava using this method, first install [Docker](https://store.docker.com/search?type=edition&offering=community). After this you may
continue with the following instructions.

Please note that we currently don't support the GUI when running with Docker. Therefore, you can only use RPC (via HTTP or the `lava-cli` utility) to interact with Lava via this method.

Pull our latest official Docker image:

```sh
docker pull lavaofficial/lavad
```

Start Lava daemon:

```sh
docker run --detach --name lavad lavaofficial/lavad
```

View current block count (this might take a while since the daemon needs to find other nodes and download blocks first):

```sh
docker exec lavad lava-cli getblockcount
```

View connected nodes:

```sh
docker exec lavad lava-cli getpeerinfo
```

Stop daemon:

```sh
docker stop lavad
```

Backup wallet:

```sh
docker cp lavad:/home/lavad/.lava/wallet.dat .
```

Start daemon again:

```sh
docker start lavad
```

Linux Build Instructions and Notes
==================================

Dependencies
----------------------
1.  Update packages

        sudo apt-get update

2.  Install required packages

        sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils libboost-all-dev

3.  Install Berkeley DB 4.8

        sudo apt-get install software-properties-common
        sudo add-apt-repository ppa:bitcoin/bitcoin
        sudo apt-get update
        sudo apt-get install libdb4.8-dev libdb4.8++-dev

4.  Install QT 5

        sudo apt-get install libminiupnpc-dev libzmq3-dev
        sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler libqrencode-dev

Build
----------------------
1.  Clone the source:

        git clone https://github.com/Lavacoreteam/Lava

2.  Build Lava-core:

    Configure and build the headless Lava binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.
        
        ./autogen.sh
        ./configure
        make

3.  It is recommended to build and run the unit tests:

        make check


macOS Build Instructions and Notes
=====================================
See (doc/build-macos.md) for instructions on building on macOS.



Windows (64/32 bit) Build Instructions and Notes
=====================================
See (doc/build-windows.md) for instructions on building on Windows 64/32 bit.

## Contributors

### Code Contributors

This project exists thanks to all the people who contribute. [[Contribute](CONTRIBUTING.md)].
<a href="https://github.com/Lavacoreteam/Lava/graphs/contributors"><img src="https://opencollective.com/lava/contributors.svg?width=890&button=false" /></a>

### Financial Contributors

Become a financial contributor and help us sustain our community. [[Contribute](https://opencollective.com/lava/contribute)]

#### Individuals

<a href="https://opencollective.com/lava"><img src="https://opencollective.com/lava/individuals.svg?width=890"></a>

#### Organizations

Support this project with your organization. Your logo will show up here with a link to your website. [[Contribute](https://opencollective.com/lava/contribute)]

<a href="https://opencollective.com/lava/organization/0/website"><img src="https://opencollective.com/lava/organization/0/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/1/website"><img src="https://opencollective.com/lava/organization/1/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/2/website"><img src="https://opencollective.com/lava/organization/2/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/3/website"><img src="https://opencollective.com/lava/organization/3/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/4/website"><img src="https://opencollective.com/lava/organization/4/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/5/website"><img src="https://opencollective.com/lava/organization/5/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/6/website"><img src="https://opencollective.com/lava/organization/6/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/7/website"><img src="https://opencollective.com/lava/organization/7/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/8/website"><img src="https://opencollective.com/lava/organization/8/avatar.svg"></a>
<a href="https://opencollective.com/lava/organization/9/website"><img src="https://opencollective.com/lava/organization/9/avatar.svg"></a>
