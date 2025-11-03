/*
użyć locka (safeguard) do ochrony zasobów współdzielonych
id 
vector <thread>
tablica mutexów dla korytarzy
PPM z labiryntem 

rozmiar nie ma żadnego znaczenia
tak żeby się na ekran zmieściło 

wątek potomny powinien sprawdzać czy może wejść do korytarza (czy mutex jest dostępny)

na początku najlepiej zrobić na małym labirynciku coś co tylko sprawdzi czy dobrze wątki się zachowują 

ewentualnie:
wyświetlić liczbę wszystkich wątków, 
który wątek miał najwięcej dzieci, 
który wątek odwiedził najwięcej korytarzy
*/