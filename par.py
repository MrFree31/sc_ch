import json, pandas, subprocess #Добавляем библиотеки для json, работы с excel, файлом С
from utils import img_get_main as IGM# Программа для обработки изображений от @askolenko_1
from utils.img_get_main import convert_folder
import pytesseract
pytesseract.pytesseract.tesseract_cmd = r"D:\Programm Files\Tesseract-OCR\tesseract.exe"
convert_folder(r"C:\Users\koner\Documents\GitHub\sc_ch\checks",r"C:\Users\koner\Documents\GitHub\sc_ch\scans")
# r"..." - "сырая" строка от ошибок с косыми палками
out_c = subprocess.check_output(["par.exe",r"C:\Users\koner\Documents\GitHub\sc_ch\scans\Receipt0.txt"],text=True,encoding='utf-8')# Кодировка полетела, фиксируем какую надо
data = json.loads(out_c)

print(data["date"])
print(data["shop"])
print(data["amount"])
