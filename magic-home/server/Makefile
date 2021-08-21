CONTAINER_NAME := magic-home-server-instance
IMAGE_NAME := magic-home-server

get-poetry:
	curl -sSL https://raw.githubusercontent.com/python-poetry/poetry/master/get-poetry.py | python -

install:
	poetry install

dev: install
	poetry run uvicorn magic_home_server.server:app --reload --host 0.0.0.0

image:
	docker build -t $(IMAGE_NAME) .

stop:
	docker ps -q --filter "name=$(CONTAINER_NAME)" | grep -q . && docker stop $(CONTAINER_NAME) || echo "No container to stop"
	docker ps -a -q --filter "name=$(CONTAINER_NAME)" | grep -q . && docker rm $(CONTAINER_NAME) || echo "No container to remove"

start: image
	docker run --name $(CONTAINER_NAME) --restart unless-stopped -d -p 8000:8000 $(IMAGE_NAME)
