# Primetheus
<p align="center">
  <img src="https://raw.githubusercontent.com/Rick-laboratory/Primetheus/main/banner.png" alt="Primetheus Banner" width="300"/>
</p>

# 🧠 Primetheus – High-Performance Prime Number Sieve

**Primetheus** is a high-performance, memory-efficient, and multithreaded prime counting engine.  
It calculates the number of primes up to **10¹²** in **~188 seconds** using only **13 MB RAM** throughout the entire generation.

Built with **pure C++**, low-level memory control, and raw performance in mind.
And LOVE of GOD

---

## 🚀 Features

- ⚙️ **Bitpacked Segment Sieving** – Ultra-efficient memory usage
- 🧵 **Multithreaded (32 threads in my case)** – Fully parallelized across CPU cores
- 🧮 **Custom Integer Square Root** – No floating-point inaccuracies, therefore little less affected by already running applications
- 🧱 **Wheel-30 Filtering** – Filters ~73% of composites *before* sieving
- 💾 **Fixed RAM footprint (~13 MB)** – No heap growth during execution
- 🔐 **Licensed under SSPL** – For controlled, transparent use

---

## 📊 Benchmark

| Input Range | Threads | Time     | RAM Usage | Prime Count        | Accuracy   |
|-------------|---------|----------|-----------|--------------------|------------|
| 10¹²        |32 mycase| ~188 sec | ~13 MB    | 37,607,912,018     | ✅ Exact  |

> Benchmarked on an AMD Ryzen 3950X system

---

## 🛠️ Build Instructions

### Visual Studio

1. Clone the repository  
   `git clone https://github.com/Rick-laboratory/Primetheus.git`
2. Open `Primetheus.vcxproj` in Visual Studio
3. Select **Release x64**
4. Build & run

✅ No `.sln` required  
✅ No external libraries  
✅ No bullshit

---

## 🔍 Why?

Because too much modern code is bloated, abstracted, and inefficient.  
Primetheus proves you can:

- Process **trillion-scale input**  
- With **megabyte-static memory**  
- At **CPUs-native speed**

It's my personal **engineering testament.**

---

## 🤯 Philosophy

We don’t overuse memory. We don’t overuse abstractions.  
We respect the system. And it repsects us.

---

## 📜 License

**SSPL (Server Side Public License)**  
Use is permitted with conditions. Redistribution and modification require full compliance.  
See `LICENSE` for details.

---

## 📧 Contact

Built by [Rick Armbruster](https://github.com/Rick-laboratory)

For questions, licensing, or performance consulting:  
📫 [LinkedIn](https://www.linkedin.com/in/rick-armbruster-721600223/)

---

## 🧠 Pro tip

Try building this on Linux using `g++` with `-std=c++20 -O3 -march=native -pthread`  
If you beat the current benchmark, let me know. 😎

---

## 🌍 What’s next?

- SIMD acceleration (optional AVX2 build)
- Extended output (store primes / diffs)
- Cross-platform benchmarks
- Binary distribution

---

> "Built because the world needed a smarter sieve.  

## License
This project is licensed under the Server Side Public License v1 (SSPL).  
See the [LICENSE](./LICENSE) file for details.
