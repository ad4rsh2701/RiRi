
# Rapid In-memory Redis Interface || RiRi

RiRi or Rapid In-memory Redis Interface is a ~~simplified and~~ efficient implementation of Redis written in C++23 to benefit from its object oriented features (specifically security) and efficient functions.


## So, another Redis clone?
Yeah, if your redis has both serverless and client-server architecture, better security, no bloat and overall less memory usage, then it's definitely *just another* redis clone.

Excuse my sarcasm, but calling RiRi "a clone of Redis" is incorrect, here's why:
1. Firstly, RiRi is built in C++, not in C (Redis uses C). So, Object Oriented comes into the picture, which is brings a very different implementations.
2. Secondly, it is not explicitly client-server based architecture. RiRi also supports for a serverless architecture (inspired from SQLite).
3. RiRi is nowhere as complex as Redis is, why? Because it is built from scratch, literally. The implementations are completely different.
4. For instance, the hash table used in Redis, is a custom hash table developed by redis, meanwhile RiRi uses *arguably* the world's fastest hash map, `ankerl::unordered_dense`.

What? You need more? Well, I do have those, but I have yet to implement those, so please wait until RiRi reaches the MVP or alpha development stage.

NOTE: This readme is incomplete.