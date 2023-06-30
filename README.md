# Comunicação Zigbee (Esp32 + Xbee modules)
Desenvolvi diversas aulas sobre protocolo Zigbee, aqui você encontrará conteúdos sobre configurações de módulos Xbee usando Xctu, interpretação de pacotes AT, API, leitura de dados analógicos e digitais de diversos sensores em uma rede mesh, e por fim, desenvolvi um projeto de um Gateway concentrador Zigbee com módulo Eso32 reunindo todo o conhecimento passado nestes conteúdos. 

| Tutoriais/Títulos  | Links/Vídeos YouTube | Links/Códigos fonte |
| --- | :---: | :---: |
| Xbee - Pacotes API e AT / Rede Mesh (Aula 01) | [Vídeo aula](https://youtu.be/zsopTptUtmU) | --- |
| Xbee - Configurando Coordinator/Router (Aula 02) | [Vídeo aula](https://youtu.be/tpphkIBAoxQ) | --- |
| Xbee - Interpretando pacotes (Aula3) | [Vídeo aula](https://youtu.be/K-Xh6O4zVpY) | --- |
| Xbee - Interpretando pacotes (Aula4)  | [Vídeo aula](https://youtu.be/ennm0YBwX60) | --- |
| Xbee/Esp32 - Lendo pacotes analógicos/digitais na prática (Aula 5)  | [Vídeo aula](https://youtu.be/Ldsz7II8Jj8) | --- |
| Xbee/Esp32 - Enviando comandos AT em pacotes API (Aula 6) | [Vídeo aula](https://youtu.be/49fqidD_KXY) | --- |

# Gateway concentrador zigbee
Unindo todo conteúdo acima criei um Gateway concentrador zigbee, que consiste em um Xbee coordenador comandando uma rede mesh com vários módulos routers e end devices enviando e recebendo pacotes de dados analógicos e digitais. Um ESP32 faz a concentração dos dados da rede zigbee e envia em uma rede Ethernet/Wifi para um broquer MQTT que faz a leitura/envio dos dados coletados na rede. Confira abaixo o vídeo do funcionamento deste projeto.

| Tutoriais/Títulos  | Links/Vídeos YouTube | Links/Códigos fonte |
| --- | :---: | :---: |
| Xbee/Esp32 - Gateway concentrador Zigbee (Projeto Final) | [Vídeo aula](https://youtu.be/Ki-BkXGZ5OA) | --- |


<hr>

[<< Voltar para página inicial](https://github.com/dev-daniel-amorim)
