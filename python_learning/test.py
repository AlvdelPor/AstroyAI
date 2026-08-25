def calculator():
    sign: str = ""
    result: float = float(input())
    while sign != "=":
        sign: str = str(input())
        if sign != "=":
            number: float = float(input())
            if sign == "+":
                result += number
            elif sign == "-":
                result -= number
            elif sign == "*":
                result *= number
            elif sign == "/":
                result /= number
            else:
                return "sign_error"
        else:
            return result
    return None

if __name__ == "__main__":
    print("---ULTIMATE CALCULATOR---")
    print(calculator())
