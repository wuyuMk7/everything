#!/bin/bash

export FLASK_ENV=development
#FLASK_APP=main.py flask run --host=127.0.0.1 --port=13303
#FLASK_APP=main.py FLASK_DEBUG=1 flask run --host=0.0.0.0 --port=13303
FLASK_APP=main.py FLASK_DEBUG=1 flask run --host=0.0.0.0 --port=13303 --no-reload
