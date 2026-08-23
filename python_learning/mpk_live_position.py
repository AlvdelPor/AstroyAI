# mpk wroclaw live vehicles position API
import requests

base_url = "https://api.open-data.cui.wroclaw.pl/od3-records/data/14"

def get_vehicle_data(line_number):
    needed_vehicle_data = []
    i = int(0)
    response = requests.get(base_url)
    mpk_data = response.json()
    while mpk_data["next"] is not None:
        if response.status_code == 200:
            for vehicle in range(len(mpk_data["results"])):
                if mpk_data["results"][vehicle]["data"]["Nazwa_Linii"] == line_number:
                    needed_vehicle_data.append(mpk_data["results"][vehicle])
        else:
            print(f"Failed to retrieve data {response.status_code}")
        response = requests.get(mpk_data["next"])
        mpk_data = response.json()
    return needed_vehicle_data

def print_vehicle_data(vehicle_data):
    if vehicle_data:
        i = int(0)
        for i in range(len(vehicle_data)):
            print(f"id: {vehicle_data[i]["id"]}")
            print(f"numer boczny: {vehicle_data[i]["data"]["Nr_Boczny"]}")
            print(f"linia: {vehicle_data[i]["data"]["Nazwa_Linii"]}")
            print(f"długosc: {vehicle_data[i]["data"]["Ostatnia_Pozycja_Dlugosc"]}")
            print(f"szerokosc: {vehicle_data[i]["data"]["Ostatnia_Pozycja_Szerokosc"]}")
            print(f"aktualizacja: {str(vehicle_data[i]["harvested_at"])[0:10] + " " + str(vehicle_data[i]["harvested_at"])[11:19]}")
            print("")

line = str(715)
print_vehicle_data(get_vehicle_data(line))