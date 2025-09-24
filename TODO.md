# TODO

## Design/ Arquitetura
- [x] Esboço/ base do sistema ECS [@GersonFeDutra](https://github.com/GersonFeDutra).
- [x] Esboço dos sistemas de jogo [@IgorTorquatto](https://github.com/IgorTorquatto)
- [ ] Cenário [@IgorTorquatto](https://github.com/IgorTorquatto)

### Sistemas de jogo

- [ ] Movimentação [@IgorTorquatto](https://github.com/IgorTorquatto)
  - [ ] Interpolação [@GersonFeDutra](https://github.com/GersonFeDutra).
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
      - [ ] Texturas / cores
  - [ ] Coletáveis
  - [ ] Cenário
    - [x] Piso [@IgorTorquatto](https://github.com/IgorTorquatto)
    - [ ] Árvores
      - [x] Modelo [@IgorTorquatto](https://github.com/IgorTorquatto)
      - [ ] Texturas / Cores
    - [ ] Prédios/ Paredes
- [x] Ative a remoção de superfícies ocultas. [@IgorTorquatto](https://github.com/IgorTorquatto)

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

- [ ] Movimento (interativo) do personagem
  - [ ] Interpolação [@GersonFeDutra](https://github.com/GersonFeDutra).
- [x] Movimento dos objetos da cena [@IgorTorquatto](https://github.com/IgorTorquatto)
    - [x] Inimigos
    - [x] Coletáveis (ex.: moedas)

### Elementos de Interface

- [x] Contador de coletáveis (ex.: número de moedas coletadas, número de mortes)

> Extras
> - [x] Menu iniciar [@IgorTorquatto](https://github.com/IgorTorquatto)
>   - [x] Botão de começar jogo
>   - [x] Botão de sair do jogo
> - [ ] Pausar jogo

<!-- - [x] Contador de fps / debug [@GersonFeDutra](https://github.com/GersonFeDutra). -->
