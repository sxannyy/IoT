from fastapi import FastAPI
from hashlib import md5
from datetime import datetime
from loguru import logger

logger.add('logs/logs.log')

app = FastAPI()

logger.info("Успешно запщуен сервер")

def generate_id():
    return md5(str(datetime.now()).encode('utf-8')).hexdigest()

id = generate_id()

@app.get('/get_server_topic')
async def get_server_topic():
    return f"lab/esp8266/{id}"
