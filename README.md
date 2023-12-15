# Secure-Access-Control

Controle de acesso inteligente com comunicação via Bluetooth. Tem como objetivo fornecer segurança e otimize a gestão de autorizações em um ambiente de Smart Campus. Para alcançar tal feito, foi construído este repositório, responsável por configurar e estabelecer as conexões e processamentos necessários para que o controle seja feito corretamente.

## Componentes utilizados

- 1x ESP32-S3 + Fonte (5V)
- 1x LED + Resistor (220 Ω)
- 1x Relé + Fonte (5V)
- 1x BeagleBone Black + Fonte (5V)
- 1x Cabo Ethernet
- Aplicativo para smartphone
- Banco de dados Firebase

## Tecnologias/Bibliotecas utilizadas

### ESP32-32

| Nome da biblioteca                                            | Descrição                                                                                                                                                                                                                                                                                                                                                                                       |
| ------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| stdio.h                                                       | Biblioteca padrão de entrada/saída em C.                                                                                                                                                                                                                                                                                                                                                        |
| string.h                                                      | Biblioteca padrão de manipulação de strings em C.                                                                                                                                                                                                                                                                                                                                              |
| freertos/FreeRTOS.h                                           | Biblioteca de código aberto que implementa um sistema operacional de tempo real (RTOS) para microcontroladores. O RTOS é responsável por gerenciar o tempo e os recursos do microcontrolador, permitindo que vários programas sejam executados simultaneamente.                                                                                                                               |
| freertos/task.h                                               | A biblioteca FreeRTOS/task.h é uma parte específica da biblioteca FreeRTOS.h responsável por gerenciar as tarefas do sistema. Tarefas são unidades de código independentes que podem ser executadas simultaneamente por um sistema multitarefa como o FreeRTOS.                                                                                                                              |
| freertos/event_groups.h                                       | Usada para criar e gerenciar grupos de eventos no FreeRTOS. Os eventos são usados para sincronização entre tarefas.                                                                                                                                                                                                                                                                            |
| esp_event.h                                                   | Biblioteca relacionada a eventos no ESP-IDF (Espressif IoT Development Framework). Usada para gerenciar eventos no sistema.                                                                                                                                                                                                                                                                      |
| nvs_flash.h                                                   | Biblioteca para operações de armazenamento não volátil (NVS). Usada para armazenar configurações persistentes.                                                                                                                                                                                                                                                                              |
| esp_log.h                                                     | Biblioteca para logs no ESP-IDF. Usada para gerar logs em diferentes níveis (debug, info, erro, etc.).                                                                                                                                                                                                                                                                                           |
| esp_nimble_hci.h                                              | Biblioteca relacionada à configuração do controlador Bluetooth de baixa energia no ESP-IDF                                                                                                                                                                                                                                                                                                     |
| nimble/nimble_port.h<br />nimble/nimble_port_freertos.<br />h | Bibliotecas relacionadas ao NimBLE, uma pilha Bluetooth de baixa energia usada no ESP-IDF.                                                                                                                                                                                                                                                                                                        |
| host/ble_hs.h                                                 | Biblioteca relacionada ao NimBLE Host, que lida com o lado do host da pilha Bluetooth de baixa energia.                                                                                                                                                                                                                                                                                           |
| services/gap/ble_svc_gap.h<br />services/gatt/ble_svc_gatt.h  | Bibliotecas relacionadas aos serviços GAP (Generic Access Profile) e GATT (Generic Attribute Profile) no Bluetooth.                                                                                                                                                                                                                                                                              |
| sdkconfig.h                                                   | Arquivo gerado pelo ESP-IDF durante a compilação. Contém configurações específicas do projeto.                                                                                                                                                                                                                                                                                              |
| driver/gpio.h                                                 | Biblioteca para operações GPIO no ESP-IDF. Usada para configurar e controlar pinos GPIO.                                                                                                                                                                                                                                                                                                        |
| mqtt_client.h                                                 | Biblioteca de código aberto que implementa um cliente MQTT para microcontroladores. O MQTT é um protocolo de comunicação leve e eficiente que é amplamente utilizado em aplicações IoT.<br /><br />É compatível com uma ampla variedade de microcontroladores, incluindo Arduino, PIC, STM32 e ARM. Ela é relativamente fácil de usar e oferece uma ampla gama de recursos, incluindo: |

### Aplicativo - BLE Connect

| Tecnologia                           | Descrição                                                                                                                                                                                                                                                                                                                                                                                                                  |
| ------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Framework: React Native              | O React Native é uma biblioteca JavaScript de código aberto que permite criar aplicativos móveis para Android e iOS usando o React. Baseado no conceito de componentes, que são blocos de construção reutilizáveis que podem ser usados para criar interfaces de usuário complexas. Os componentes são renderizados pelo React Native, que é responsável por manter o estado da interface de usuário atualizado. |
| Linguagem: JavaScript                | JavaScript (JS) é uma linguagem de programação interpretada, estruturada, de script em alto nível com tipagem dinâmica fraca e multiparadigma. Juntamente com HTML e CSS, o JavaScript é uma das três principais tecnologias da World Wide Web.                                                                                                                                                                      |
| Biblioteca: react-native-ble-manager | Biblioteca de React Native para comunicação com dispositivos Bluetooth Low Energy (BLE). Ela fornece uma API simples e consistente para acessar os recursos BLE do dispositivo, permitindo que você crie aplicativos que se comuniquem com dispositivos BLE, como sensores, rastreadores de fitness e dispositivos IoT                                                                                                    |

### Beaglebone - Gateway (MQTT Server)

| Tecnologia             | Descrição                                                                                                                                                                                                                                                                                                                                                                                            |
| ---------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Linguagem: Python 3.11 | O Python é uma linguagem de programação popular para aplicações MQTT. Através da linguagem, será possível utilizar a biblioteca PahoMQTT para realizar a comunicação, envio e recebimento de mensagens pelo protocolo MQTT.                                                                                                                                                                  |
| Biblioteca: PahoMQTT   | Para usar o Python para uma aplicação MQTT utilizando PahoMQTT, o desenvolvedor precisa instalar a biblioteca PahoMQTT. Isso pode ser feito usando o gerenciador de pacotes do Python, pip. Após isso, será possível utilizar a biblioteca para realizar a comunicação, realizando a conexão com um IP e Porta para que o servidor possa estar ativo e esperando por recebimento de mensagens. |
| Serviço: Firebase     | Plataforma de desenvolvimento de aplicativos móveis e web que oferece uma variedade de serviços, o utilizado será o Banco de dados: O Firebase Realtime Database é um banco de dados NoSQL que permite que os dados sejam sincronizados em tempo real entre dispositivos e clientes. Será utilizado sempre que uma mensagem for recebida por MQTT.                                                |

## Funcionamento do Sistema

O sistema funciona da seguinte forma:

* **Etapa 1:** O usuário abre o aplicativo do sistema no smartphone.
* **Etapa 2:** O aplicativo gera um token único e o envia para o ESP32.
* **Etapa 3:** O ESP32 recebe o token e o envia para o BeagleBone.
* **Etapa 4:** O BeagleBone valida o token em um banco de dados hospedado na nuvem.
* **Etapa 5:** O BeagleBone retorna uma mensagem para o ESP32 indicando se o acesso foi autorizado ou negado.
* **Etapa 6:** O ESP32 recebe a mensagem, libera o acesso e a exibe no smartphone do usuário.

### ESP32-S3

Para usar nosso aplicativo, o usuário deve ter a extensão Espressif IDF instalada no VSCode e configurado na versão 4.4.6. Depois de carregar o arquivo modelo, é necessário ativar o Bluetooth e definir o Nimble como Host nas configurações do esp-idf para garantir o funcionamento adequado.

Nosso código possui algumas funções que precisam ser inicializadas no arquivo *main* que são:

1. **init_led()**

   - Sem parâmetros.
   - Ela configura os pinos GPIO do relé e do led que serão utilizados.
   - Inicia o pino do relé em nível lógico 1, porque ele é ativado em nível lógico 0.
2. **connectBLE()**

   - Sem parâmetros.
   - Inicia o Bluetooth no ESP, definindo nome, UUID, etc.
   - Configura serviços e características do Bluetooth, finalizando a inicialização.

Além disso, existem duas funções principais para receber e enviar dados do celular:

1. **static int device_write()**

   - Ativada sempre que o usuário envia dados para o serviço do ESP por um aplicativo.
   - Salva esses dados em uma variável que será enviada para o BeagleBone via MQTT.
   - Possui alguns parâmetros que não precisam ser definido pelo usuário como o ‘uint16_t conn_handle’ que identifica a conexao com o dispositivo, ‘uint16_t attr_handle’ que identifica o atributo específico ao qual se deseja acessar ou escrever dados.
   - ‘struct ble_gatt_access_ctxt *ctxt’ é um ponteiro para a estrutura que contém informação e o ‘void *arg’ é apenas mais um ponteiro genérico que pode conter informações adicionais.
   - Após salvar o dado em uma variável ele instância a função ‘enviarMensagemMQTT’.
2. **void enviarMensagemMQTT()**

   - Recebe um dado (mensagem) que é enviada pelo smartphone no parâmetro.
   - Inicializa uma conexão como cliente para o servidor MQTT estabelecido nas configurações (*mqtt_cfg*).
   - Realiza o delay da task por 2 segundos para garantir que a conexão seja realiza e faz a publicação da mensagem, enviando *cliente*, *tópico da mensagem* e *a mensagem*. No caso, será enviado o tópico "*token*/access", juntamente com a mensagem sendo composta pelo **token** a ser validado.
   - Realiza o delay da task por 1 segundo para garantir o envio da mensagem corretamente e confirmação da entrega.
   - Fecha a conexão por MQTT por segurança.
3. **int esp_mqtt_client_wait_subscribe()**

   - Recebe o *cliente* configurado com IP e Porta configurados referente ao BeagleBone, assim como o *tópico de resposta* esperado.
   - A função tem como objetivo esperar por uma resposta por MQTT para o tópico esperado se é recebido um "OK".
   - Caso a mensagem seja "OK", será chamada a função *mensagemMQTTCallback*.
4. **void mensagemMQTTCallback()**

   - Recebe o *evento* como parâmetro, onde está guardado as informações de recebimento da mensagem.
   - A função tem como objetivo verificar se a mensagem recebida é "ok", representando que o BeagleBone validou o acesso ao token enviado.
   - Caso seja verdadeiro a verificação, então o Relé e o LED serão ativados, liberando o acesso, é feito um delay por 100 millisegundos, e é desativado em seguida.
   - Caso contrário, então o Relé e o LED serão desativados.

### Aplicativo - BLE Connect

Para utilizar nosso aplicativo móvel, os usuários precisam ter um dispositivo Android e instalar o APK. No caso de dispositivos com Android superior à versão 11, é necessário ativar manualmente a permissão para dispositivos próximos. Ao abrir o aplicativo, os usuários se deparam com um botão para escanear dispositivos próximos, que quando clicado listará os dispositivos próximos disponíveis. Depois disso, basta clicar no dispositivo desejado para conectar. Uma vez conectado, uma tela é exibida com um botão para enviar um token para o dispositivo conectado. Posteriormente, os usuários podem desconectar-se pressionando o botão "Desconectar" localizado abaixo do botão de envio do token.

Criamos quatro arrow functions essenciais para o componente *react-native-ble-manager*:

1. **startScan()**

   - Sem parâmetros.
   - Utilizada para escanear aplicativos próximos, exibindo por padrão o nome e o ID de cada dispositivo Bluetooth encontrado.
   - Cada dispositivo descoberto é adicionado a uma lista na tela, permitindo que o usuário escolha com qual deseja se conectar.
2. **ConnectDevice(deviceId)**

   - Recebe como parâmetro o ID do dispositivo, automaticamente obtido quando o usuário seleciona o dispositivo desejado para conexão.
   - Exibe no console uma mensagem confirmando ou negando a conexão ao dispositivo.
3. **sendTextFile()**

   - Sem parâmetros, pois o token está armazenado em uma variável global, gerando um token exclusivo com base no ID do smartphone.
   - Converte o token (string) para uma matriz de dados antes de enviá-lo para o ESP.
   - Utiliza a função 'writeWithoutResponse' da biblioteca 'ble-manager' para enviar a matriz de dados. Com o dispositivo já conectado, obtém o ID, UUID de serviço e UUID de característica do dispositivo, enviando o token quando o usuário pressiona o botão de envio.4. disconnectDevice()
4. **disconnectDevice()**

   - Não recebe parâmetros.
   - Altera o estado de conexão da biblioteca para null, desconectando o aplicativo do dispositivo ESP.

### BeagleBone - Gateway (MQTT Server)

O repositório para nosso gateway (BeagleBone) foi desenvolvido utilizando Python. Nele, utilizamos duas bibliotecas, a primeira PahoMQTT, para realizar a comunicação, receber e enviar mensagems pelo protocolo MQTT, e a segunda Firebase, para realizar comunicação com o banco de dados da plataforma para verificar se o token recebido é válido ou não.

Para isso, é necessário possuir Python3.11 ou superior, além de instalar as bibliotecas paho-mqtt e firebase, utilizando o gerenciador de pacotes pip. Após realizar a instalações das dependências necessárias, será necessário configurar as variáveis de ambiente: IP e Porta para recebimento/envio de informações por MQTT e os tópicos de envio e resposta para comunicação.

Com o repositório configurado, basta executar. O seu funcionamento está organizado com as funções:

1. **connect_mqtt()**

   - Esta função tenta estabelecer uma conexão com o broker MQTT.
   - Define um callback para ser chamado durante a conexão.
   - Cria um cliente MQTT com o ID gerado.
   - Registra o callback para o evento de conexão.
   - Tenta se conectar ao broker no endereço e porta definidos.
   - Retorna o cliente MQTT conectado.
2. **subscribe()**

   - Esta função inscreve o cliente MQTT para receber mensagens no tópico esperado para tokens de acesso.
   - Define um callback para ser chamado quando receber uma mensagem. No callback, será feito a conexão com o banco de dados, verificação dos tokens existentes e validação se o token recebido está entre eles. Caso positivo, será publicado "ok" como resposta; caso contrário, será publicado "fail" como resposta.
   - Inscreve o cliente no tópico MQTT_TOPIC_RESPONSE.
   - Registra o callback para o evento de recebimento de mensagens.
3. **run()**

   - Esta função principal configura e inicia o cliente MQTT.
   - Cria um cliente MQTT usando a função connect_mqtt.
   - Inscreve o cliente no tópico de recebimento usando a função subscribe.
   - Entra em um loop infinito esperando e processando mensagens recebidas.

**Link para documentação do projeto**: https://drive.google.com/file/d/1zjrZ5Pq0EkRmKUNS3rj-Z2lkJL8zm91b/view?usp=drive_link

**Link para vídeo de demonstração**: https://youtu.be/WXUR6n89Z58
