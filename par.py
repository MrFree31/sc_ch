import json, pandas, subprocess #Добавляем библиотеки для json, работы с excel, файлом С
from utils import img_get_main as IGM# Программа для обработки изображений от @askolenko_1
from utils.img_get_main import convert_folder
import os
import pytesseract
pytesseract.pytesseract.tesseract_cmd = r"D:\Programm Files\Tesseract-OCR\tesseract.exe"
#Отключение удаления текстовиков, чтобы видеть, насколько плохо тессеракт с данными настройками сканирует текст
DEBUG = True

#Чек в скан
convert_folder(r"C:\Users\koner\Documents\GitHub\sc_ch\checks",r"C:\Users\koner\Documents\GitHub\sc_ch\scans")
# r"..." - "сырая" строка от ошибок с косыми палками

#Берём date,shop,amount во всех .txt из папки
scan_folder = r"C:\Users\koner\Documents\GitHub\sc_ch\scans"
for filename in os.listdir(scan_folder):
    if not filename.endswith(".txt"):
        continue
    file = os.path.join(scan_folder, filename)
    print(f"В работе адрес: {file}")

    try: #Обработаем ошибки
        out_c = subprocess.check_output(["par.exe",file],text=True,encoding='utf-8')# Кодировка полетела, фиксируем какую надо
        data = json.loads(out_c) #Преобразование в словарь
        print(f"Содержание файла: {data['date']} | {data['shop']} | {data['amount']:.2f}")
    except Exception as e:
        print(f"Ошибка! при парсинге {file}: {e}")
        continue #Пропуск не читаемого файла

    #base.exe, передавая JSON через stdin(Кавычки)
    try: #Для подачи JSON в base применяем stdin, для этого используем run вместо check_output
        res = subprocess.run(["base.exe"], input = out_c, capture_output=True, text=True, encoding='utf-8') #input - ввод переменной stdin(тот самый); capture_output - "ловушка" для stdout, stderr
        if res.returncode != 0: #Обработка ответа base.exe. Может выдать ошибки, то есть не выполнилась программа, но нам нужен результат!
            print(f"Ошибка! Файл base: {res.stderr.strip()}")
        else:
            print(f"{res.stdout.strip()}")#strip чистит от символов перевода, пробелов и прочего
    except Exception as e:
        print(f"Ошибка! base.exe: {e}")
        continue #Пропуск не читаемого файла

    if not DEBUG:#Если включена отладка, то не удаляем файл, а удаляем для очистки диска
        os.remove(file)

#print(data["date"])
#print(data["shop"])
#print(data["amount"])
print("The End")