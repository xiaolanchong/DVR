//! \file IAnalyzer.h
//! \brief Интерфейсы алгоритмов
//! 
//! Создан в 2002 году, А. Малистов, А. Хамухин.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IANALYZER_H__B496D355_BB7D_4DB4_BD1D_618E285EE8E3__INCLUDED_)
#define AFX_IANALYZER_H__B496D355_BB7D_4DB4_BD1D_618E285EE8E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// declare in DLL
// #define SRSDLL_API __declspec(dllexport)
//
//  
// #define SRSDLL_API __declspec(dllimport)

#ifndef SRSDLL_API
#define SRSDLL_API
#endif

//////////////////////////////////////////////////////////////////////
// Forward declare
//////////////////////////////////////////////////////////////////////

namespace Elvees {
namespace Win32 {

	class CImage;
	class CPTZControl;
}
}

class CDataWare;

//////////////////////////////////////////////////////////////////////
// Common structures
//////////////////////////////////////////////////////////////////////

//! \brief Информация об именах параметрах и их значениях
struct AlgoParamsInfo
{
	long Number;		//!< Число параметров в алгоритмах
	char **Names;		//!< Массив указателей на строки с названием параметров
	double *Values;		//!< Массив значений параметров
};

/*! \brief Структура с информацией об объекте.
*
* Возвращается либо методом CIAnalyzer::GetObjects, либо CISlaveManager::GetNextPhoto.
* Содержит информацию об объектах/объекте
*/
class ObjectReg{
public:
	ObjectReg();
	virtual ~ObjectReg();

	/*! \brief Удалить структуру ObjectReg
	*
	* Этот метод вызывается только после получения структуры из CISlaveManager::GetNextPhoto.
	* Чтобы удалить массив, полученный от CIAnalyzer::GetObjects, необходимо вызывать
	* CIAnalyzer::DeleteObjects
	*/
	virtual long _Release();

	/*! \brief номер мастера
	*
	* Актуален для ObjectReg, полученный из CISlaveManager::GetNextPhoto
	*/
	int masterID;

	/*! \brief номер объекта
	*
	* уникален для данного обработчика мастера
	*/
	int UID;
	
	//! \brief координаты появляения объекта
	int XApp,YApp;
	
	/*! \brief фотография объекта
	*
	* возвращается из CISlaveManager::GetNextPhoto. Метод ObjectReg::_Release вызывает
	* img->_Release().
	*/
	Elvees::Win32::CImage *img;

	/*! \brief индекс класса объекта
	*
	* индекс даётся в системе распознавания человек/машина/группа людей/...
	*/
	int Class;
	//! \brief индекс цвета объекта
	//! имя класса можно получить из функций CIAnalyzer::GetShortName и
	//! CIAnalyzer::GetFullName
	int Color;
	//! \brief средний цвет объекта R, G, B
	BYTE AvgCol[3];

	//! \brief средняя скорость объекта в пикселах в секунду
	double Avgspeed;
	
	//! \brief координаты исчезновения объекта
	int XDApp,YDApp;
#pragma warning(disable:4201)
	//! \brief координаты рамки объекта
	union {
		RECT rect;
		struct {int xleft,yup,xright,ydown;};
	};
#pragma warning(default:4201)
	/*! \brief Статус объекта
	*
	* status=0 - объект ещё "не существует" <br>
	* status=1 - объект в первый раз появился <br>
	* status=2 - объект на экране <br>
	* status=3 - объект временно исчез <br>
	* status=4 - объект исчез навсегда <br>
	* status=5 - фотография не прошла проверку на то, что объект в кадре (слейв промахнулся) <br>
	* status=6 - фотография прошла проверку на то, что объект в кадре <br>
	* Значения status с 0 по 4 выдает обработчик мастера, остальные - слейв.
	*/
	long status;
	/*! \brief флаг "дырки"
	*
	* Если равен true, то объект - "дырка"
	*/
	bool holeFlag;

	/*! \brief флаг "некачественного" объекта
	*
	* Если флаг равен true, то объект по разным причинам является "подозрительным" на
	* "ложность". Такие объекты нужно скрывать от пользователя и не выдывать на них
	* тревоги.
	*/
	bool suspeciousObject;

	/*! \brief флаг оставленного предмета
	*
	* Если равен true, то объект является оставленным
	*/
	bool leftObjectFlag;

	/*! \brief флаг остановившегося объекта
	*
	* Если равен true, то объект остановился
	*/
	bool stoppedObjectFlag;
	
	/*! \brief флаг объекта, выбранного для слежения
	*
	* Если равен true, то объект выбран для слежения и является приоритетным
	* при фотографировании слейвом.
	*/
	bool followFlag;
	
	/*! \brief время присутствия объекта на кадре в миллисекундах*/
	DWORD ExistingTime;

	/*! \brief время присутствия объекта на кадре в кадрах*/
    long ExistingFrame;

	/*! \brief значение ключа объекта в базе данных
	*
	* dbID используется как ключевое поле для объектов в базе данных.
	* dbID должен устанавливаться сразу при получении status=1 при помощи
	* функции CIAnalyzer::SetDatabaseID, в противном случае для объекта
	* с данным UID на текущем мастере номер masterID в поле dbID будет
	* возвращаться -1.
	*
	* Значение поля dbID для данного UID может измениться. Это сигнал
	* того, что данный объект связан с объектом на другой камере, то есть
	* установлено, что в зоне пересечения двух камер находится один и тот
	* же объект. Изменившееся значение dbID является ключевым значением
	* для связанного объекта. По факту измениния данного поля можно
	* установить связь между объектами.
	*/
	long dbID;

	/*! \brief "качество" снимка
	*
	* Условная характериска качества изображения объекта. Чем больше значение, тем выше качество.
	*/
	long photoQuality;
	//__int64 objectID;
};

//////////////////////////////////////////////////////////////////////
//! \brief Интерфейс обработчика мастер-камеры CIAnalyzer
//!
//! Интерфейс можно получить от функции GetAnalyzerInterface
//////////////////////////////////////////////////////////////////////

class CIAnalyzer  
{
public:
	/*! \brief Удаление класса.
	*
	* Счётчик ссылок не поддерживается.
	*/
	virtual long _Release()=0;
	
	//! \brief Инициализация.
	virtual BOOL Init(const char *iniFile, CDataWare *dw, long ID)=0; //!< Инициализация настроек
	//!< \param iniFile - файл настроек, \param dw - указатель на DataWare, \param ID - уникальный номер камеры
	//!< \return Возвращает FALSE при ошибке.
	
	/** @name Функции работы с параметрами
	*  Все функции возвращают 0, если OK.
	*/
	//@{	
	virtual long GetParamsInfo(AlgoParamsInfo *api)=0;
	//!< \brief Получить информацию о параметрах и их значениях.
	//!< \param api - информация о параметрах, записанная в структуре AlgoParamsInfo
	//!< \sa struct AlgoParamsInfo
	virtual long DeleteParamsInfo(AlgoParamsInfo *api)=0;
	//!< Удалить выделенную память
	virtual long SetParams(double *source)=0;
	//!< \brief Установить параметры
	//!< \param source - Массив параметров
	virtual long SetParams(AlgoParamsInfo *api)=0;
	//!< Установить параметры
	//@}
	
	
	/** @name Функции слежения
	*  Работа со слежением за объектом и позиционированием.
	*/
	//@{
	virtual void FollowObjectOff()=0; //!< \brief Отключение слежения
	virtual void FollowObject(long x, long y)=0; //!< \brief Слежение за ближайшим объектом
	virtual bool FollowByID(long ID)=0;	//!< \brief Слежение за объектом c известным ID
	//!< \return TRUE, если согласен следить.
	virtual long GoToXY(long x, long y, long slaveID=-1)=0;
	//!< \brief Переместить слейв
	//!<
	//!< Переместить слейв slaveID в координату x,y; 
	//!< Если slaveID=-1, то выбирается произвольный.
	//@}
	
	/** @name Основные функции
	*  Вызов распонавания и получение результатов.
	*  Возвращают 0, если OK.
	*/
	//@{	
	/*! \brief Обработка кадра
	*
	* Вызывается последовательно для каждого кадра видеопотока.
	* \param newFrame - кадр потока в формате CImage.
	* \return 0, если нет ошибок
	*/
	virtual int HandleBmp(Elvees::Win32::CImage *newFrame)=0; 
	/*! \brief Получение массива объектов.
	* Получает массив объектов
	* \param obj - указатель на массив объектов
	* \param n - число объектов (может быть 0)
	* \return число объектов
	*/
	virtual int GetObjects(ObjectReg **obj, long *n)=0; 
	/*! \brief Удаление массива объектов
	* \param obj - указатель на массив объектов
	* \return 0, если нет ошибок
	*/
	virtual int DeleteObjects(ObjectReg* obj)=0; 
	//@}
	
	//! \brief Сброс
	virtual void DeleteStatistic()=0;
	//! \brief получить статистический задний фон
	//! \return HBITMAP картинки с фоном
	virtual HBITMAP GetBackground()=0;
	//! \brief Уставновка hdc для вывода служебной информации
	//! Чтобы не выводить отладочные картинки, следует сделать hdc=0. По умолчанию hdc=0.
	virtual void SetHDC(HDC hdc)=0;

	/*! \brief тип отладочной картинки.
	* \param printBG - номер картинки от 1 до 3. Если -1, то картинка переключается на следующий фон. Если 0,
	* фон не выводиться.
	* \return TRUE, если фон теперь выводится, FALSE, если нет
	*/
	virtual BOOL SwitchPrintBG(long printBG=-1)=0;
	
	/** @name Функции названий классов объектов
	* возвращают короткое и длинное имя класса id для распознающей системы с номером system. 
	* (На данный момент) system=0 распознает тип объекта system=1 его цвет.
	* Освобождать полученную строку не надо. Время жизни указателя на строку совпадает с временем
	* жизни CIAnalyzer. При неправильных параметрах возвращает NULL;
	*/
	//@{
	virtual char* GetShortName(long system,long id)=0;
	virtual char* GetFullName(long system,long id)=0;
	//@}

	/*!\brief Сохранить наблюдаемый объект в файл формата adb. 
	*
	* Имя adb-файла указывается в ключе ObjecMapFileName секции [main] ini-файла.
	*/
	virtual long SaveFollowedObjectToMap()=0;

	/*! \brief Установить идентификатор базы данных.
	*
	* Устанавливает dbID для объекта под номером analyzerID при получении status=1
	* \param analyzerID - поле UID структуры ObjectReg
	* \param dbID - выделенное ключевое значение объекта
	* \return 0, если все в порядке, 1, если объекта analyzerID не существует.
	*/
	virtual long SetDatabaseID(long analyzerID,long dbID)=0;

	//! \brief Получить количество тематических страниц для вывода информации о работе алгоритмов
	virtual long GetNumOfInfoPages()=0;
	//! \brief  Получить заголовок информационной страницы с номером index
	virtual char* GetPageTitle(long index)=0;
	//! \brief Получить адрес буффера с текстом выводимой информации для тематической странице с номером index
	virtual char* GetInfoPage(long index)=0;

	//! \brief Обновить дескрипторы путем чтения параметров из реестра. 
	//! \return 0, если все параметры обновлены; >0, если есть параметры, которые не считались
    virtual long UpdateParams()=0;

	//! \brief Получить флаг состояния (если все в порядке, то флаг равен 0)
	virtual long GetStateFlags()=0;
	//! \brief Получить строку с описанием ошибки по флагу, если он не ноль.
	virtual char* GetStateString(long flags)=0;

	//! \brief  Получить запись для обучающей системы. При ошибке возвращает NULL
	virtual char* GetObjectInfo(long obj_id)=0;
	//! \brief  Удалить память, выделенную функцией GetObjectInfo.
	virtual long FreeObjectInfo(char* data)=0;

  virtual bool GetLatestMotionLabels( unsigned char *& pImage, __int32 & width, __int32 & height );
};

//////////////////////////////////////////////////////////////////////
//! \brief Интерфейс обработчика слейв-камер CISlaveManager
//!
//! Интерфейс можно получить при помощи функции GetSlavManagerInteface
//////////////////////////////////////////////////////////////////////
class CISlaveManager  
{
public:
	virtual long _Release()=0;

	virtual long Init(Elvees::Win32::CImage * is, CDataWare *dw,long clusterID, long _SlaveID, const char* iniFile=NULL)=0;
	//!< В clusterID нужно указать id кластера, к которому относится данный слейв, в iniFile возможно указать Connection String
	//!< к базе данных. Если не указать, он будет взят из реестра
	//!< Успех - 0

	virtual long GetNextPhoto(ObjectReg **obj)=0;
	//!< \brief Основной метод.Обрабатывает сообщения от процесса Commutator. 
	//!< 
	//!< Как только заканчивается очередная группа обменов сообщений, после которой 
	//!< известна фотография объекта и примерная рамка, функция возвращает управление (при этом 
	//!< фотография возвращается в структуре ObjectReg с 5 или 6 статусом - как в CIAnalyzer::GetObjects). 
	//!< Поскольку возможна ситуация, когда задания от коммутатора не поступают в течении
	//!< неопределенно долгого времени, то GetNextPhoto может находиться в CDataWire::WaitForMessage
	//!< в течении неопределенно долгого времени. Можно прервать это ожидание, вызвав
	//!< SendQuitMessage из параллельного потока или из обработки GUI-сообщения (см. примечания к 
	//!< CCommutator::MainLoop). 
	//!< \return
	//!<	<ul>
	//!<		<li>	0, если фотография получена верно,
	//!<		<li>	1, если произошел выход по SendQuitMessage, 
	//!<		<li>	2, если произошло зависание мастера, с которым общался Slave
	//!<		<li>	3, если CallPreset выдал ошибку (obj->status=код ошибки)
	//!<		<li>	4, если не может соединиться с коммутатором 
	//!<		<li>	5, если запущен еще один коммутатор, управляющий данным слейвом. Происходит разрыв соединения 
	//!<			со старым и соединение с новым.
	//!<		<li>	6, переход в ручное управление
	//!<		<li>	7, вызов позиционирование по координате
	//!<		<li>	-1, если проблемы с работой DataWire
	//!<		<li>	-2, если ошибки в работе коммутатора (не должна возникать вообще)
	//!<			отрицательные значения при ошибке - критические, 
	//!<			после положительных можно продолжать работу
	//!<	</ul>
	
	virtual long SendQuitMessage()=0;
	//!< метод аналогичен методу CCommutator::SendQuitMessage

	virtual long GetNumOfInfoPages()=0;
	//!< Получить количество тематических страниц для вывода информации о работе алгоритмов
	virtual char* GetPageTitle(long index)=0;
	//!< Получить заголовок информационной страницы с номером index
	virtual char* GetInfoPage(long index)=0;
	//!< Получить адрес буффера с текстом выводимой информации для тематической странице с номером index

	virtual long GetStateFlags()=0;
	//!< Получить флаг состояния (если все в порядке, то флаг равен 0)
	virtual char* GetStateString(long flags)=0;
	//!< Получить строку с описанием ошибки по флагу, если он не ноль.
	
};

//////////////////////////////////////////////////////////////////////
//! \brief Интерфейс CICommutator
//!
//! Интерфейс можно получить при помощи функции GetCommutatorInteface
//////////////////////////////////////////////////////////////////////
class CICommutator  
{
public:
	//! \brief Удаление интерфейса
	virtual long _Release()=0;
	
	virtual long Init(char *iniFile, CDataWare *_dw, long id)=0;//!< Инициализация
	//!<\param iniFile - путь к БД/ini-файлу (если NULL, то путь к БД берётся из реестра)
	//!<\param _dw - указатель на _dw
	//!<\param id - номер кластера в БД (для обращения к настройкам через CIDBBridge)
	//!<\return <ul> <li>0, если все в порядке
	//!<			<li>-1, если не смог прочесть корректно ini-файл
	//!<			<li>-2, елси _dw=NULL
	//!<			<li>-3, если произведена попытка вызать Init ПОВТОРНО после УСПЕШНОГО запуска
	//!<			<li>-4, если не удалось создать экземпляр CAlgoPTZManager
	//!<			<li>-5, если не удалось проинициализировать экземпляр CAlgoPTZManager
	//!<		</ul>
	virtual long SendQuitMessage()=0;
	//!< Послать сообщение на выход из MainLoop. Данный метод просто 
	//!< формирует сообщение определенного формата и посылает его при помощи CDataWire "самому 
	//!< себе".
	
	virtual void MainLoop()=0;
	//!< Данный метод обеспечивает цикл обработки сообщений от всех элементов 
	//!< кластера. Он не вернет управления, пока не будет вызван метод SendQuitMessage либо из другого 
	//!< потока, либо при обработке GUI-сообщения в том случае, если метод CDataWire::WaitForMessage 
	//!< поддерживает прием этих сообщений (основное время MainLoop будет проводить в ожидании 
	//!< сообщения в методе CDataWire::WaitForMessage).
	virtual long SlaveCameraSwitch(long slaveID, long state)=0;	
	//!< \brief переключение ручного/автоматического режима работы камеры
	//!< \param slaveID - номер слейва (-1 для всех камер)
	//!< \param state - <ul> <li>=0 - работает только функция CIAnalyzer::GotoXY
	//!<					<li>=1 - основной режим получение картинок со Slave-камеры</ul>

	virtual long GetNumOfInfoPages()=0;
	//!< Получить количество тематических страниц для вывода информации о работе алгоритмов
	//!< Может меняться по ходу работы коммутатора
	virtual char* GetPageTitle(long index)=0;
	//!< Получить заголовок информационной страницы с номером index
	virtual char* GetInfoPage(long index)=0;
	//!< Получить адрес буффера с текстом выводимой информации для тематической странице с номером index

	virtual long GetStateFlags()=0;
	//!< Получить флаг состояния (если все в порядке, то флаг равен 0)
	virtual char* GetStateString(long flags)=0;
	//!< Получить строку с описанием ошибки по флагу, если он не ноль.
	
};

//////////////////////////////////////////////////////////////////////
//! \brief интерфейс CIFilters
//!
//! все методы возвращают 0 при правильной работе, отрицательные числа при критических ошибках,
//! положительные - при некритических
//////////////////////////////////////////////////////////////////////
class CIFilters {
public:
	//! \brief Удаление интерфейса
	virtual long _Release()=0;

	virtual	long Reflection(PBITMAPINFOHEADER pbih, LPBYTE bitsIn, LPBYTE bitsOut)=0;
	//!< выделяет высокосчастотные элементы из bitsIn в bitsOut. Интереса для системы не представляет
	//!< и введен для отладки алгоритмов.
	
	//! @name стабилизация
	//@{
	virtual long SetStabParams(long windowX,long windowY,long windowSize, long maxDisp)=0;
	//!< установить квадратное окно с левым нижним углом (windowX,windowY) и стороной windowSize,
	//!< а также максимально допустимое смещение maxDisp (при увеличении смещения меняется
	//!< шаблон стабилизации)
	virtual long Stabilize(PBITMAPINFOHEADER pbih,LPBYTE bits)=0;
	//!< стабилизация потока (вызывать для каждого нового кадра)
	//!< вход и выход - в bits
	//@}

	//! @name фильтр определения дня и ночи
	//!@{
	virtual long SetDayLightThresholds(BYTE thresh, double procent, int frameInert)=0;
	//!< установить пороги thresh и procent, выдаваемые программой обучения DayNightLearn,
	//!< а также инерцию frameInert - количество кадров подряд, необходимое для переключения
	//!< состояния в противоположное.
	virtual long GetDayLightStatus(PBITMAPINFOHEADER pbih,LPBYTE bits,long * status, char* logFile=NULL)=0;
	//!< определить, день сейчас или ночь для картинки pbih, bits.
	//!< в status возвращается -1, если состояние неопределенное (не вызван метод SetDayLightThresholds),
	//!< 0 - если ночь, 1 - если день. Если правильно задано имя файла logFile, то при переключениях
	//!< состояний в нем будет отражаться дата со временем переключения.
	//!@}
	
	//! фильтр накопления
	virtual long TimeSmoothing(PBITMAPINFOHEADER pbih,LPBYTE bits,long coef)=0;
	//!< вход и выход - в bits.
	//!< coef изменяется от 0 до 100 и обозначает степень накопления
	virtual long LocalBinarization(PBITMAPINFOHEADER pbih,LPBYTE bitsIn, LPBYTE bitsOut, long full=0)=0;
	//!< \brief локальная бинаризация
	virtual long CheckFocus(PBITMAPINFOHEADER pbih,LPBYTE bits,long negative,long *result)=0;
	//!< \brief вычисление меры фокуса
	virtual long AutoLevel(BYTE *bits, PBITMAPINFOHEADER pbih, BOOL onlyLuminocity=FALSE)=0;
	//!< \brief регулировка диапозона яркости/контрастности
	virtual long SmartBlur(BYTE *bitsIn, PBITMAPINFOHEADER pbih, BYTE *bitsOut, long r, long Tr)=0;
	//!< \brief "умный" смазыватель
	
	virtual long RemoveClouds(PBYTE bits, PBITMAPINFOHEADER pbih)=0;
	//!< \brief фильтр облаков

	virtual long BoundsRoberts(PBYTE bits, PBITMAPINFOHEADER pbih)=0;
	//!< \brief выделение границ
	virtual long FocusChecker(PBYTE bits, PBITMAPINFOHEADER pbih, long flags)=0;
	//!< \brief вычисление меры фокуса

};


//////////////////////////////////////////////////////////////////////
// Function
//////////////////////////////////////////////////////////////////////

//! \brief Получить интерфейс обработчика мастера
SRSDLL_API long __cdecl GetAnalyzerInterface(CIAnalyzer **ani);
//! \brief Получить интерфейс диспетчера
SRSDLL_API long __cdecl GetCommutatorInterface(CICommutator **coi);
//! \brief Получить интерфейс обработчика слейва
SRSDLL_API long __cdecl GetSlaveManagerInterface(CISlaveManager **smi);
//! \brief Получить интерфейс 
SRSDLL_API long __cdecl GetFiltersInterface(CIFilters **filti);
//! \brief Установить в базу данных
//! \param xmlFile - xml-файл с настройками в формате UNICODE
SRSDLL_API long __cdecl SetDefaultSettings(LPCSTR xmlFile);

/*! \brief Получить массив структур ObjectReg
\param arr - указатель на результирующий массив
\param nSize - число элементов в заказанном массиве
\return 0, если нет ошибки
*/
SRSDLL_API long __cdecl GetObjectRegArray(ObjectReg ** arr, int nSize);

//! \brief удалить массив структур ObjectReg, полученные функцией GetObjectArray
SRSDLL_API long __cdecl FreeObjectArray( ObjectReg * arr);

#define GAIE_STATUS0	1
SRSDLL_API long __cdecl GetAnalyzerInterfaceExt(CIAnalyzer **ani, DWORD options=0);
//!< \brief Получить интерфейс класса CAnalyzer, с доп. опциями.
//!< \param ani - указатель на итнтерфейс
//!< \param options - если 0, то работает также, как и
//!< GetAnalyzerInterface; если поднят флаг GAIE_STATUS0, то дает интерфейс с объектами статуса 0 (для SuperVisor).


#endif // !defined(AFX_IANALYZER_H__B496D355_BB7D_4DB4_BD1D_618E285EE8E3__INCLUDED_)
