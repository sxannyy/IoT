from fastapi import FastAPI
from hashlib import md5
from datetime import datetime
from loguru import logger

logger.add('logs/logs.log')

app = FastAPI()

logger.info("Успешно запщуен сервер")

@app.get('/get_id')
async def get_id():
    logger.info("Вызван метод get_id()")
    return{md5(str(datetime.now()).encode('utf-8')).hexdigest()}

@app.get('/get_topic')
async def get_id():
    logger.info("Вызван метод get_topic()")
    return{"lab/kich_sanzh/sensor/state"}
