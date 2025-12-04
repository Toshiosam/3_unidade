# 👽 Meme Hunter: Operação NASA

> *"O brasileiro precisa ser estudado pela NASA."*

**Meme Hunter** é um jogo de investigação e estratégia desenvolvido em C++ com a biblioteca gráfica **SFML 3.0.0**. Inspirado no clássico *Carmen Sandiego*, o jogo coloca você na pele de um agente da divisão brasileira da NASA (Núcleo de Análise de Sujeitos Anormais), com a missão de rastrear, identificar e catalogar memes brasileiros que estão causando colapso na realidade.

---

## 🎮 Game Design Document (GDD)

### 1. Visão Geral (High Concept)
Um jogo de "esconde-esconde" global onde o jogador deve usar raciocínio lógico e conhecimentos de cultura da internet brasileira para interceptar alvos antes que o tempo acabe. O tom é humorístico, misturando pixel art com a cultura e referências virais.

### 2. Narrativa e Progressão
* **Premissa:** A NASA descobriu que as leis da física não se aplicam ao Brasil. Para conter essa anomalia, foi criada a divisão M.I.B. (Men In Brazil).
* **Carreira:** O jogador começa como **Estagiário** (Nível Fácil). Ao capturar todos os sujeitos de um nível, ele é **Promovido**, desbloqueando casos mais complexos e rotas de viagem mais longas.
* **Objetivo Final:** Alcançar a patente máxima de **Agente M.I.B.** e zerar a "pokedex" de memes.

### 3. Mecânicas Principais (Core Loop)
O ciclo de jogo consiste em quatro etapas estratégicas:

1.  **Investigação:** O jogador visita cidades locais para interrogar moradores.
    * *Dica de Identidade:* Revela características do suspeito (ex: "Tem formato de garrafa").
    * *Dica de Viagem:* Revela o próximo país de destino (ex: "Foi comer sushi").
2.  **Gestão de Tempo:** O jogo possui um timer implacável ("T-Horas").
    * Investigar gasta **1 Unidade de Tempo**.
    * Viajar gasta **3 Unidades de Tempo**.
    * *Punição:* Errar o país de destino faz o jogador perder tempo precioso voltando.
3.  **Estudo (Barra de Energia):**
    * Coletar pistas corretas preenche a "Barra de Estudo".
    * Ao atingir 100%, a imagem do meme é revelada na tela (com tratamento de *Chroma Key* em tempo real), facilitando a identificação visual.
4.  **Interceptação:**
    * No país final, o jogador deve iniciar o "Protocolo de Interceptação".
    * É necessário selecionar o nome correto do meme em uma lista. Se errar, o tempo é penalizado ou a missão falha.

### 4. Dificuldade Dinâmica
* **Fácil (Estagiário):** Rotas curtas (3 Países). Tempo generoso (24h). 3 Erros permitidos.
* **Médio (Agente):** Rotas médias (5 Países). Tempo ajustado (33h). 2 Erros permitidos.
* **Difícil (M.I.B.):** Rotas longas (7 Países). Tempo "Hardcore" (39h). Zero margem para erro.

### 5. Personagens (Os Alvos)
O jogo conta com 12 lendas da internet, voce será capaz de capturar todos e se tornar uma lenda viva entre os M.I.Bs ????
