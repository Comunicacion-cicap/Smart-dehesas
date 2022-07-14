import requests
import json
from datetime import datetime

url = "https://covap.cumulocity.com/measurement/measurements"
now = datetime.now()

payload = json.dumps({
  "c8y_RS232Measurement": {
    "Peso": {
      "value": 37,
      "unit": "Kg"
    }
  },
  "time": now,
  "source": {
    "id": "82339"
  },
  "type": "c8y_RS232Measurement"
})
headers = {
  'Authorization': 'Basic bWlndWVsLnZpbGxhZ3JhQGJhYmVsLmVzOjIwMjBDb3ZhcA==',
  'Content-Type': 'application/json',
  'Accept': 'application/vnd.com.nsn.cumulocity.measurement+json'
}

response = requests.request("POST", url, headers=headers, data=payload)