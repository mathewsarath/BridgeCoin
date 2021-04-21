# BridgeCoin
Note: This is for educational purposes, showing how to develop a cryptocurrency using C++20; it is not a real cryptocurrency.
I wanted a new project to teach and also improve my own knowledge about C++20. This is combined with my love for cryptocurrency  solving real-world problems that are in existing ones. Common problems are either the language chosen for the reference implementation are not performant enough e.g using GoLang (eth/avax), have fees (most cryptos) and most are not designed with performance in mind from the get-go and try to shoe horn optimizations in afterwards. People often quote VISA as peaking over 57K tps, crypto opens up a lot more doors for new innovation and most of the world does not even has access to this network. I believe over 1Million TPS is required for a fully automonous world.

Features include:  
* Decentralized
* Instant - Finality will need to be reached very quickly
* Consensus mechanism - Novel
* Smart contracts
* User friendly - Requiring the underlying crypto to pay as a gas fee for interacting with smart contracts is very bad UX and makes onboarding users much more difficult (imagine scheduling nationwide voting system).
* DB driven design
* No serialization/deserialization required and many other features making use of the underling hardware architecture.
* Signature verification is moved outside to reduce compute load
* Sharding of the network to improve throughput
