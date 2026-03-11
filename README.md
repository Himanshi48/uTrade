<h1 align="center">🚀 RedisLite</h1>

<p align="center">
A <b>Redis-inspired thread-safe in-memory key-value store</b> built in <b>C++</b>.
<br>
Designed for <b>caching systems, session stores, and low-latency services</b>.
</p>

<hr>

<h2>✨ Features</h2>

<ul>
<li>⚡ <b>Fast O(1) key-value storage</b> using <code>unordered_map</code></li>
<li>🔒 <b>Thread-safe operations</b> using <code>mutex</code></li>
<li>⏳ <b>TTL expiration</b> with lazy + background cleanup</li>
<li>🔍 <b>Pattern search</b> using <code>KEYS prefix*</code></li>
<li>💾 <b>Snapshot persistence</b> using JSON</li>
<li>📊 <b>Memory statistics</b> via <code>STATS</code> command</li>
</ul>

<hr>

<h2>📦 Supported Commands</h2>

<pre>
SET key value [EX seconds]
GET key
DEL key
TTL key
KEYS pattern
SAVE
LOAD
STATS
EXIT
</pre>

<hr>

<h2>🧪 Example</h2>

<pre>
SET user:1 Alice EX 10
GET user:1
TTL user:1
KEYS user:*
SAVE
STATS
</pre>

<pre>
OK
Alice
9
user:1
Snapshot saved
Total keys: 1
</pre>

<hr>

<h2>⚙️ Compile & Run</h2>

<pre>
clang++ main.cpp -o redislite
./redislite
</pre>

<hr>

<p align="center">
Built for demonstrating <b>concurrency, caching, TTL expiration, and persistence</b> in C++.
</p>
