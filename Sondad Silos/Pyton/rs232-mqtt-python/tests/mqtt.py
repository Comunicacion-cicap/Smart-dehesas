import paho.mqtt.client as mqtt

def on_publish(client,userdata,result):
	print("data published \n")

client=mqtt.Client(client_id="linux-0000000024647748", clean_session=True, protocol=mqtt.MQTTv311)
#client.on_connect = on_connect
#client.on_message = on_message
#client.on_publish = on_publish

username="t588701203/miguel.villagra@babel.es"
password="2020Covap"

client.username_pw_set(username,password)
client.connect("t588701203.cumulocity.com",1883,60)
TOPIC = "s/us"
client.publish(TOPIC, "200,c8y_RS232Measurement,Peso,86,Kg")