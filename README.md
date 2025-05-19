<p align="center">
  <img src="https://github.com/Carlosesposito22/JogoAED/blob/main/src/sprites/logo_jogo.png?raw=true" alt="Logo do Jogo" width="800">
</p>

Este projeto é um jogo desenvolvido em C utilizando a biblioteca [Raylib](https://www.raylib.com/) e integração com a [API Gemini](https://ai.google.dev/). O jogo simula um processo seletivo em um braço do **FBI** voltado para cibersegurança chamado **Cybertech**, onde o jogador precisa provar suas habilidades técnicas e éticas para conquistar uma vaga.

---

## 🌟 Sobre o Jogo

O jogador assume o papel de um candidato tentando ingressar na **Cybertech**, um braço do **FBI** especializado em cibersegurança. Para isso, deve enfrentar desafios práticos e éticos sob avaliação do agente **Hank Miccuci**. A cada fase, decisões técnicas e morais são postas à prova, e o desempenho final determinará o sucesso no processo seletivo.

### 🎯 Objetivo

- **Superar os desafios cibernéticos**: Gerenciar recursos, manter sistemas críticos ativos e lidar com situações que testam a ética profissional.
- **Desafios técnicos complexos:** Desenvolver um firewall, implementar um proxy, criar um keylogger e realizar ataques de brute force simulados.
- **Fase final**: Na fase final, o jogador precisará utilizar suas habilidades para hackear um outro computador, provando seu domínio completo sobre técnicas de invasão e controle remoto.
- **Convencer o Hank**: Durante os interrogatórios, responder perguntas que avaliam a ética e a intenção das decisões tomadas ao longo do jogo.

---

## ⚙️ Requisitos

- **Windows**
- [Raylib](https://www.raylib.com/) baixada manualmente
- Acesso ao terminal `w64devkit` (incluso na pasta da Raylib/w64devkit)

---

## 🧭 Passo a Passo para Rodar

### 1. Baixe a Raylib

Acesse o site oficial da Raylib:

🔗 [https://www.raylib.com/](https://www.raylib.com/)

- Baixe a versão para **Windows**
- Extraia os arquivos e navegue até a pasta `/c/raylib/w64devkit`

---

### 2. Crie um atalho do terminal

- Dentro da pasta `w64devkit`, localize o arquivo `w64devkit.exe`
- Crie um **atalho** desse executável na sua **área de trabalho**
- Este terminal é onde você vai compilar e executar o projeto

---

### 3. Entre no terminal especial e rode o jogo

- Clique no atalho criado para abrir o terminal `w64devkit`
- No diretório do jogo, execute o comando:

```bash
make run
```
---

### ⚠️ Aviso Importante ⚠️

O executável do jogo deve estar no mesmo diretório que o arquivo **libcurl-x64.dll** para funcionar corretamente.  

- Para garantir isso, coloque o executável na pasta **/bin**.  
- Isso já é feito automaticamente ao utilizar o comando **make run**.
