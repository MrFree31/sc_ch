import json
import subprocess
import os

# Путь к тестовому файлу receipt3.txt (лежит рядом со скриптом)
file = os.path.join(os.path.dirname(__file__) or ".", "receipt3.txt")

print(f"Обрабатывается: {file}")

try:
    # Получаем JSON от par.exe (он выводит в системной кодировке)
    out_c = subprocess.check_output(["par.exe", file], text=True, encoding='utf-8')
    data = json.loads(out_c)
    print(f"Содержание: {data['date']} | {data['shop']} | {data['amount']:.2f}")

    # Передаём JSON в base.exe через stdin в кодировке UTF-8
    res = subprocess.run(
        ["base.exe"],
        input=out_c,
        capture_output=True,
        text=True,
        encoding='utf-8'
    )
    if res.returncode != 0:
        print(f"Ошибка base.exe: {res.stderr.strip()}")
    else:
        print(res.stdout.strip())

except Exception as e:
    print(f"Ошибка: {e}")

print("The End")