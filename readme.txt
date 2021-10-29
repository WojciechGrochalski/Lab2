Laboratorium 2
==============

Literatura
----------
  + Rochkind,"Advanced Unix Programming" rozdz. 6.2 i 6.3  
  + W. R. Stevens 
    "Programowanie zastowan sieciowych w systemie UNIX", rozdz. 2.3 i 3.4
  + M. J. Bach "Budowa systemu operacyjnego UNIX", rozdz. 7.1


Pliki, z kt�rych b�dziemy korzysta�
-----------------------------------
   Makefile
	Plik Makefile.

   err.h
        Plik nag��wkowy biblioteki obs�uguj�cej b��dy.

   err.c
        Biblioteka obs�uguj�ca b��dy.

   parent_pipe.c
        Tworzy proces, kt�ry ma wykona� program "child_pipe".  Proces 
        macierzysty komunikuje si� z nim za pomoc� ��cza, do kt�rego pisze
        kr�tki komunikat.

   child_pipe.c
        Czyta komunikat z deskryptora, kt�rego numer jest 
        przekazywany jako jedyny argument wywo�ania programu.

   parent_dup.c
        Wywo�anie: parent_dup <command> [<arg_1> ... <arg_n>]
        Tworzy nowy proces, kt�ry ma wykona� polecenie z argumentami 
        <arg_1> ... <arg_n>.  Proces macierzysty zapisuje do 
        utworzonego ��cza kr�tki komunikat. Proces potomny 
        odbiera ten komunikat z ��cza przekierowuj�c standardowe 
        wej�cie.

        Przyk�ady wywo�a�:
                parent_dup cat
                parent_dup grep from
                parent_dup grep ludziska
                parent_dup wc
                parent_dup mail `whoami`

Scenariusz zaj��
----------------

1. Deskryptory
   -----------
   Ka�dy proces utrzymuje tablic� deskryptor�w. Tablica 
   deskryptor�w jest lokalna dla procesu i zawiera informacje o otwartych
   przez proces plikach. Gdy proces otwiera plik, informacje o tym pliku 
   (m.in.: tryb otwarcia, po�o�enie na dysku, wska�nik pliku, czyli 
   pozycja w pliku, kt�rej b�dzie dotyczy� kolejna operacja wej�cia
   wyj�cia, itp.) s� zapisywane w 
   tablicy otwartych plik�w utrzymywanej przez system operacyjny. W 
   ca�ym systemie jest jedna taka tablica. Z kolei informacja o po�o�eniu
   danych o pliku w systemowej tablicy plik�w otwartych (oraz pewne 
   dodatkowe informacje) jest zapami�tywana 
   w tablicy deskryptor�w tego procesu, kt�ry plik otworzy�. 
   Indeks w tej tablicy to w�a�nie deskryptor pliku. 

 
   Do otwierania pliku s�u�y funkcja systemowa open. W wyniku
   przekazuje ona deskryptor otwartego pliku lub informacje o b��dzie. 

   * Przeczytaj man open

   Zwr��my uwag�, �e je�li proces dwukrotnie otworzy ten sam plik, to 
   w tablicy otwartych plik�w b�d� DWIE r�ne pozycje dotycz�ce tego pliku.
   Proces b�dzie mia� r�wnie� DWA r�ne deskryptory odnosz�ce si�
   do tego samego pliku. Poniewa� wska�nik pliku jest pami�tany w systemowej
   tablicy otwartych plik�w, wi�c operacje wej�cia/wyj�cia wykonywane 
   za pomoc� r�nych deskryptor�w b�d� od siebie niezale�ne (wykonanie
   operacji odczytu za po�rednictwem jednego deskryptora nie ma wp�ywu
   na pozycj�, z kt�rej odczytamy kolejne dane z pliku za po�rednictwem
   drugiego deskryptora).

   Do czytania z pliku s�u�y funkcja systemowa read, a do zapisu funkcja
   systemowa write. Zwr�� uwag�, �e funkcje te s� niepodzielne. 
   Oznacza to, �e operacje read i/lub write wykonywane na tym samym pliku 
   "jednocze�nie" nie b�d� si� przeplata�: druga rozpocznie si� po 
   zako�czeniu pierwszej. 

   * Przeczytaj man read i man 2 write 

   Gdy proces wykonuje funkcje systemowa fork() tablica deskryptor�w
   jest kopiowana do potomka. Zatem potomek "dziedziczy" po procesie
   macierzystym wszystkie otwarte pliki. Co wi�cej deskryptory o tych 
   samych numerach w procesie potomnym i macierzystym odnosz� si� do 
   tej samej pozycji w tablicy otwartych plik�w. Wniosek: 
   ojciec i syn WSPӣDZIEL� otwarte pliki: ka�da operacja wej�cia/wyj�cia
   w procesie ojca przesuwa wska�nik pliku i powoduje, �e kolejna
   operacja wej�cia/wyj�cia w procesie potomnym b�dzie dotyczy� 
   KOLEJNEJ porcji danych.
   Funkcja systemowa exec() nie wp�ywa na posta� tablicy deskryptor�w.
   Proces po wykonaniu exec() zachowuje otwarte pliki (cho� zapewne
   nie zna ju� ich numer�w, bo zmienne, kt�re przechowywa�y 
   deskryptory otwartych plik�w przesta�y istnie� w chwili wykonania
   funkcji exec() ).
   
   Wsp�dzielenie deskryptor�w jest tak�e mo�liwe w pojedynczym procesie.
   Aby stworzy� kopie deskryptora pliku u�ywamy funkcji:
       int dup(int oldfd);
       int dup2(int oldfd, int newfd);
   Zwr�� uwag�, �e dup tworzy kopi� deskryptora, kt�ra odnosi si� 
   do TEGO SAMEGO miejsca w tablicy otwartych plik�w, co orygina�. 
   Jest to zatem zupe�nie inna sytuacja ni� przy wielokrotnym 
   otwarciu tego samego pliku.
   Uwaga! Wynikiem funkcji dup i dup2 jest numer nowego deskryptora,
   a w wypadku b��du warto�� -1.
 * Przeczytaj man dup

   Przy starcie ka�dego procesu rezerwowane s� trzy specjalne deskryptory
   o numerach 0, 1 i 2. S� to deskryptory standardowego
   wej�cia, wyj�cia i standardowego wyj�cia b��d�w procesu.
   Wej�cie, wyj�cie i wyj�cie b��d�w mo�na przekierowywa� z poziomu
   interpretatora polece� (shella) za pomoc� operator�w '<' '>' '>>' 
   '2>' oraz  '&>'. 
   Na przyk�ad: cat plik > kopia.pliku  "wy�wietli" plik nie na ekran 
   lecz do pliku 'kopia.pliku'.

2. PIPE - czyli ��cza nienazwane 
   -----------------------------
   ��cza nienazwane to rodzaj plik�w istniej�cych tylko wewn�trz j�dra
   systemu operacyjnego - nie mo�na ich znale�� na dysku twardym. S�u��
   one do komunikacji mi�dzy procesami pokrewnymi. Dwa procesy nazwiemy
   pokrewnymi je�li maja wsp�lnego przodka (ojca, dziadka, itd.) lub 
   jeden jest przodkiem drugiego. Do ��cza mo�na zapisywa� dane i 
   odczytywa� je za pomoc� tych samych funkcji systemowych co 
   pliki (read oraz write). ��cza s� jednokierunkowe, zatem aby
   uzyska� komunikacje w dwie strony nale�y utworzy� par� ��cz
   nienazwanych. 
   Do tworzenia ��czy nienazwanych s�u�y funkcja systemowa
	int pipe(int filedes[2]);

 * Przeczytaj man pipe

  Funkcja ta tworzy nowe ��cze nienazwane oraz umieszcza w tablicy podanej
  jako parametr, numery dw�ch deskryptor�w. Pierwszy deskryptor s�u�y do
  czytania, a drugi do pisania do utworzonego ��cza.


3. Jak pisa� do ��cza ?
   --------------------
   Funkcja write (fd, buf, count) zapisuje do otwartego ��cza o 
   deskryptorze fd nie wi�cej ni� count bajt�w znajduj�cych si� w tablicy buf.
   ��cza maj� ograniczona pojemno�� (nie mniejsz� ni� 4Kb).
   Proces, kt�ry pr�buje zapisa� do ��cza, w kt�rym nie ma miejsca 
   na ca�� zapisywan� porcj� jest wstrzymany do
   czasu, a� z ��cza zostanie odczytana taka ilo�� danych by znalaz�o
   si� miejsce na wszystkie zapisywane dane. Zwr��my uwag�: write
   zapisze wszystko albo nic. Jedyny wyj�tek od tej regu�y jest wtedy,
   gdy pr�bujemy na raz zapisa� do ��cza wi�cej ni� jego pojemno��. 
   Wtedy i proces zapisuje do ��cza ile mo�e i jest wstrzymywany do 
   momentu a� znowu b�dzie m�g� co� do ��cza wpisa�.

   Wynikiem funkcji write jest liczba zapisanych bajt�w lub 
   -1, je�li nast�pi� b��d. 
   UWAGA
   -----
   Zapis do ��cza jest mo�liwy tylko wtedy, gdy jest ono otwarte 
   (przez ten sam lub inny proces) do czytania.
   Je�li proces spr�buje pisa� do ��cza, kt�re nie jest przez �aden
   proces otwarte do czytania, zostanie przerwany sygna�em SIGPIPE 
   (wi�cej o sygna�ach wkr�tce). Ten b��d najcz�ciej ujawnia si�
   komunikatem 'broken pipe' z poziomu interpretera polece�.


4. Jak czyta� z ��cza
   -----------------
   Do odczytu z ��cza u�ywamy funkcji systemowej
       ssize_t read(int fd, void *buf, size_t count);

 * Przeczytaj man read

   Tak jak w przypadku zwyk�ych plik�w funkcja odczytuje z ��cza 
   o deskryptorze fd nie wi�cej ni� count bajt�w do bufora znajduj�cego si� 
   pod adresem buf. Je�li w ��czu znajduje si� mniej bajt�w ni� count (ale
   ��cze nie jest puste), to funkcja read odczytuje to, co jest w ��czu i
   ko�czy si� pomy�lnie. 
   Odczyt z pustego ��cza wstrzymuje proces odczytuj�cy do czasu pojawienia
   si� w ��czu jakichkolwiek danych.

   Kolejno�� odczytu jest zgodna z kolejno�ci� zapisu (��cza s� kolejkami
   prostymi).

   Wynikiem funkcji read jest liczba faktycznie przeczytanych bajt�w. 
   Zwr�� uwag�, �e zgodnie z powy�szym opisem, nie powinno to nigdy by�
   zero. 

   Jednak zero mo�e tak�e by� wynikiem funkcji read.  Dzieje si� tak 
   przy pr�bie odczytu z ��cza, kt�re nie jest przez �aden proces otwarte 
   do zapisu.

   Typowy scenariusz u�ycia ��cza jest zatem nast�puj�cy. 
   Proces tworzy ��cze (funkcja pipe), nast�pnie rozmna�a si� 
   (funkcja fork). Proces macierzysty zamyka deskryptor do zapisu
   a proces potomny deskryptor do odczytu (odziedziczony po ojcu).  
   Teraz proces macierzysty mo�e wykona� funkcj� read. B�dzie ona 
   czeka� a� do chwili, gdy proces potomny co� zapisze w ��czu. Wtedy
   przeka�e w wyniku warto�� niezerow� i odczyta z ��cza wiadomo��. 
   Gdy proces potomny zamknie deskryptor do zapisu, read w procesie 
   macierzystym przeka�e warto�� zero. W ten spos�b proces mo�e 
   wykry� koniec strumienia danych i na przyk�ad zako�czy� si�. 
   Oczywi�cie do komunikacji w dwie strony potrzebne s� dwa ��cza.
 
* Przeanalizuj plik �r�d�owy parent_pipe.c oraz child_pipe.c.
  Zwr�� uwag� na to, �e proces potomny po wykonaniu funkcji exec()
  ma otwarty deskryptor do ��cza, ale nie zna jego numeru (bo 
  zmienna przechowuj�ca ten numer przesta�a istnie� w chwili
  wykonania funkcji exec). St�d konieczno�� przekazania tego numeru 
  przez argumenty wywo�ania programu. Nie jest to elegancka technika,
  �adniejszy spos�b znajduje si� w punkcie 6. 

5. Zamykanie ��cza 
   ---------------
   Aby zamkn�� ��cze (lub plik) u�ywamy funkcji systemowej 
       int close(int fd);

 * Przeczytaj man close

   Uwaga: Nigdy nie zamykamy sami deskryptor�w 0, 1, 2, chyba 
   �e jest do tego istotny pow�d. Zawsze zamykamy to, co sami
   otworzyli�my. Robimy to NATYCHMIAST, gdy deskryptor jest juz
   niepotrzebny. W ten spos�b zapobiegamy zb�dnemu kopiowaniu 
   deskryptor�w (przy fork()) i nadmiernemu wzrostowi tablic
   deskryptor�w (we wczesnych systemach uniksowych mozna by�o 
   mie� jednocze�nie otwartych tylko 20 deskryptor�w).

6. Podmiana standardowego wej�cia/wyj�cia
   --------------------------------------
   Ciekawym sposobem wykorzystania ��czy jest podmiana standardowego
   wej�cia i wyj�cia procesu. Jest to BARDZO WAZNA technika, gdy� 
   tak w�a�nie dzia�a wi�kszo�� program�w uniksowych. Scenariusz jest taki:
   
   Proces tworzy ��cze, nast�pnie wykonuje fork(). 
   Proces macierzysty zamyka niepotrzebne mu deskryptory, po czym 
   wykonuje si� dalej uzywaj�c otwartego ��cza. 
   Proces potomny zamyka duplikuje (funkcja dup lub dup2) deskryptory 
   ��cza na standardowe wej�cie lub wyj�cie w zale�no�ci od potrzeb, 
   a nast�pnie zamyka niepotrzebne deskryptory. 
   Teraz wykonuje exec(). Zwr��my uwag�, �e na skutek zduplikowania
   standardowe wej�cie/wyj�cie zosta�o teraz przekierowane do utworzonego
   ��cza. Zatem tekst programu wywo�ywanego w funkcji exec piszemy 
   "standardowo": czytanie ze standardowego wej�cia, zapis na standardowe
   wyj�cie. Przekierowane dokonane PRZED wywo�aniem funkcji exec 
   powoduje, �e te standardowe deskryptory dotyczy� b�d� ju� nie terminala
   lecz uprzednio utworzonego ��cza.  
 
  * Przeanalizuj plik �r�d�owy parent_dup.c

   Klasycznym przyk�adem u�ycia komunikacji jednostronnej przy u�yciu
   ��czy z podmiana wej�cia i wyj�cia jest operator | w shell-u
   
   np. cat /etc/passwd | grep Michal | more


ZADANIE
---------

   Napisz program tworz�cy dwa procesy: pierwszy z nich w p�tli oczekuje na 
   tekst podany z klawiatury, przesy�a go za pomoc� ��cza do drugiego 
   procesu, oczekuje na potwierdzenie od drugiego procesu (r�wnie�
   za pomoc� ��cza). Proces powinien ko�czy� si� po wczytaniu 
   pustego tekstu.  Drugi proces cyklicznie oczekuje na komunikat,
   i potwierdza jego odbi�r. Wykorzystaj w programie funkcje exec() oraz 
   przekierowywanie standardowych deskryptor�w (drugi proces powinien
   odczytywa� deskryptor 0 i zapisywa� na deskryptorze 1).
 
