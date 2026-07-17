import requests

base_url = "https://api.open-data.cui.wroclaw.pl/od3-records/data/14"

def get_bus_data(side_number):
    needed_bus_data = None
    response = requests.get(base_url)
    mpk_data = response.json()
    while mpk_data["next"] is not None:
        if response.status_code == 200:
            mpk_data = response.json()
            for bus in range(len(mpk_data["results"])):
                if mpk_data["results"][bus]["data"]["Nr_Boczny"] == side_number:
                    needed_bus_data = mpk_data["results"][bus]
        else:
            print(f"Failed to retrieve data {response.status_code}")
        response = requests.get(mpk_data["next"])
        mpk_data = response.json()
    return needed_bus_data

def print_bus_data(bus_data):
    if bus_data:
        print(f"id: {bus_data["id"]}")
        print(f"numer boczny: {bus_data["data"]["Nr_Boczny"]}")
        print(f"linia: {bus_data["data"]["Nazwa_Linii"]}")
        print(f"długosc: {bus_data["data"]["Ostatnia_Pozycja_Dlugosc"]}")
        print(f"szerokosc: {bus_data["data"]["Ostatnia_Pozycja_Szerokosc"]}")
        print(f"aktualizacja: {str(bus_data["harvested_at"])[0:10] + " " + str(bus_data["harvested_at"])[11:19]}")

number = int(3015)
print_bus_data(get_bus_data(number))