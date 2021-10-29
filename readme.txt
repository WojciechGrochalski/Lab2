Laboratorium 2
==============

Literatura
----------
  + Rochkind,"Advanced Unix Programming" rozdz. 6.2 i 6.3  
  + W. R. Stevens 
    "Programowanie zastowan sieciowych w systemie UNIX", rozdz. 2.3 i 3.4
  + M. J. Bach "Budowa systemu operacyjnego UNIX", rozdz. 7.1


Pliki, z których bêdziemy korzystaæ
-----------------------------------
   Makefile
	Plik Makefile.

   err.h
        Plik nag³ówkowy biblioteki obs³uguj±cej b³êdy.

   err.c
        Biblioteka obs³uguj±ca b³êdy.

   parent_pipe.c
        Tworzy proces, który ma wykonaæ program "child_pipe".  Proces 
        macierzysty komunikuje siê z nim za pomoc± ³±cza, do którego pisze
        krótki komunikat.

   child_pipe.c
        Czyta komunikat z deskryptora, którego numer jest 
        przekazywany jako jedyny argument wywo³ania programu.

   parent_dup.c
        Wywo³anie: parent_dup <command> [<arg_1> ... <arg_n>]
        Tworzy nowy proces, który ma wykonaæ polecenie z argumentami 
        <arg_1> ... <arg_n>.  Proces macierzysty zapisuje do 
        utworzonego ³±cza krótki komunikat. Proces potomny 
        odbiera ten komunikat z ³±cza przekierowuj±c standardowe 
        wej¶cie.

        Przyk³ady wywo³añ:
                parent_dup cat
                parent_dup grep from
                parent_dup grep ludziska
                parent_dup wc
                parent_dup mail `whoami`

Scenariusz zajêæ
----------------

1. Deskryptory
   -----------
   Ka¿dy proces utrzymuje tablicê deskryptorów. Tablica 
   deskryptorów jest lokalna dla procesu i zawiera informacje o otwartych
   przez proces plikach. Gdy proces otwiera plik, informacje o tym pliku 
   (m.in.: tryb otwarcia, po³o¿enie na dysku, wska¼nik pliku, czyli 
   pozycja w pliku, której bêdzie dotyczyæ kolejna operacja wej¶cia
   wyj¶cia, itp.) s± zapisywane w 
   tablicy otwartych plików utrzymywanej przez system operacyjny. W 
   ca³ym systemie jest jedna taka tablica. Z kolei informacja o po³o¿eniu
   danych o pliku w systemowej tablicy plików otwartych (oraz pewne 
   dodatkowe informacje) jest zapamiêtywana 
   w tablicy deskryptorów tego procesu, który plik otworzy³. 
   Indeks w tej tablicy to w³a¶nie deskryptor pliku. 

 
   Do otwierania pliku s³u¿y funkcja systemowa open. W wyniku
   przekazuje ona deskryptor otwartego pliku lub informacje o b³êdzie. 

   * Przeczytaj man open

   Zwróæmy uwagê, ¿e je¶li proces dwukrotnie otworzy ten sam plik, to 
   w tablicy otwartych plików bêd± DWIE ró¿ne pozycje dotycz±ce tego pliku.
   Proces bêdzie mia³ równie¿ DWA ró¿ne deskryptory odnosz±ce siê
   do tego samego pliku. Poniewa¿ wska¼nik pliku jest pamiêtany w systemowej
   tablicy otwartych plików, wiêc operacje wej¶cia/wyj¶cia wykonywane 
   za pomoc± ró¿nych deskryptorów bêd± od siebie niezale¿ne (wykonanie
   operacji odczytu za po¶rednictwem jednego deskryptora nie ma wp³ywu
   na pozycjê, z której odczytamy kolejne dane z pliku za po¶rednictwem
   drugiego deskryptora).

   Do czytania z pliku s³u¿y funkcja systemowa read, a do zapisu funkcja
   systemowa write. Zwróæ uwagê, ¿e funkcje te s± niepodzielne. 
   Oznacza to, ¿e operacje read i/lub write wykonywane na tym samym pliku 
   "jednocze¶nie" nie bêd± siê przeplataæ: druga rozpocznie siê po 
   zakoñczeniu pierwszej. 

   * Przeczytaj man read i man 2 write 

   Gdy proces wykonuje funkcje systemowa fork() tablica deskryptorów
   jest kopiowana do potomka. Zatem potomek "dziedziczy" po procesie
   macierzystym wszystkie otwarte pliki. Co wiêcej deskryptory o tych 
   samych numerach w procesie potomnym i macierzystym odnosz± siê do 
   tej samej pozycji w tablicy otwartych plików. Wniosek: 
   ojciec i syn WSPÓ£DZIEL¡ otwarte pliki: ka¿da operacja wej¶cia/wyj¶cia
   w procesie ojca przesuwa wska¼nik pliku i powoduje, ¿e kolejna
   operacja wej¶cia/wyj¶cia w procesie potomnym bêdzie dotyczyæ 
   KOLEJNEJ porcji danych.
   Funkcja systemowa exec() nie wp³ywa na postaæ tablicy deskryptorów.
   Proces po wykonaniu exec() zachowuje otwarte pliki (choæ zapewne
   nie zna ju¿ ich numerów, bo zmienne, które przechowywa³y 
   deskryptory otwartych plików przesta³y istnieæ w chwili wykonania
   funkcji exec() ).
   
   Wspó³dzielenie deskryptorów jest tak¿e mo¿liwe w pojedynczym procesie.
   Aby stworzyæ kopie deskryptora pliku u¿ywamy funkcji:
       int dup(int oldfd);
       int dup2(int oldfd, int newfd);
   Zwróæ uwagê, ¿e dup tworzy kopiê deskryptora, która odnosi siê 
   do TEGO SAMEGO miejsca w tablicy otwartych plików, co orygina³. 
   Jest to zatem zupe³nie inna sytuacja ni¿ przy wielokrotnym 
   otwarciu tego samego pliku.
   Uwaga! Wynikiem funkcji dup i dup2 jest numer nowego deskryptora,
   a w wypadku b³êdu warto¶æ -1.
 * Przeczytaj man dup

   Przy starcie ka¿dego procesu rezerwowane s± trzy specjalne deskryptory
   o numerach 0, 1 i 2. S± to deskryptory standardowego
   wej¶cia, wyj¶cia i standardowego wyj¶cia b³êdów procesu.
   Wej¶cie, wyj¶cie i wyj¶cie b³êdów mo¿na przekierowywaæ z poziomu
   interpretatora poleceñ (shella) za pomoc± operatorów '<' '>' '>>' 
   '2>' oraz  '&>'. 
   Na przyk³ad: cat plik > kopia.pliku  "wy¶wietli" plik nie na ekran 
   lecz do pliku 'kopia.pliku'.

2. PIPE - czyli ³±cza nienazwane 
   -----------------------------
   £±cza nienazwane to rodzaj plików istniej±cych tylko wewn±trz j±dra
   systemu operacyjnego - nie mo¿na ich znale¼æ na dysku twardym. S³u¿±
   one do komunikacji miêdzy procesami pokrewnymi. Dwa procesy nazwiemy
   pokrewnymi je¶li maja wspólnego przodka (ojca, dziadka, itd.) lub 
   jeden jest przodkiem drugiego. Do ³±cza mo¿na zapisywaæ dane i 
   odczytywaæ je za pomoc± tych samych funkcji systemowych co 
   pliki (read oraz write). £±cza s± jednokierunkowe, zatem aby
   uzyskaæ komunikacje w dwie strony nale¿y utworzyæ parê ³±cz
   nienazwanych. 
   Do tworzenia ³±czy nienazwanych s³u¿y funkcja systemowa
	int pipe(int filedes[2]);

 * Przeczytaj man pipe

  Funkcja ta tworzy nowe ³±cze nienazwane oraz umieszcza w tablicy podanej
  jako parametr, numery dwóch deskryptorów. Pierwszy deskryptor s³u¿y do
  czytania, a drugi do pisania do utworzonego ³±cza.


3. Jak pisaæ do ³±cza ?
   --------------------
   Funkcja write (fd, buf, count) zapisuje do otwartego ³±cza o 
   deskryptorze fd nie wiêcej ni¿ count bajtów znajduj±cych siê w tablicy buf.
   £±cza maj± ograniczona pojemno¶æ (nie mniejsz± ni¿ 4Kb).
   Proces, który próbuje zapisaæ do ³±cza, w którym nie ma miejsca 
   na ca³± zapisywan± porcjê jest wstrzymany do
   czasu, a¿ z ³±cza zostanie odczytana taka ilo¶æ danych by znalaz³o
   siê miejsce na wszystkie zapisywane dane. Zwróæmy uwagê: write
   zapisze wszystko albo nic. Jedyny wyj±tek od tej regu³y jest wtedy,
   gdy próbujemy na raz zapisaæ do ³±cza wiêcej ni¿ jego pojemno¶æ. 
   Wtedy i proces zapisuje do ³±cza ile mo¿e i jest wstrzymywany do 
   momentu a¿ znowu bêdzie móg³ co¶ do ³±cza wpisaæ.

   Wynikiem funkcji write jest liczba zapisanych bajtów lub 
   -1, je¶li nast±pi³ b³±d. 
   UWAGA
   -----
   Zapis do ³±cza jest mo¿liwy tylko wtedy, gdy jest ono otwarte 
   (przez ten sam lub inny proces) do czytania.
   Je¶li proces spróbuje pisaæ do ³±cza, które nie jest przez ¿aden
   proces otwarte do czytania, zostanie przerwany sygna³em SIGPIPE 
   (wiêcej o sygna³ach wkrótce). Ten b³±d najczê¶ciej ujawnia siê
   komunikatem 'broken pipe' z poziomu interpretera poleceñ.


4. Jak czytaæ z ³±cza
   -----------------
   Do odczytu z ³±cza u¿ywamy funkcji systemowej
       ssize_t read(int fd, void *buf, size_t count);

 * Przeczytaj man read

   Tak jak w przypadku zwyk³ych plików funkcja odczytuje z ³±cza 
   o deskryptorze fd nie wiêcej ni¿ count bajtów do bufora znajduj±cego siê 
   pod adresem buf. Je¶li w ³±czu znajduje siê mniej bajtów ni¿ count (ale
   ³±cze nie jest puste), to funkcja read odczytuje to, co jest w ³±czu i
   koñczy siê pomy¶lnie. 
   Odczyt z pustego ³±cza wstrzymuje proces odczytuj±cy do czasu pojawienia
   siê w ³±czu jakichkolwiek danych.

   Kolejno¶æ odczytu jest zgodna z kolejno¶ci± zapisu (³±cza s± kolejkami
   prostymi).

   Wynikiem funkcji read jest liczba faktycznie przeczytanych bajtów. 
   Zwróæ uwagê, ¿e zgodnie z powy¿szym opisem, nie powinno to nigdy byæ
   zero. 

   Jednak zero mo¿e tak¿e byæ wynikiem funkcji read.  Dzieje siê tak 
   przy próbie odczytu z ³±cza, które nie jest przez ¿aden proces otwarte 
   do zapisu.

   Typowy scenariusz u¿ycia ³±cza jest zatem nastêpuj±cy. 
   Proces tworzy ³±cze (funkcja pipe), nastêpnie rozmna¿a siê 
   (funkcja fork). Proces macierzysty zamyka deskryptor do zapisu
   a proces potomny deskryptor do odczytu (odziedziczony po ojcu).  
   Teraz proces macierzysty mo¿e wykonaæ funkcjê read. Bêdzie ona 
   czekaæ a¿ do chwili, gdy proces potomny co¶ zapisze w ³±czu. Wtedy
   przeka¿e w wyniku warto¶æ niezerow± i odczyta z ³±cza wiadomo¶æ. 
   Gdy proces potomny zamknie deskryptor do zapisu, read w procesie 
   macierzystym przeka¿e warto¶æ zero. W ten sposób proces mo¿e 
   wykryæ koniec strumienia danych i na przyk³ad zakoñczyæ siê. 
   Oczywi¶cie do komunikacji w dwie strony potrzebne s± dwa ³±cza.
 
* Przeanalizuj plik ¼ród³owy parent_pipe.c oraz child_pipe.c.
  Zwróæ uwagê na to, ¿e proces potomny po wykonaniu funkcji exec()
  ma otwarty deskryptor do ³±cza, ale nie zna jego numeru (bo 
  zmienna przechowuj±ca ten numer przesta³a istnieæ w chwili
  wykonania funkcji exec). St±d konieczno¶æ przekazania tego numeru 
  przez argumenty wywo³ania programu. Nie jest to elegancka technika,
  ³adniejszy sposób znajduje siê w punkcie 6. 

5. Zamykanie ³±cza 
   ---------------
   Aby zamkn±æ ³±cze (lub plik) u¿ywamy funkcji systemowej 
       int close(int fd);

 * Przeczytaj man close

   Uwaga: Nigdy nie zamykamy sami deskryptorów 0, 1, 2, chyba 
   ¿e jest do tego istotny powód. Zawsze zamykamy to, co sami
   otworzyli¶my. Robimy to NATYCHMIAST, gdy deskryptor jest juz
   niepotrzebny. W ten sposób zapobiegamy zbêdnemu kopiowaniu 
   deskryptorów (przy fork()) i nadmiernemu wzrostowi tablic
   deskryptorów (we wczesnych systemach uniksowych mozna by³o 
   mieæ jednocze¶nie otwartych tylko 20 deskryptorów).

6. Podmiana standardowego wej¶cia/wyj¶cia
   --------------------------------------
   Ciekawym sposobem wykorzystania ³±czy jest podmiana standardowego
   wej¶cia i wyj¶cia procesu. Jest to BARDZO WAZNA technika, gdy¿ 
   tak w³a¶nie dzia³a wiêkszo¶æ programów uniksowych. Scenariusz jest taki:
   
   Proces tworzy ³±cze, nastêpnie wykonuje fork(). 
   Proces macierzysty zamyka niepotrzebne mu deskryptory, po czym 
   wykonuje siê dalej uzywaj±c otwartego ³±cza. 
   Proces potomny zamyka duplikuje (funkcja dup lub dup2) deskryptory 
   ³±cza na standardowe wej¶cie lub wyj¶cie w zale¿no¶ci od potrzeb, 
   a nastêpnie zamyka niepotrzebne deskryptory. 
   Teraz wykonuje exec(). Zwróæmy uwagê, ¿e na skutek zduplikowania
   standardowe wej¶cie/wyj¶cie zosta³o teraz przekierowane do utworzonego
   ³±cza. Zatem tekst programu wywo³ywanego w funkcji exec piszemy 
   "standardowo": czytanie ze standardowego wej¶cia, zapis na standardowe
   wyj¶cie. Przekierowane dokonane PRZED wywo³aniem funkcji exec 
   powoduje, ¿e te standardowe deskryptory dotyczyæ bêd± ju¿ nie terminala
   lecz uprzednio utworzonego ³±cza.  
 
  * Przeanalizuj plik ¼ród³owy parent_dup.c

   Klasycznym przyk³adem u¿ycia komunikacji jednostronnej przy u¿yciu
   ³±czy z podmiana wej¶cia i wyj¶cia jest operator | w shell-u
   
   np. cat /etc/passwd | grep Michal | more


ZADANIE
---------

   Napisz program tworz±cy dwa procesy: pierwszy z nich w pêtli oczekuje na 
   tekst podany z klawiatury, przesy³a go za pomoc± ³±cza do drugiego 
   procesu, oczekuje na potwierdzenie od drugiego procesu (równie¿
   za pomoc± ³±cza). Proces powinien koñczyæ siê po wczytaniu 
   pustego tekstu.  Drugi proces cyklicznie oczekuje na komunikat,
   i potwierdza jego odbiór. Wykorzystaj w programie funkcje exec() oraz 
   przekierowywanie standardowych deskryptorów (drugi proces powinien
   odczytywaæ deskryptor 0 i zapisywaæ na deskryptorze 1).
 
