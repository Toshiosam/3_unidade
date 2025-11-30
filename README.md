👽 Meme Hunter: Operação NASA
"O brasileiro precisa ser estudado pela NASA."

Meme Hunter é um jogo de investigação e estratégia desenvolvido em C++ com a biblioteca gráfica SFML 3.0.0. Inspirado no clássico Carmen Sandiego, o jogo coloca você na pele de um agente da divisão brasileira da NASA (Núcleo de Análise de Sujeitos Anormais), com a missão de rastrear, identificar e catalogar memes brasileiros que estão causando colapso na realidade.

🎮 Game Design Document (GDD)
1. Visão Geral (High Concept)
Um jogo de "esconde-esconde" global onde o jogador deve usar raciocínio lógico e conhecimentos de cultura da internet brasileira para interceptar alvos antes que o tempo acabe. O tom é humorístico, misturando pixel art com a cultura "maker" e referências virais.

2. Narrativa e Progressão
Premissa: A NASA descobriu que as leis da física não se aplicam ao Brasil. Para conter essa anomalia, foi criada a divisão M.I.B. (Men In Brazil).

Carreira: O jogador começa como Estagiário (Nível Fácil). Ao capturar todos os sujeitos de um nível, ele é Promovido, desbloqueando casos mais complexos e rotas de viagem mais longas.

Objetivo Final: Alcançar a patente máxima de Agente M.I.B. e zerar a "pokedex" de memes.

3. Mecânicas Principais (Core Loop)
O ciclo de jogo consiste em quatro etapas estratégicas:

Investigação: O jogador visita cidades locais para interrogar moradores.

Dica de Identidade: Revela características do suspeito (ex: "Tem formato de garrafa").

Dica de Viagem: Revela o próximo país de destino (ex: "Foi comer sushi").

Gestão de Tempo: O jogo possui um timer implacável ("T-Horas").

Investigar gasta 1 Unidade de Tempo.

Viajar gasta 3 Unidades de Tempo.

Punição: Errar o país de destino faz o jogador perder tempo precioso voltando.

Estudo (Barra de Energia):

Coletar pistas corretas preenche a "Barra de Estudo".

Ao atingir 100%, a imagem do meme é revelada na tela (com tratamento de Chroma Key em tempo real), facilitando a identificação visual.

Interceptação:

No país final, o jogador deve iniciar o "Protocolo de Interceptação".

É necessário selecionar o nome correto do meme em uma lista. Se errar, o tempo é penalizado ou a missão falha.

4. Dificuldade Dinâmica
Fácil (Estagiário): Rotas curtas (3 Países). Tempo generoso (24h). 3 Erros permitidos.

Médio (Agente): Rotas médias (5 Países). Tempo ajustado (33h). 2 Erros permitidos.

Difícil (M.I.B.): Rotas longas (7 Países). Tempo "Hardcore" (39h). Zero margem para erro.

5. Personagens (Os Alvos)
O jogo conta com 12 lendas da internet brasileira, cada um com 7 dicas culturais únicas, incluindo:

Fofão da Carreta (Parkour e Dança)

Grávida de Taubaté (Engenharia Biológica Reversa)

Et Bilu (Buscador de Conhecimento)

Bora Bill (Narrativa Esportiva)

Jeremias (O Cão foi quem botou pra nois beber)

...e muitos outros.

🛠️ Aspectos Técnicos
Estrutura do Código
O projeto utiliza Orientação a Objetos (OOP) moderna em C++17/20:

Game Engine Customizada: Gerencia o loop principal, processamento de eventos (usando std::optional do SFML 3) e renderização.

Máquina de Estados: Controla o fluxo entre INVESTIGANDO, VIAJANDO, ABDUZINDO e PROMOVIDO.

Sistema de Áudio Procedural: Gerenciador de som (SoundManager) que sintetiza ondas sonoras em tempo real (senoidais e sweeps) para criar efeitos retrô sem necessidade de arquivos .wav externos.

Chroma Key Dinâmico: Algoritmo que carrega imagens PNG, detecta pixels brancos puros e cria uma máscara de transparência em tempo de execução, permitindo o uso de imagens simples da internet (JPG/PNG) sem edição prévia no Photoshop.

Tecnologias Utilizadas
Linguagem: C++ (Padrão C++17 ou superior)

Biblioteca Gráfica: SFML 3.0.0 (Graphics, Window, System, Audio)

IDE Recomendada: VS Code (MinGW 64-bit)

🚀 Como Compilar e Jogar
Pré-requisitos
Certifique-se de ter o compilador g++ e as bibliotecas do SFML 3.0.0 configuradas corretamente no seu ambiente (pastas include e lib).

Compilação (Terminal)
Navegue até a pasta do projeto (evite pastas com espaços ou acentos como 3ºunidade, use Unidade3) e execute:

Bash

# Compila os objetos
g++ -c src/*.cpp -I include

# Linka e cria o executável
g++ *.o -o MemeHunter -L lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
Controles
Setas (↑ / ↓) ou Mouse: Navegar no menu de opções e cidades.

Botão Esquerdo do Mouse: Clicar nos botões de cidades, viagens e abdução.

Enter: Confirmar seleção / Avançar diálogos de promoção.

M: Mutar/Desmutar o som (toggle).

ESC: Sair do jogo.
