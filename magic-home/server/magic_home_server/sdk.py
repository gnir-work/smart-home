import json
import base64
import cachetools.func
import requests
from datetime import timedelta

from magic_home_server.consts import LOGIN_URL, CONTROL_URL, STATE_QUERY_HEADER, LOGIN_SESSION_KEY
from magic_home_server.utils import generate_power_off_request_params, generate_power_on_request_params, \
    generate_login_request_params, generate_query_state_request_params

LOGIN_SESSION_TTL = timedelta(hours=1).total_seconds()


class MagicHomeSdk:
    def __init__(self):
        self.session = requests.session()

    @property
    def login_session(self):
        return self._get_login_session()

    def power_off(self):
        data, headers = generate_power_off_request_params(self.login_session)
        return self.session.post(CONTROL_URL, data=data, verify=False, headers=headers)

    def power_on(self):
        data, headers = generate_power_on_request_params(self.login_session)
        return self.session.post(CONTROL_URL, data=data, verify=False, headers=headers)

    def query_state(self):
        data, headers = generate_query_state_request_params(self.login_session)
        res = self.session.post(CONTROL_URL, data=data, verify=False, headers=headers)
        return json.loads(
            base64.decodebytes(bytes(res.json()['event']['payload']['data'], "ascii"))[len(STATE_QUERY_HEADER):])

    @cachetools.func.ttl_cache(ttl=LOGIN_SESSION_TTL)
    def _get_login_session(self):
        login_response = self._login()
        return login_response.json()[LOGIN_SESSION_KEY]

    def _login(self):
        data, headers = generate_login_request_params()
        return self.session.post(LOGIN_URL, data=data, verify=False, headers=headers)


if __name__ == '__main__':
    magic_home_sdk = MagicHomeSdk()
    state = magic_home_sdk.query_state()
    print(state)
