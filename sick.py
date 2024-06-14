from flask import Flask, request, jsonify
import paho.mqtt.client as mqtt

app = Flask(__name__)
data_list = []
temp_data = {}

MQTT_BROKER = "c195ef0f5f534d53b72397d524b180f3.s1.eu.hivemq.cloud"
MQTT_PORT = 8883
MQTT_SUHU = "sensor/suhu"
MQTT_LEMBAB = "sensor/lembab"
MQTT_USERNAME = "hivemq.webclient.1718283629828"
MQTT_PASSWORD = "3bRf;8VvK#qd7$AH0<Mu"

def on_connect(client, userdata, flags, rc):
    print(f"nyambungin {rc}")
    client.subscribe(MQTT_SUHU)
    client.subscribe(MQTT_LEMBAB)

def on_message(client, userdata, msg):
    global temp_data, data_list
    payload = msg.payload.decode()
    topic = msg.topic
    print(f"Received message '{payload}' on topic '{topic}'")
    
    if topic == MQTT_SUHU:
        temp_data['suhu'] = str(payload)
        
    elif topic == MQTT_LEMBAB:
        temp_data['kelembapan'] = str(payload)
    
   
    if 'suhu' in temp_data and 'kelembapan' in temp_data:
        data = {
            'suhu': temp_data['suhu'],
            'kelembapan': temp_data['kelembapan']
        }
        data_list.append(data)
        print(f"Appended data: {data}")
        temp_data = {} 

client = mqtt.Client()
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
client.on_connect = on_connect
client.on_message = on_message
client.tls_set()

client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_start()

@app.route('/sensor/data', methods=['POST'])
def add_dummy_data():
    readings = request.json.get('readings')
    print(f"Received readings: {readings}")

    if not readings:
        return jsonify({"error": "ilang"}), 400

    for reading in readings:
        temperature = reading.get('suhu')
        humidity = reading.get('kelembapan')

        if temperature is None or humidity is None:
            return jsonify({"error": "Missing data in one of the readings"}), 400

        dummy_data = {
            "suhu": temperature,
            "kelembapan": humidity,
        }
        data_list.append(dummy_data)
        print(f"Appended data: {dummy_data}")

    print(f"Final data list: {data_list}")
    return jsonify({"message": "Data telah dikirim"}), 200

@app.route('/sensor/data', methods=['GET'])
def get_data():
    print(f"Meminta Data: {data_list}")
    return jsonify(data_list), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)