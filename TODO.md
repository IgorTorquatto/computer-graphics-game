# TODO

## Design/ Arquitetura
- [x] Esboço/ base do sistema ECS [@GersonFeDutra](https://github.com/GersonFeDutra).
- [x] Esboço dos sistemas de jogo [@IgorTorquatto](https://github.com/IgorTorquatto)
- [ ] Cenário [@IgorTorquatto](https://github.com/IgorTorquatto) 

### Sistemas de jogo

- [x] Movimentação [@IgorTorquatto](https://github.com/IgorTorquatto)
  - [x] Interpolação [@GersonFeDutra](https://github.com/GersonFeDutra).
  - [x] Entrada [@IgorTorquatto](https://github.com/IgorTorquatto).
  - [x] Pulo [@IgorTorquatto](https://github.com/IgorTorquatto).
- [x] Coleta de itens [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Colisão e Condição de morte [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Loop de jogabilidade [@IgorTorquatto](https://github.com/IgorTorquatto)

## Construção da cena

### Geometria
- [ ] Carregamento de objetos 3D com cores distintas
  - [ ] Personagem
  - [ ] Inimigos / Obstáculos
    - [ ] Pedras
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Cores / sombreamento
      - [ ] Texturas
  - [ ] Coletáveis
    - [x] Moedas [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Modelo
      - [x] Cores / sombreamento
  - [ ] Cenário
    - [x] Piso [@IgorTorquatto](https://github.com/IgorTorquatto)
    - [ ] Árvores
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Cores / sombreamento [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [ ] Texturas
    - [ ] Pilha de madeira
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [x] Cores / sombreamento [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [ ] Texturas
    - [ ] Prédios/ Paredes
- [x] Ative a remoção de superfícies ocultas. [@IgorTorquatto](https://github.com/IgorTorquatto)
- [x] Anti-aliasing (MSAA) [@GersonFeDutra](https://github.com/GersonFeDutra)

> Extras
> - [x] Importação de um objeto.
> - [ ] Adição de texturas nos objetos.
> - [x] Limpeza de memória.

### Iluminação

- [ ] Adicione as iluminações na cena:
  - [ ] Ambiente (para a cena);
  - [ ] Especular;
  - [ ] Difusa.

> Extras
> - [ ] Implementação da projeção da sombra dos objetos.

### Camera

- [x] Uso de câmera para navegação ou visualização da cena. 
- [x] Use projeções perspectivas e/ou ortogonais. 

### Animações

> Extras
> - [ ] Movimento (interativo) do personagem
>   - [x] Entrada [@IgorTorquatto](https://github.com/IgorTorquatto)
>   - [x] Interpolação [@GersonFeDutra](https://github.com/GersonFeDutra).
>   - [ ] Animação
> - [x] Movimento dos objetos da cena [@IgorTorquatto](https://github.com/IgorTorquatto)
>     - [x] Inimigos
>     - [x] Coletáveis (ex.: moedas)

### Elementos de Interface

- [x] Menu iniciar [@IgorTorquatto](https://github.com/IgorTorquatto)
  - [x] Botão de começar jogo
  - [x] Botão de sair do jogo
- [x] Contador de coletáveis (ex.: número de moedas coletadas, número de mortes)

> Extras
> - [ ] Pausar jogo

## Funcionalidades extras

- [ ] Sonorização [@GersonFeDutra](https://github.com/GersonFeDutra).
  - [x] Música
  - [ ] Efeitos sonoros

<!-- - [x] Contador de fps / debug [@GersonFeDutra](https://github.com/GersonFeDutra). -->
