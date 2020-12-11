## Navodila:

#### Zagon igre (priporočeno):

Zaženi `TowerDefense` za Linux, `TowerDefense.exe` za Windows, za macOS pa se lahko uporabi WINE(WINE is not an emulator - www.winehq.org). 
Windows bo ob zagonu opozoril da je datoteka lahko škodljiva, ker jo ne prepozna, vendar jo je popolnoma varno zagnati.

#### Izdelava izvršljive datoteke iz izvorne kode:

##### Linux:

Postopek je precej enostaven. Naprej potrebujemo programe za idelavo izvršljive datoteke:

Debian based:
`sudo apt install cmake make gcc g++ libgl1-mesa-dev`

Fedora: `sudo dnf install cmake make gcc g++ libXrandr-devel.x86_64 libXinerama-devel.x86_64 libXcursor-devel.x86_64 libXi-devel.x86_64`

```
mkdir build && cd build
cmake ../
make
./TowerDefense
```

Po izvršenih ukazih bo nastala nova datoteka `TowerDefense` znotraj `build` imenika.
`TowerDefense` datoteka mora biti na istem nivoju kakor `res/` in 'external/' imenika.

##### Windows:

Postopek je malo bolj zahteven. Naprej je treba namestiti programe za izdelavo izvršljive datoteke:
- https://cmake.org/download/ (vsaj 3.17 verzija)
- http://mingw-w64.org/doku.php/download/mingw-builds (lahko tudi Visual Studio 2012-2019 prevajalnik)

```
mkdir build
cd build
cmake ../
make
```

Po izvršenih ukazih bo nastala nova datoteka `TowerDefense.exe` znotraj `build` imenika.
`TowerDefense.exe` datoteka mora biti na istem nivoju kakor `res/` in 'external/' imenika.

#### Uporabljene knjižnice:
- GLFW (ustvarjanje okna in za uporabnikov vnos) - https://www.glfw.org/
- GLEW (dostop do OpenGL konteksta) - http://glew.sourceforge.net/
- stb_image (nalaganje slik, Public Domain :D) - https://github.com/nothings/stb
- Assimp (nalaganje objektov) - https://assimp.org/
- Dear_ImGui (UI za razhroščevanje igre) - https://github.com/ocornut/imgui
- stb_truetype (font) - https://github.com/nothings/stb/blob/master/stb_truetype.h
- raudio.h (audio) - https://www.raylib.com/ (od raylib-a je samo uporabljena raudo.h knjižnica, ki je samostojna)

Za urejanje projekta se lahko uporablja poljubni urejevalnik besedila oziroma IDE, ki podpira cmake (jaz sem uporabljal CLion od JetBrains)