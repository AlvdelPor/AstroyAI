# Print
    print("sth") # shows "sth" in the terminal
    print("sth", end = " ") # ptint at the same line, after " "

# Variables
    # a container for a value (string, integer, float, boolean)

    # Strings (tekst)
    first_name: str = "Maciej"
    plane: str = "b717"

    # Integers (liczba całkowita)
    age: int = 18
    quantity: int = 3

    # Float (liczba wymierna zmiennoprzecinkowa)
    pi: float = 3.1415
    price: float = 5.13

    # Boolean (true / false)
    is_student: bool = True
    for_sale: bool = False

    # Other
    x, y, z = "Orange", "Banana", "Cherry"
    myName = MyName = my_name = "Maciej"

# F-string
    # the way to include an variable into text
    print(f"Mam {age} lat")

# Typecasting
    # the process of converting a variable from one data type to another, str(), int(), float(), bool()

    name = "Bro Code"
    age = 25
    gpa = 3.2
    is_Student = True

    gpa = int(gpa)
    is_Student = int(is_Student)

    print(type(age)) # type - shows the type of variable

# Input
    # a function that prompts the user to enter data, returns the entered data as a string

    name = input("The name is ")
    print(f"Hello {name}")
    age = int(input("The age is "))
    print(f"You are {age + 1}")

# Arithmetic operators
    # Addition
    age = age + 24
    age += 2

    # Subtraction
    age = age - 3
    age -= 3

    # Multiplication
    age = age * 3
    age *= 3

    # Division
    age = age / 2
    age /= 2

    # Exponentiation (potęgowanie)
    age = age ** 2
    age **= 2

    # Remainder (reszta)
    age = age % 2
    age %= 2

    # Root (pierwiastkowanie)
    age = age ** 0.5
    age **= 0.5

    # Integer division (dzielenie całkowite)
    age = age // 2
    age //= 2

# Math functions
    # Round
    x = 3.1415
    result_a = round(x, 3) # zaokrąglanie do danej liczby miejsc po przecinku

    # Absolute value
    x = -4
    result = abs(x)

    # Exponentiation (podnoszenie do danej potęgi)
    result_b = pow(x, 4)

    # Maximum value
    x = 3.15
    y = 10
    z = 2
    result_c = max(x, y, z)

    # Minimum value
    result_d = min(x, y, z)

    import math
    print(math.pi)
    print(math.e)
    result_e = math.sqrt(x) # Pierwiastek kwadratowy
    result_f = math.ceil(x) # Zaokrągla float zawsze w górę
    result_g = math.floor(x) # Zaokrągla float zawsze w dół

# IF statement
    if x > 18:
        print(True)
    elif x < 0:
        print("error")
    else:
        print(False)

    online = False
    if online:
        print(f"Status is online")
    else:
        print(f"Status is offline")