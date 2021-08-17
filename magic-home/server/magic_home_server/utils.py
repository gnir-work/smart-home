import base64
import json
import time

from magic_home_server.consts import BASIC_DATA, BASIC_HEADERS, LOGIN_DATA, STATE_QUERY_HEADER, POWER_ON_HEADER, POWER_OFF_HEADER, \
    POWER_STATE_FIELD, LOGIN_SESSION_KEY


def _dict_to_bytes(data: dict) -> bytes:
    return bytes(json.dumps(data, separators=(',', ':')), 'ascii')


def _data_to_base64(header: bytes, data: dict) -> str:
    data_bytes = _dict_to_bytes(data)
    return base64.encodebytes(header + data_bytes).strip().decode()


def _generate_state_query_data():
    data = BASIC_DATA.copy()
    data['directive']['header']['timestamp'] = str(int(time.time()))
    data['directive']['payload']['data'] = _data_to_base64(header=STATE_QUERY_HEADER, data=dict())
    return json.dumps(data)


def _generate_power_on_data():
    data = BASIC_DATA.copy()
    data['directive']['header']['timestamp'] = str(int(time.time()))
    data['directive']['payload']['data'] = _data_to_base64(header=POWER_ON_HEADER, data={POWER_STATE_FIELD: 1})
    return json.dumps(data)


def _generate_power_off_data():
    data = BASIC_DATA.copy()
    data['directive']['header']['timestamp'] = str(int(time.time()))
    data['directive']['payload']['data'] = _data_to_base64(header=POWER_OFF_HEADER, data={POWER_STATE_FIELD: 0})
    return json.dumps(data)


def generate_power_on_request_params(login_session):
    data = _generate_power_on_data()
    headers = BASIC_HEADERS | {
        "Content-Length": str(len(data)),
        LOGIN_SESSION_KEY: login_session
    }
    return data, headers


def generate_power_off_request_params(login_session):
    data = _generate_power_off_data()
    headers = BASIC_HEADERS | {
        "Content-Length": str(len(data)),
        LOGIN_SESSION_KEY: login_session
    }
    return data, headers


def generate_login_request_params():
    headers = BASIC_HEADERS | {
        "Content-Length": str(len(LOGIN_DATA))
    }
    return LOGIN_DATA, headers


def generate_query_state_request_params(login_session):
    data = _generate_state_query_data()
    headers = BASIC_HEADERS | {
        "Content-Length": str(len(data)),
        LOGIN_SESSION_KEY: login_session
    }
    return data, headers
