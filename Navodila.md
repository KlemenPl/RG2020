## Navodila:

#### Zagon igre:

Zaženi `TowerDefense` za Linux, `TowerDefense.exe` za Windows, za macOS pa uporabi WINE(wine is not an emulator). 
Windows bo ob zagonu opozoril da je datoteka lahko škodljiva, ker je ne prepozna vendar jo je popolnoma varno zagnati.

#### Izdelava izvršljive datoteke iz izvorne kode:

##### Linux:

Postopek je precej enostaven. Naprej potrebujemo programe za idelavo izvršljive datoteke:

`sudo apt install cmake make gcc`

Namesto `gcc` prevajalnika se lahko uporabi tudi drugi poljubni prevajalnik.

```
mkdir build && cd build
cmake ../
make
./TowerDefense &
```

Po izvršenih ukazih bo nastala nova datoteka `TowerDefense` znotraj `build` imenika.
`TowerDefense` datoteka mora biti na istem nivoju kakor `res/` imenik.

##### Windows:

Postopek je malo bolj zahteven. Naprej je treba namestiti programe za izdelavo izvršljive datoteke:
- https://cmake.org/download/


#### Uporabljene knjižnice:
- GLFW (ustvarjanje okna in za uporabnikov vnos) - https://www.glfw.org/
- GLEW (dostop do OpenGL konteksta) - http://glew.sourceforge.net/
- stb (nalaganje slik, Public Domain :D) - https://github.com/nothings/stb
- Assimp (nalaganje objektov) - https://assimp.org/
- Dear_ImGui (UI za razhroščevanje igre) - https://github.com/ocornut/imgui

Za urejanje projekta se lahko uporablja poljubni urejevalnik besedila oziroma IDE (jaz sem uporabljal CLion od JetBrains)