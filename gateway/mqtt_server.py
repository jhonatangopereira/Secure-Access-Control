import random
from paho.mqtt import client as mqtt_client
import os
from dotenv import load_dotenv


BROKER_ID = os.getenv("BROKER_ID")
PORT = os.getenv("PORT")
#USERNAME = os.getenv("USERNAME")
#PASSWORD = os.getenv("PASSWORD")

topic = "python/mqtt"
client_id = f"python-mqtt:{random.randint(0, 100)}"

def connect_mqtt():
	def on_connect(client, userdata, flags, rc):
		if rc == 0:
			print("Connected to MQTT Broker!")
		else:
			print("Failed to connect")

	client = mqtt_client.Client(client_id)
	client.on_connect = on_connect
	client.connect(BROKER_ID, PORT)
	return client

def subscribe(client):
	def on_message(client, userdata, msg):
		print(f"Received {msg.payload.decode()} from {msg.topic}")

	client.subscribe(topic)
	client.on_message = on_message

def run():
	client = connect_mqtt()
	subscribe(client)
	client.loop_forever()

if __name__ == "__main__":
	run()
