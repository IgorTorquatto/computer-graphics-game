# Computer Graphics Game

Repositório para desenvolvimento de um jogo 3D em OpenGL usando a linguagem C
para o curso de computação gráfica [CC0029] - Semestre 2025.1 da Universidade
Federal do Cariri (UFCA).

## TOC

- [Objetivos](#objetivos)
- [Instalação](#instalação)
    - [Code Blocks](#code-blocks)
    - [Vs Code](#vs-code)
    - [Linux](#linux)
    - [Outros](#objetivos)

----

## Objetivos

1. Este trabalho tem como objetivo aplicar os conceitos e técnicas de computação 
gráfica para desenvolver um jogo interativo utilizando a API OpenGL e a 
linguagem C, integrando elementos visuais, animação e interatividade em suas implementações. 
    > a) Desenvolva algum jogo interativo de sua escolha (não é necessário que seja conhecido. [Exemplo](https://www.youtube.com/watch?v=y2PS-YD625k). O jogo 
    deve incluir, no mínimo, os seguintes requisitos:
    > - [ ] Carregamento de objetos 3D com cores distintas.
    > - [ ] Adicione as iluminações na cena:
    >   - [ ] Ambiente (para a cena);
    >   - [ ] Especular;
    >   - [ ] Difusa.
    > - [ ] Uso de câmera para navegação ou visualização da cena. 
    > - [ ] Use projeções perspectivas e/ou ortogonais. 
    > - [ ] Ative a remoção de superfícies ocultas.
 
    > b) Pontos extras (cada item vale 0,5 ponto extra):
    > - [ ] Importação de um objeto.
    > - [ ] Adição de texturas nos objetos. 
    > - [ ] Implementação da projeção da sombra dos objetos. 
2. Demonstração do jogo. 
    - [ ] Durante a apresentação, cada grupo deve demonstrar o funcionamento do 
    jogo e explicar as técnicas de computação gráfica aplicadas. 

### Definição das Apresentações

Cada grupo deverá preparar e realizar uma apresentação detalhada sobre o projeto desenvolvido, abordando os seguintes aspectos: 
- [ ] Ideia e conceito: descreva a proposta do jogo e a inspiração que motivou a 
escolha do tema. 
- [ ] Demonstração do jogo: apresente o funcionamento geral do jogo, os 
objetivos do jogador e os principais elementos de jogabilidade. 
- [ ] Implementação e lógica: explique como o código foi estruturado e como as 
principais funcionalidades foram implementadas, como movimentação, 
colisões, estados do jogo e interações. 
- [ ] Técnicas de computação gráfica: explique como foram aplicadas as técnicas 
de modelagem 3D, câmera, iluminação, projeção e remoção de superfícies 
ocultas. 
- [ ] Recursos extras (se aplicáveis): demonstre a implementação de importação 
de objetos, texturas e projeção de sombras. 
- [ ] Desafios e soluções: comente os principais problemas enfrentados e como 
foram resolvidos.

## Instalação

### Code Blocks

1) Baixar o [MinGW](https://www.mingw-w64.org) ou [Link Direto](https://drive.google.com/drive/folders/1-rRkvCay8gRyIh5ZZNpYVvOQPz1Z_82q?usp=drive_link) -  Windows 10+.<br>
É um ambiente de desenvolvimento que dá suporte ao compilador GCC em sistemas Windows.
    1) Instalar em C:\MinGW (<MINGW_HOME>)
    2) Na instalação do MinGW, instalar os pacotes básicos “mingw32-base”, 
“mingw32-gcc-g++” e “msys-base”
    3) Colocar os diretórios <MINGW_HOME>\bin e <MINGW_HOME>\msys\1.0\bin na variável de ambiente Path do Windows
        - Execute o Powershell como Administrador:
        ```powershell
        setx Path "$env:MINGW_HOME\bin;$env:MINGW_HOME\msys\1.0\bin;$env:Path" /M
        ```

2) Para instalar a biblioteca FreeGLUT no MinGW
    1) Faça o download da [biblioteca](https://drive.google.com/drive/folders/1cQi8lQwe98hFjQcXezO6Hn2YaiFCaGWx?usp=share_link). Contém os arquivos de cabeçalho (headers) e as bibliotecas 
estáticas (libs) e dinâmicas (bin)
    2) Descompacte o arquivo baixado no passo anterior.
    3) Copie os arquivos que estão em “include\GL” para “<MINGW_HOME>\include\GL”
    4) Os arquivos da pasta “lib” para “<MINGW_HOME>\lib”
    5) Os arquivos da pasta “bin” para “<MINGW_HOME>\bin”
3) Baixar e instalar o CodeBlocks [versão sem MinGW](http://www.codeblocks.org), caso goste de usar uma IDE para compilar seus programas em C.
4) Importe o arquivo [CG-Paint.cdb](./CG-Paint.cdb)

### VS Code

1. Abra o painel de extensões `Ctrl + Shift + X`;
2. Instale as extensões [recomendadas](extensions.json);
3. Caso você esteja no Linux siga as instruções [abaixo](#linux), caso contrário veja a seção ["Outros"](#outros).
4. Execute o projeto com `F5` (ou em `View > Command Pallet > Debug: Start Debugging`). **Você deve ter o gdb instalado!**

> [!WARNING]
> Por padrão o projeto será construído em modo debug. Você pode fazer uma build release usando a task `F7` (ou em `View > Command Pallet > Tasks: Run Tasks`): selecione `Build and Run [release]`.

### Linux

1. Instale as bibliotecas:
    <details> <summary>Arch [pacman]</summary>

    ```bash
    sudo pacman -Syu freeglut glu mesa
    ```
    </details>

    <details> <summary>Debian / Ubuntu [apt]</summary>

    ```bash
    sudo apt update
    sudo apt install freeglut3 freeglut3-dev mesa-utils mesa-common-dev libglu1-mesa-dev
    ```
    </details>

    <details> <summary>Outros: [compile source]</summary>

    ```bash
    sudo apt install build-essential cmake git   # (Debian-like)
    # ou
    sudo pacman -S base-devel git cmake          # (Arch-like)
    
    # Clone e compile FreeGLUT do código fonte:
    git clone https://github.com/FreeGLUTProject/freeglut
    cd freeglut
    cmake .
    make -j$(nproc)
    sudo make install
    ```
    </details>
2. Instale o CMake. (Opcional, mas recomendado):
    <details> <summary>Arch [pacman]</summary>

    ```bash
    sudo pacman -Syu cmake
    ```
    </details>

    <details> <summary>Debian / Ubuntu [apt]</summary>

    ```bash
    sudo apt update
    sudo apt install cmake
    ```
    </details>

    <details> <summary>Outros [build src]</summary>

    ```bash
    # Baixando o código fonte:
    wget https://github.com/Kitware/CMake/releases/latest/download/cmake-$(wget -qO- https://cmake.org/download/ | grep -oP 'cmake-\d+\.\d+\.\d+' | head -n 1).tar.gz
    # Extração + Compilação:
    tar -xzvf cmake-*.tar.gz
    cd cmake-*
    ./bootstrap
    make -j$(nproc)
    sudo make install
    ```
    </details>
3. Compile com [scripts/build.sh](./scripts/build.sh):
    - Torne executável:
        ```bash
        chmod +x ./scripts/build.sh
        ```
    - Build [debug]:
        ```bash
        ./scripts/build.sh --debug
        ```
    - Build [Release]:
        ```bash
        ./scripts/build.sh --debug
        ```
4. Execute com [scripts/run.sh](./scripts/run.sh):
    - Torne executável:
        ```bash
        chmod +x ./scripts/run.sh
        ```
    - Build:
        ```bash
        # o script usará o symlink para a última compilação [release / debug]
        ./scripts/run.sh
        ```

    > [!TIP]
    > Você pode configurar sua IDE para compilar e executar o projeto usando o script de build. Por exemplo: [usando build tasks do VsCode](#vs-code).

### Outros

- Requisitos:
    - Compilador C: [MingW](https://learn.microsoft.com/pt-br/vcpkg/users/platforms/mingw) / (MSVC)[https://learn.microsoft.com/pt-br/cpp/windows/latest-supported-vc-redist?view=msvc-170] / [GCC](https://gcc.gnu.org/) / [Clang](https://clang.llvm.org/) / [LLVM](https://llvm.org/)
    - Biblioteca (OpenGL): [FreeGLUT (latest)](https://freeglut.sourceforge.net/)
- Compilação / Link:
    - O binário deve ser feito considerando todos os arquivos da pasta [src](./src/)
    - Alguns compiladores exigem que a biblioteca math do C seja incluída com a flag `-lm`.
    - Opcionalmente você pode compilar usando o script Cmake do projeto [CMakeLists.txt](./CMakeLists.txt).
- Convenções:
    - Crie seu executável na pasta [bin](./bin/). Use `bin/debug` para versão de desenvolvimento ou `bin/release` para versão de lançamento.
    - A flag `DEBUG` é usada no código fonte para detectar versões de desenvolvimento. Ela pode ser definida na fase de build pelo seu compilador.
