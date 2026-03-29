import os
# Библиотека os, необходимая для работы с директориями, файлами компьютера пользователя

import shutil
# Библиотека, необходимая для удаления директории, в которую сохраняются кадры видео

import cv2
# Библиотека, предназначенная для анализа, классификации и обработки изображений.

import pytesseract
# Библиотека, которая является оболочкой для движка Google Tesseract-OCR .

from PIL import Image

'''
Модуль Image предоставляет класс с тем же именем, который используется для представления изображения PIL.
Модуль также предоставляет ряд заводских функций, в том числе функции загрузки изображений из файлов
и создания новых изображений.
'''

### Функция, необходимая для выбора пользователем дальнейших действий

'''
При выборе единицы пользователь желает распознать текст с картинки, при выборе двойки пользователю предоставится 
возможность распознать текст с видео, а при выборе нуля программа завершит свою работу
'''


def choice():
    print()
    print()
    print("Choose a number from 0 to 2 and write it")
    print()
    print("'1' - if You want to recognize text from an image")
    print("'2' - if You want to recognize text from a video")
    print("'3' - Если вы хотите прочитать все изображения из папки") #Дополнительная строка
    print("'0' - if You want to exit")
    f = False
    while f != True:
        theChoice = str(input())
        if theChoice.isdigit():
            if int(theChoice) == 0 or int(theChoice) == 1 or int(theChoice) == 2 or int(theChoice) == 3:
                f = True
                if int(theChoice) == 1:
                    print("You have chosen to recognize a text from an image successfully!")
                    print()
                    return int(theChoice)
                elif int(theChoice) == 2:
                    print("You have chosen to recognize a text from a video successfully!")
                    print()
                    return int(theChoice)
                elif int(theChoice) == 3:#Дополнительные строки
                    print("Вы выбрали чтение текста со всех изображений в папке")
                    return int(theChoice)
                elif int(theChoice) == 0:
                    print("You have chosen to exit! See you later!")
                    exit(1)
            else:
                print("Choose a number from 0 to 3, please and write it again")
                print()
        else:
            print("Choose an integer")
            print()


'''
Функция проверки входных данных, а именно файла, путь к которому вводится пользователем, на существование и на 
правильное расширение файла, при ошибке ввода пользователю предоставлена возможность ввести данные еще раз
'''


def inputting(theChoice):
    print()
    f = False

    # Если файл - картинка
    if theChoice == 1:

        # Пока программа не получит подходящий файл, она будет требовать путь к файлу
        while f != True:

            inputFile = str(input("Specify the path to the image: "))
            print()

            # Проверка существования файла
            if os.path.exists(inputFile):

                # Проверка, что данный файл - картинка
                if inputFile.lower().endswith(('.png', '.jpg', '.jpeg', '.tiff', '.bmp')):
                    print("The image was read successfully")
                    f = True
                    return inputFile
                else:

                    # Файл - не картинка
                    print("The file is not an image")

            else:

                # Файла не существует
                print("The file was not found")
    # Если файл - видео
    elif theChoice == 2:

        # Пока программа не получит подходящий файл, она будет требовать путь к файлу
        while f != True:
            inputFile = str(input("Specify the path to the video: "))
            if os.path.exists(inputFile):
                if inputFile.lower().endswith(('.mp4', '.MP4', '.avi', '.wmv', '.mov', '.amv', '.m4v', '.flv', '.gif')):
                    print("The video was read successfully")
                    f = True
                    return inputFile
                else:
                    # Файл - не видео
                    print("The file is not a video")

            else:
                # Файла не существует
                print("the file was not found")


# Функция выборки файлов из папки
def folder_read(folder_path):
    file_form = (".png",".jpg",".jpeg")#Расширения изображений для выборки
    images = [] #Массив для включения файлов
    if os.path.isdir(folder_path):#Проверка, является ли файл по данному пути папкой
        for file in os.listdir(folder_path):#Цикл перебора файлов данной папке
            if file.lower().endswith(file_form):# Уменьшаем регистр и проверяем, кончается ли файл из папки на расширения из file_form
                images.append(os.path.join(folder_path,file))# Включаем в список для обработки
    return images # Возврат значения функции



### Функция преобразования видео (запись скриншотов в папку)

def readingVideo(inputFile):
    print()

    # Директория для загрузки в нее снятых кадров с видео
    theDirectory = './VideoResult/'

    # Если директория существует, то удаляем ее, чтобы программа не брала кадры с дургих тестов
    if os.path.exists(theDirectory):
        shutil.rmtree(theDirectory)

    # Создаем директорию для загрузки в нее кадров с видео
    os.makedirs(theDirectory)

    # С помощью cv2 присваиваем переменной Video исходное видео с помощью модуля VideoCapture
    Video = cv2.VideoCapture(inputFile)

    # Запускаем счетчик кадров
    index = 0

    # Пока видео открыто
    while Video.isOpened():

        '''
        Переменная bool для проверки на возможность считывания кадров (пока видео не закончилось) 
        и фрейм - скриншот кадра
        '''
        successInOpening, frame = Video.read()
        if not successInOpening:
            break
        # Скриншот делается раз в 8 секунд
        if index % 250 == 0:
            print("The frame № " + str(index) + " is written to " + theDirectory)
            # Запись скриншота в папку директории
            cv2.imwrite('./VideoResult/frame' + str(index) + '.jpg', frame)
        index += 1
        # Если видео закончилось
        if cv2.waitKey(10) & 0xFF == ord('q'):
            break
    # Закрытие видео
    Video.release()
    # Закрывает все cv2 окна
    cv2.destroyAllWindows()


### Преобразование картинки

def readingImg(inputFile):
    print()
    # С помощью cv2.imread читается картинка с помощью пути к файлу
    inputImg = cv2.imread(inputFile)

    # Корректировка размеров картинки
    inputImg = cv2.resize(inputImg, None, fx=0.71, fy=0.71)

    # Установление новой картинки, в которую мы будем помещать поля для распознавания
    placingBoxesImg = inputImg
    placingBoxesImg = cv2.resize(placingBoxesImg, None, fx=0.99, fy=0.99)

    # Преобразование картинки в серый цвет
    grayImg = cv2.cvtColor(placingBoxesImg, cv2.COLOR_BGR2GRAY)

    # Преобразование картинки в черно-белую картинку
    blackWhiteImg = cv2.adaptiveThreshold(grayImg, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 85, 11)

    # Алгоритм наложения боксов на распознанные слова
    hImg, wImg, _ = placingBoxesImg.shape
    boxes = pytesseract.image_to_data(placingBoxesImg)

    # print(boxes) - необходимо для выбора нужных данных
    for x, b in enumerate(boxes.splitlines()):
        if x != 0:
            b = b.split()
            if len(b) == 12:
                x, y, w, h = int(b[6]), int(b[7]), int(b[8]), int(b[9])
                cv2.rectangle(placingBoxesImg, (x, y), (w + x, h + y), (126, 17, 0), 1)
                cv2.putText(placingBoxesImg, b[11], (x, y), cv2.FONT_HERSHEY_COMPLEX, 0.5, (26, 17, 0), 1)

    return blackWhiteImg, inputImg, placingBoxesImg


### Получение файла, в который необходимо записать распознанный текст

def getting_output():
    print()
    f = 0

    # Возможность ввести текстовый файл еще раз при ошибке
    while f != 1:
        outputFile = str(input("Specify the path to the file for printing the recognized text: "))

        # Проверка файла на то, что он - текстовый
        if outputFile.lower().endswith('.txt'):
            f = 1
            return outputFile
        else:
            # Файл не является .txt
            print("The file is not a .txt file")
            print("Try again")


### Запись распознанного текста в указанный пользователем текстовый файл с видео

def printingVideo(outputFile):
    print()

    # Директория VideoResult для того, чтобы оттуда взять скриншоты и прочитать с них текст
    theDirectory = './VideoResult/'
    file = open(outputFile, 'w')

    # Проходим по всем скриншотам
    for i in os.listdir(theDirectory):

        # Открываем изображение
        currentImage = Image.open(theDirectory + '/' + i)

        # Преобразуем в текст текущее изображение
        text = pytesseract.image_to_string(currentImage, lang='eng')
        if text != '':
            file.write(text)
    print("The recognized text is in ")
    print(outputFile)
    file.close()


### Запись распознанного текста в указанный пользователем текстовый файл с изображения

def printingImg(blackWhiteImg, outputFile):
    print()

    # Подключаем конфиг для четкого считывания слов с картинки
    config = "--psm 3"
    text = pytesseract.image_to_string(blackWhiteImg, config=config)
    file = open(outputFile, 'w')
    if text != '':
        file.write(text)
        print("The recognized text is in: " + outputFile)

    # При отсутствии распознанного текста программа выводит соответствующее сообщение
    else:
        file.write("The program did not recognize the text")
        print("The problem is in the: " + outputFile)
    file.close()


### Вызов изображений: полученное на вход изображение, изображение, переведенное в черно-белый тон, изображение с установленными боксами считанных слов

def showingImgs(blackWhiteImg, inputImg, placingBoxesImg):
    print()
    print("Now there are 3 tabs opened, so you can see, how the program was changing the image")
    print("Wait for 21 seconds to choose the next option")
    print()

    # Функция, показывающая пользователю фотографии с интервалом в 7 секунд
    cv2.imshow('INPUT', inputImg)
    cv2.waitKey(7000)
    cv2.imshow("BLACK AND WHITE", blackWhiteImg)
    cv2.waitKey(7000)
    cv2.imshow('BOXES', placingBoxesImg)
    cv2.waitKey(7000)

    # Закрывает все окна (изображения)
    cv2.destroyAllWindows()

# Функция преобразования всех изображений в папке, изображение - текст
def convert_folder(input_folder,out_folder):
    images = folder_read(input_folder)# Выборка изображений
    if not images: # Нет изображений - выходим
        print("В папке нет изображений")
        return
    for img_path in images:# Перебираем выборку
        name = os.path.splitext(os.path.basename(img_path))[0]# Выбираем имя файла, потом певрвый индекс из ('name', 'format')
        out_file = os.path.join(out_folder, name + ".txt")# Совмещаем путь папки вывода и файл

        print(f"Создаётся файл: {out_file}")
        blackWhiteImg, inputImg, placingBoxesImg = readingImg(img_path)  # Скопируем из основной функции вывод различных вариаций изображения
        config = '--psm 3'
        text = pytesseract.image_to_string(blackWhiteImg,config=config,lang='rus')# Конвертирование в текст и обязательно РУССКИЙ

        with open(out_file,"w",encoding='UTF-8') as out:# Предупреждение ошибки записи, если ошибка - он не закроется при открытии-записи-закрытии
            out.write(text if text else "Текста нет")# Текст есть, тогда пишем, иначе "Текста нет"
    print(f'Файлы обработаны и в папке: {out_folder}')

### Главная функция

if __name__ == "__main__":
    print("Welcome to 'Text Recognition' program!")
    # Программа, помогающая считывать текст с картинки
    # Применение написанных ранее функций

    # Смотрим выбор пользователя
    theChoice = choice()
    pytesseract.pytesseract.tesseract_cmd = r"D:\Programm Files\Tesseract-OCR\tesseract.exe"
    f = 0
    while f != 1:
        if pytesseract.pytesseract.tesseract_cmd:
            print("Tesseract program has been founded on your computer")
            print("The path to tesseract file is " + pytesseract.pytesseract.tesseract_cmd)
            f = 1
        else:
            print("The tesseract program has not been found on your PC")
            print(
                "If You do not have tesseract.exe on your PC, You can read README.md to understand how to download it")
            print("Would You like to stop the program? ")
            f1 = False
            while f1 != True:
                answer = str(input("Type 'yes' or 'no': "))
                if answer == 'yes':
                    print("You have chosen " + answer + " See you later!")
                    f1 = True
                    exit(1)
                elif answer == "no":
                    pytesseract.pytesseract.tesseract_cmd = str(
                        input("Please, specify the path to it or read README.md to download it: "))
                    f1 = True
                else:
                    print("Please, write 'yes' or 'no' again")

    while theChoice == 1 or theChoice == 2:

        # Если пользователь желает считать текст с картинки
        if theChoice == 1:
            inputFile = inputting(theChoice)
            blackWhiteImg, inputImg, placingBoxesImg = readingImg(inputFile)
            outputFile = getting_output()
            printingImg(blackWhiteImg, outputFile)
            showingImgs(blackWhiteImg, inputImg, placingBoxesImg)

        # Если пользователь желает считать текст с видео
        elif theChoice == 2:
            inputFile = inputting(theChoice)
            readingVideo(inputFile)
            outputFile = getting_output()
            printingVideo(outputFile)
            print()

        # Чтение всех изображений в папке
        elif theChoice == 3:
            print("Укажите путь к папке с изображениями для обработки:")
            i_path = input()
            if not os.path.isdir(i_path):
                print("Такой папки нет")
                continue
            print("Введите путь к папке для выходных файлов:")
            o_path = input()

            convert_folder(i_path,o_path)

        # Возможность повторного ввода программы
        print("If You want to recognize something again: ")
        theChoice = choice()
