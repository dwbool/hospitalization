# hospitalization
hospitalization XML data parsing

			Hospitalization 1.11 on 5.11.2020

Программа запускается как файл Hospitalization.exe, после чего она появляется в Windows System Tray.
Зайти в программу можно щелкнув ее значок в трее. На данный момент идет стандартный значок C++ Builder XE8.
Зайти в настройку базы данных щелкнув меню File / Database. Задать строку подключения к базе и нажать ОК. Параметр лежит в survey_info.ini file in program's directory:
[database]
connection_string=
Программа использует базу как основную DB_SURVEYS, 
также дополнительно таблицу базы [DB_REG].[dbo].[PRK_REG], [DB_NSI].[dbo].F003_comp                                                                            , [DB_NSI].[dbo].F008  , [DB_NSI].[dbo].F002_comp     , [DB_NSI].[dbo].V005, [DB_NSI].[dbo].V020 , [DB_NSI].[dbo].T019_otd                                                                                                                                                                                                                                                                                                                                                                                                    
более подробно структура в файле: structs.xls
Зайти в меню программы File / Settings. Появится окно настроек. Там следует задать параметры
Survey / Incoming files directory  - директория для входных файлов по диспансеризации
параметр лежит в survey_info.ini
[settings]
incoming_dir=
Hospitalization / Incoming files directory - директория для входных файлов по госпитализации
Поставить галочку:
Survey ( "dispensarización" ) files process - если нужно чтобы программы обрабатывала диспансеризацию
Hospitalization files process - если нужно чтобы программы обрабатывала госпитализацию
Задать настройки:
General Settings / Processed Files Directory - директория куда будут копироваться файлы после обработки и удаления из Incoming files directory . Она идет общая для dispensarización / hospitalization.
General Settings / Outcoming Files Directory - директория для исходящих файлов. Предполагается что в ней лежат поддиректории, начинающиеся с кода ЛПУ, туда она будет копировать файл предназначенный для ЛПУ. Если она его там не находит, на данный момент сделано так: программа создаст поддиректорию: КодЛПУ_auto и туда скопирует файл.
General Settings / Make XML Reports at [time] - Параметр в настройках, задать время когда программа рассылает отчеты в форме XML в ЛПУ и страховую Т1-Т6. Поставить на нем галочку если нужно чтобы программа выполняла отчеты. Иначе снять галочку. Параметр лежит в файле survey_info.ini:
[report]
time=10:00:00
make=1

Нажать в настройках ОК.
Нажать кнопку с зеленым треугольником для запуска процесса обработки файлов программы.
Для остановки программы нажать кнопку с черным квадратиком (рис. 1). 
Нажатие на крестик в окне в левом верхнем углу, закрывает окно, но не выгружает программу из памяти, она продолжает находиться в трее и работать в фоновом режиме. 
Нажатие меню File / Exit - выгружает программу из памяти. 
При появлении файла в Incoming files directory  программа его обрабатывает и отображает имя файла в списке вверху окна. Более детальный лог приема файла идет в нижней части окна. При приеме следующего файла этот лог очищается.
После обработки копируется файл с ответом для ЛПУ в XML - в каталог General Settings / Outcoming Files Directory.  Ответ содержит описания ошибок - как общую ошибку , так и по каждой записи.

рис. 1. Окно программы


Отчеты XML
Зайти в меню File / XML Reports. В окне справа пункты Т1-Т6. На данный момент сделано Т1-Т4. Поставить курсор на пункт и посмотреть записи из базы которые будут выгружены. Нажать кнопку с 1 треугольником и посмотреть выгрузку. Внизу идет лог. В логе можно выделить имя файла и нажать Menu / open selection для открытия файла. В таблице записей из базы можно поставить курсор на имя файла и нажать то же самое для его открытия. При нажатии кнопки с 2 треугольниками и линией программа проходит и выгружает все пункты Т1-Т6. Файл уходит в директорию начинающуюся с кода ЛПУ 4 цифры.  Директории ЛПУ лежат в General Settings / Outcoming Files Directory, survey_info.ini file parameter:
[settings]
outcoming_dir=
Ошибки
Смотреть таблицу DB_SURVEYS / hosp_error_define
field - поле над которым идет проверка
operator1 - тип операции проверки
arg1 - аргумент операции - например с чем сравнимаем, какой справочник смотрим при проверке.
operator2, arg2 - то же самое если проверка двухсторонняя.
При расхождении с условием в XML падает ошибка с кодом из колонки err_code, а также описание ошибки из колонки  defined_as. В таблице HOS_T1_PAC .. HOS_T6_PAC идет пометка данной записи полем faulty=1, а также пишутся коды err_code в поле [err_codes]. Если запись без ошибок то в таблице будет faulty=0.






