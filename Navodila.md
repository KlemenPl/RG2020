## Navodila:

#### Zagon igre (priporočeno):

Zaženi `TowerDefense` za Linux, `TowerDefense.exe` za Windows, za macOS pa se lahko uporabi WINE(WINE is not an emulator - www.winehq.org). 
Windows bo ob zagonu opozoril da je datoteka lahko škodljiva, ker jo ne prepozna, vendar jo je popolnoma varno zagnati.

#### Izdelava izvršljive datoteke iz izvorne kode:

##### Linux:

Postopek je precej enostaven. Naprej potrebujemo programe za idelavo izvršljive datoteke:

Ubuntu:
`sudo apt install cmake make gcc g++ glfw`

Fedora: `sudo dnf install cmake make gcc g++ glfw`

Navodila za prevajanje:
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
- http://mingw-w64.org/doku.php/download/mingw-builds (lahko tudi Visual Studio 2012-2019 prevajalnik, 
  vendar ne vem če bo pravilno delalo)

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
- glad (za lažji dostop do OpenGL funkcij) - https://github.com/go-gl/glfw/blob/master/v3.1/glfw/glfw/deps/glad/glad.h
- stb_image (nalaganje slik, Public Domain :D) - https://github.com/nothings/stb
- Dear_ImGui (UI za razhroščevanje igre) - https://github.com/ocornut/imgui
- stb_truetype (font) - https://github.com/nothings/stb/blob/master/stb_truetype.h
- glm (za računanje) - https://glm.g-truc.net/0.9.9/index.html

Za urejanje projekta se lahko uporablja poljubni urejevalnik besedila oziroma IDE, ki podpira cmake (jaz sem uporabljal CLion od JetBrains)