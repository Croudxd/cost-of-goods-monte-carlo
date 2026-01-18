# cost-of-goods-monte-carlo
Monte carlo model for predicting the price of goods from select inputs.

The simulation is good until around 4 million + simsize, where it starts taking a very long time and can be funny on my machine anyway.

**(Built for my dad)**

## Built using glfw and ImGui.
Both are small enough to be statically linked. so no dependencies other than if your building cmake and a cpp compiler (17).

## Preview

https://github.com/user-attachments/assets/b6c86aab-e975-49ae-b563-31492b934abb


### Building on linux.

```c
git clone https://github.com/Croudxd/cost-of-goods-monte-carlo
```

```c
mkdir build & cd build
```

```c
cmake ..
```

```c
make -j
```
### Building on windows.

```c
git clone https://github.com/Croudxd/cost-of-goods-monte-carlo
```

```c
mkdir build
```

```c
cmake ..
```

```c
cmake --build . --config Release
```

#### Now built we can run the program :

```c
./MyApp
```


### Bugs
Please email me at
```lua
benjamincroud@gmail.com
```
for any bug reports.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
