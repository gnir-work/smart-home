from fastapi import FastAPI
from pydantic import BaseModel
from magic_home_server.consts import POWER_STATE_FIELD
from magic_home_server.sdk import MagicHomeSdk

app = FastAPI()
magic_home_sdk = MagicHomeSdk()


class PowerState(BaseModel):
    is_on: bool


def _check_action_response(response):
    if response.status_code == 200:
        return {"success": True}
    else:
        return {"success": False}


@app.get('/')
async def index():
    return "Hello There"


@app.get("/power", response_model=PowerState)
async def power():
    state = magic_home_sdk.query_state()
    return PowerState(is_on=state[POWER_STATE_FIELD] == 1)


@app.post("/power")
async def power(power_state: PowerState):
    if power_state.is_on:
        res = magic_home_sdk.power_on()
    else:
        res = magic_home_sdk.power_off()

    res.raise_for_status()
    return _check_action_response(res)

