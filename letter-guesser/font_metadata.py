import requests

API_KEY = ""
url = f"https://www.googleapis.com/webfonts/v1/webfonts?key={API_KEY}"
response = requests.get(url)
fonts = response.json().get('items', [])

for font in fonts:
    print(font['family'])
