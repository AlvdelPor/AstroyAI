import requests

base_url = "https://api.open-data.cui.wroclaw.pl/od3-records/data/14"

def get_bus_data(side_number):
    response = requests.get(f"{base_url}/?{side_number}")
    if response.status_code == 200:
        needed_bus_data = response.json()
        print("success")
    else:
        return print(f"Failed to retrieve data {response.status_code}")
    return needed_bus_data

number = int(1900)
get_bus_data(number)