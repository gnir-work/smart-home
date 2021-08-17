FROM python:3.9.5-alpine

RUN mkdir /magic_home_server

RUN wget https://raw.githubusercontent.com/python-poetry/poetry/master/get-poetry.py && python get-poetry.py

COPY pyproject.toml /magic_home_server

COPY poetry.lock /magic_home_server

WORKDIR /magic_home_server

RUN ~/.poetry/bin/poetry install

COPY magic_home_server/ /magic_home_server/magic_home_server

CMD ["/root/.poetry/bin/poetry", "run", "uvicorn", "magic_home_server.server:app", "--host", "0.0.0.0"]