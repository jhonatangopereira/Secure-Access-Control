# Importa bibliotecas necessárias
import random
from paho.mqtt import client as mqtt_client
import os
from dotenv import load_dotenv
import firebase

load_dotenv()

# Carrega variáveis de ambiente
BROKER_ID = os.getenv("BROKER_ID")
PORT = os.getenv("PORT")

# Define tópicos MQTT para envio e recebimento de mensagens
MQTT_TOPIC_RESPONSE = "token/access"  # Tópico esperado para receber tokens
MQTT_TOPIC_REQUEST = "token/response"  # Tópico usado para enviar respostas ("ok" ou "fail")

# Gera um ID aleatório para o cliente MQTT
client_id = f"python-mqtt:{random.randint(0, 100)}"


def connect_mqtt():
    """
    Esta função tenta estabelecer uma conexão com o broker MQTT.
    * Define um callback para ser chamado durante a conexão.
    * Cria um cliente MQTT com o ID gerado.
    * Registra o callback para o evento de conexão.
    * Tenta se conectar ao broker no endereço e porta definidos.
    * Retorna o cliente MQTT conectado.
    """

    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Conectado ao broker MQTT!")
        else:
            print("Falha ao conectar-se ao broker!")

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.connect(BROKER_ID, PORT)
    return client


def subscribe(client):
    """
    Esta função inscreve o cliente MQTT para receber mensagens
    no tópico esperado para tokens de acesso.
    * Define um callback para ser chamado quando receber uma mensagem.
    * Inscreve o cliente no tópico MQTT_TOPIC_RESPONSE.
    * Registra o callback para o evento de recebimento de mensagens.
    """

    def on_message(client, userdata, msg):
        """
        Este callback é chamado quando o cliente recebe uma mensagem.
        * Extrai o token da mensagem recebida.
        * Imprime informações sobre a mensagem recebida.
        * Inicializa uma instância do FirebaseApp.
        * Obtém a referência ao nó "tokens" do banco de dados Firebase.
        * Busca o token recebido dentro do nó "tokens".
        * Verifica se o token foi encontrado no banco de dados.
            * Se encontrado, publica "ok" no tópico de resposta.
            * Se não encontrado, publica "fail" no tópico de resposta.
        """

        token = msg.payload.decode()
        print(f"Recebido token {token} do tópico {msg.topic}")

        firebase_app = firebase.FirebaseApp.initialize_app()
        database = firebase_app.database()
        token_data = database.child("tokens").child(token).get()

        if token_data is not None:
            print("Token presente")
            client.publish(MQTT_TOPIC_REQUEST, "ok")
        else:
            print("Token não presente")
            client.publish(MQTT_TOPIC_REQUEST, "fail")

    client.subscribe(MQTT_TOPIC_RESPONSE)
    client.on_message = on_message


def run():
    """
    Esta função principal configura e inicia o cliente MQTT.
    * Cria um cliente MQTT usando a função connect_mqtt.
    * Inscreve o cliente no tópico de recebimento usando a função subscribe.
    * Entra em um loop infinito esperando e processando mensagens recebidas.
    """

    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()


if __name__ == "__main__":
    run()

