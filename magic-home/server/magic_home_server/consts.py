# Add the secrets.py containing
from .secrets import SECRET_HEADERS, LOGIN_DATA

HOST = "13.52.16.214"
BASE_URL = f"https://{HOST}"

LICENSE = "k2WA9WdfOwTbUk9uL1bwIthu0NMKfA1XTA0fJCEsj8bIGj3uKcPlNtQznUZCj9VB0k40WwAAAACdkYIAqg9mavVJGJuUszhzang8QeW4N77JTP3Xj9VU6lc18FZO/qA3UPxYRw5QnuQjvw7ULdRaQ3wHxCDH65IFRKd5NnYqrioGLurgNsJ49AAAAAA="

LOGIN_URL = f"{BASE_URL}/account/login"
CONTROL_URL = f"{BASE_URL}/device/control/v2/sdkcontrol?license={LICENSE}"

LOGIN_SESSION_KEY = "loginsession"

POWER_STATE_FIELD = "pwr"

STATE_QUERY_HEADER = b'\xa5\xa5ZZ\xb3\xc1\x01\x0b\x02\x00\x00\x00'
POWER_ON_HEADER = b'\xa5\xa5ZZ\xc3\xc3\x02\x0b\t\x00\x00\x00'
POWER_OFF_HEADER = b'\xa5\xa5ZZ\xc2\xc3\x02\x0b\t\x00\x00\x00'

BASIC_HEADERS = {
                    "Content-type": "application/x-java-serialized-object",
                    "system": "android",
                    "appPlatform": "android",
                    "language": "en-us",
                    "timestamp": "1623459001",
                    "User-Agent": "Dalvik/2.1.0 (Linux; U; Android 11; sdk_gphone_x86_arm Build/RSR1.201013.001)",
                    "Host": "app-service-usa-acf15bd5.ibroadlink.com",
                    "Connection": "Keep-Alive",
                    "Accept-Encoding": "gzip",
                } | SECRET_HEADERS

BASIC_DATA = {
    "directive": {
        "header": {
            "namespace": "DNA.TransmissionControl", "name": "commonControl", "interfaceVersion": "2",
            "messageId": "00000000000000000000a043b00afcec-1623499153", "timstamp": 0
        },
        "endpoint": {
            "devicePairedInfo": {
                "did": "00000000000000000000a043b00afcec", "pid": "000000000000000000000000c8600000",
                "mac": "a0:43:b0:0a:fc:ec",
                "cookie": "eyJkZXZpY2UiOnsiaWQiOjEsImtleSI6IjU4NDBhZjNmNGYyYzE3NDY5YzkyNDA0MjNmNzMyYjM0IiwiYWVza2V5IjoiNTg0MGFmM2Y0ZjJjMTc0NjljOTI0MDQyM2Y3MzJiMzQiLCJkaWQiOiIwMDAwMDAwMDAwMDAwMDAwMDAwMGEwNDNiMDBhZmNlYyIsInBpZCI6IjAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMGM4NjAwMDAwIiwibWFjIjoiYTA6NDM6YjA6MGE6ZmM6ZWMifX0="
            },
            "endpointId": "00000000000000000000a043b00afcec", "cookie": {}
        },
        "payload": {
            "data": "",
            "notpadding": 0
        }
    }
}
