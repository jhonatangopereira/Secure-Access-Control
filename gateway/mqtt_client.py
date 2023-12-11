from paho.mqtt import client as mqtt_client
from dotenv import load_dotenv
import os
import random
import time

load_dotenv()

BROKER_ID = os.getenv("BROKER_ID")
PORT = int(os.getenv("PORT"))
#USERNAME = os.getenv("USERNAME")
#PASSWORD = os.getenv("PASSWORD")

topic = "python/mqtt"
client_id = f"Python-mqtt-{random.randint(0, 1000)}"


# connect to MQTT client
def connect_mqtt():
	def on_connect(client, userdata, flags, rc):
		if rc == 0:
			print("Connected to MQTT Broker")
		else:
			print(f"Failed to connect, return code {rc}\n")

	# set connecting client ID
	client = mqtt_client.Client(client_id)
#	client.username_pw_set(USERNAME, PASSWORD)
	client.on_connect = on_connect
	client.connect(BROKER_ID, PORT)
	return client

def publish(client):
	msg_count = 1
	while True:
		time.sleep(5)
		msg = f"messages: {msg_count}"
		client.subscribe(f"{topic}/{msg}")
		result = client.publish(topic, msg)
		status = result[0]
		if status == 0:
			print(f"Send {msg} to topic {topic}")
		else:
			print(f"Failed to send message to topic {topic}")
		msg_count += 1

def run():
	client = connect_mqtt()
	client.loop_start()
	publish(client)
	client.loop_stop()


if __name__ == "__main__":
	run()
