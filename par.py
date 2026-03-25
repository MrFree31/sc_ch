import json, pandas, subprocess #Добавляем библиотеки для json, работы с excel, файлом С

out_c = subprocess.check_output(["par.exe","receipt2.txt"],text=True)

data = json.loads(out_c)

print(data["date"])
print(data["shop"])
print(data["amount"])
