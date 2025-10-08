# TODO

## Design/ Arquitetura
- [x] Esboço/ base do sistema ECS [@GersonFeDutra](https://github.com/GersonFeDutra)
- [x] Esboço dos sistemas de jogo [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Cenário [@AndreCastr0](https://github.com/AndreCastr0)

### Sistemas de jogo

- [x] Movimentação [@IgorTorquatto](https://github.com/IgorTorquatto)
  - [x] Interpolação [@GersonFeDutra](https://github.com/GersonFeDutra)
  - [x] Entrada [@IgorTorquatto](https://github.com/IgorTorquatto)
  - [x] Pulo [@IgorTorquatto](https://github.com/IgorTorquatto)
  - [x] Scroll do cenário [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Coleta de itens [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Colisão e Condição de morte [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Loop de jogabilidade [@IgorTorquatto](https://github.com/IgorTorquatto)

## Construção da cena

### Geometria
- [x] Carregamento de objetos 3D com cores distintas
  - [x] Personagem [@marclobo01](https://github.com/marclobo01)
    - [x] Modelo cubóide
    - [x] Cores
  - [x] Inimigos / Obstáculos
    - [x] Pedras
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Iluminação [@AndreCastr0](https://github.com/AndreCastr0)
      - [x] Sombreamento Helmano
      - [x] Materiais [@GersonFeDutra](https://github.com/GersonFeDutra)
      - [x] Textura [@GersonFeDutra](https://github.com/GersonFeDutra)
  - [x] Coletáveis
    - [x] Moedas [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Iluminação [@AndreCastr0](https://github.com/AndreCastr0)
      - [x] Sombreamento Helmano
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Materiais + Cores [@GersonFeDutra](https://github.com/GersonFeDutra)
  - [x] Cenário
    - [x] Piso
      - [x] Modelo Plano - Quad [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Iluminação [@AndreCastr0](https://github.com/AndreCastr0)
      - [x] Sombreamento Helmano
      - [x] Textura [@GersonFeDutra](https://github.com/GersonFeDutra)
    - [x] Árvores
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Iluminação [@AndreCastr0](https://github.com/AndreCastr0)
      - [x] Sombreamento Helmano
      - [x] Materiais [@GersonFeDutra](https://github.com/GersonFeDutra)
      - [x] Textura [@GersonFeDutra](https://github.com/GersonFeDutra)
    - [x] Pilha de madeira
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Iluminação [@AndreCastr0](https://github.com/AndreCastr0)
      - [x] Sombreamento Helmano
      - [x] Materiais [@GersonFeDutra](https://github.com/GersonFeDutra)
      - [x] Textura [@GersonFeDutra](https://github.com/GersonFeDutra)
    - [x] Ambiente [@marclobo01](https://github.com/marclobo01)
- [x] Ative a remoção de superfícies ocultas. [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Anti-aliasing (MSAA) [@GersonFeDutra](https://github.com/GersonFeDutra)

> Extras
> - [x] Importação de um objeto.
> - [x] Adição de texturas nos objetos. [@GersonFeDutra](https://github.com/GersonFeDutra)
> - [x] Limpeza de memória.

### Iluminação

- [x] Adicione as iluminações na cena:
  - [x] Ambiente (para a cena);
  - [x] Especular;
  - [x] Difusa.

> Extras
> - [x] Implementação da projeção da sombra dos objetos.

### Camera

- [x] Uso de câmera para navegação ou visualização da cena. 
- [x] Use projeções perspectivas e/ou ortogonais. 

### Animações

> Extras
> - [x] Movimento (interativo) do personagem
>   - [x] Entrada [@IgorTorquatto](https://github.com/IgorTorquatto)
>   - [x] Interpolação [@GersonFeDutra](https://github.com/GersonFeDutra).
>   - [x] Animação
> - [x] Movimento dos objetos da cena [@IgorTorquatto](https://github.com/IgorTorquatto)
>     - [x] Inimigos
>     - [x] Coletáveis (ex.: moedas)

### Elementos de Interface

- [x] Menu iniciar [@IgorTorquatto](https://github.com/IgorTorquatto)
  - [x] Botão de começar jogo
  - [x] Botão de ranking
  - [x] Botão de sair do jogo
- [x] Ranking
  - [x] Menu
  - [x] Salvar e carregar arquivos
- [x] Contador de coletáveis (ex.: número de moedas coletadas, número de mortes)

> Extras
> - [x] Pausar jogo [Helmano]

## Funcionalidades extras

- [x] Sonorização [@GersonFeDutra](https://github.com/GersonFeDutra).
  - [x] Importar biblioteca [STL2]
  - [x] Música
  - [x] Efeitos sonoros
- [ ] Menu de opções
  - [ ] Controle de áudio

<!-- - [x] Contador de fps / debug [@GersonFeDutra](https://github.com/GersonFeDutra). -->
