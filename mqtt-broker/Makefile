ROOT_DIR:=$(shell pwd)
CURRENT_UID:=$(shell id -u)
CONTAINER_NAME:="mosquitto-broker"
PASSWORDS_FILE:="/mosquitto/config/passwd"


server:
	docker rm -f `docker ps -a -f name=$(CONTAINER_NAME) -q | xargs docker rm -f`
	docker run --name $(CONTAINER_NAME) -u $(CURRENT_UID) -p 1883:1883 -p 9001:9001 -v $(ROOT_DIR)/config:/mosquitto/config -d eclipse-mosquitto 


shell:
	docker exec -it $(CONTAINER_NAME) /bin/sh

logs:
	docker logs $(CONTAINER_NAME) -f

# Example use make user username=test password=12345
user:
	docker exec $(CONTAINER_NAME) touch $(PASSWORDS_FILE)
	docker exec $(CONTAINER_NAME) mosquitto_passwd -b $(PASSWORDS_FILE) $(username) $(password)
