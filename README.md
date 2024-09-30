![GitHub release (with filter)](https://img.shields.io/github/v/release/lekstar/CYD-Klipper_rus)

# CYD-Klipper-rus
Это русифицированная версия проекта [CYD-Klipper](https://github.com/suchmememanyskill/CYD-Klipper) --- это беспроводной экран для 3D-принтеров на прошивке Klipper, собранный на основе недорогого сенсорного экрана ESP32-2432S028R. В этой версии добавлена опция выбора языка (английский и русский), также есть возможность добавить любой другой язык --- нужно только нативно перевести все названия кнопок на нужный язык.

### Что нужно для сборки проекта

Для сборки экрана требуется отладочная плата ESP32-2432S028R --- это модуль, объединяющий ESP32 и резистивный сенсорный экран с диагональю 2.8". Купить можно в ваших местных интернет-магазинах или на Aliexpress по запросу "ESP32-2432S028R" или "ESP32-2432". Оригинальный проект [CYD-Klipper](https://github.com/suchmememanyskill/CYD-Klipper) можно собрать и на других конфигурациях этой платы с большей диагональю или емкостным экраном, но мы тестировали сборку только на экране ESP32-2432S028R. Подробнее об этих платах можно прочитать в репозитории ["ESP32 Cheap Yellow Display"](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display#where-to-buy).

### Особенности
- Отображение статуса принтера
- Отображение информации и статистики печати 
- Запуск печати
- (Когда принтер в режиме ожидания) позиционировать печатающую голову
- (В процессе печати) задать обороты вентилятора, поток, скорость печати и параметр z-offset
- Управление температурой
- Экструзия/откат филамента
- Запустить предопределенные макросы
- Переключение устройств через Moonraker
- Обновление по OTA
- Вывод данных в консоль по последовательному порту через USB (115200 8n1, echo off, LF/LF)

### Установка

На данный момент установить прошивку на плату можно только вручную средствами VS Code. Подробнее об установке вы найдете в файле Manual_Install.md.

### Фото
(Здесь будут фото с интерфейсом экрана, но попозже)

### Благодарности (Acknowledgment)
- [CYD-Klipper](https://github.com/suchmememanyskill/CYD-Klipper)
- [xtouch](https://github.com/xperiments-in/xtouch)
- [ESP32-Cheap-Yellow-Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display)
- [OperatorB](https://github.com/OperatorB) for the ESP32-3248S035C display driver
- [esp32-smartdisplay](https://github.com/rzeldent/esp32-smartdisplay)
