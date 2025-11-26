# LRU Cache Implementation in C

This project implements an LRU (Least Recently Used) cache using:
- HashMap (array of Node pointers) for O(1) lookup
- Doubly Linked List to track usage order

## Features
 get(key) — returns value or NULL  
 put(key, value) — insert/update in O(1)  
 Evicts least recently used item when capacity is full  
 Most recently used at front, least recently used at rear  

## Run
gcc lru_cache.c -o lru
./lru

## Run With testcases
./lru < testcases.txt