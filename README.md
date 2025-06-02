# Sprawozdanie

==

## Krótki opis wykonania zadania:

### Konfiguracja
W ustawieniach repozytorium na github ustawiłem odpowiednie zmienne środowiskowe,
oraz secrety. Następnie utworzyłem plik zawierający workflow i umieściłem w nim
podstawową strukturę. Jako sposoby wywołania workflow-u, ustawiłem reakcję na
zaktualizowanie repozytorium tagiem zaczynającym się na 'v', lub poprzez manualne
wywołanie.
Ustawiłem zadaniu odpowiednie uprawnienia wymagane przez niektóre z akcji,
i przeszedłem do konfiguracji kolejnych kroków.
Najpierw repozytorium jest checkoutowane, co jest wymagane, aby runner mógł pracować
na zawartości danego repozytorium.
Następnie wywołuję utworznie tagów dla obrazu.
Potem następuje konfiguracja Quemu, Buildx, logowanie do dockerhuba oraz githuba.

### Testowanie
Buduję obraz przy wykorzystaniu danych cache z registry, zapisuję dane cache i
obraz lokalnie. W kolejnym kroku przy pomocy Trivy testuję bezpieczeństwo danego
obrazu.

### Publikowanie obrazu
Następnie wywoływana jest akcja budująca obraz od nowa, korzystając z lokalnych
danych cache, natomiast tym razem zapisując je do registry. Obraz natomiast jest
zapisywany w ghcr.io.
Na koniec wykonywany jest manifest provenance na podstawie budowy poprzednich
obrazów.

==

## Przyjęty sposób tagowania
Automat GH Actions otagowuje obraz na trzy sposoby:
- Każdy nowy obraz zostaje otagowany jako latest (flavour: latest=true)
- Jeśli występuje w obecnych źródłach tag zaczynający się na "v", to tag obrazu
będzie numerem konkretnej wersji (pattern={{version}})
- Jeśli nie występuje tag, to tagiem jest krótki zapis sha1-ID commita,
od którego obraz powstaje

Otagowanie numerem wersji posiada większy priorytet niż otagowanie sha1-ID, co
zapewnia użecie wersji jako taga zawsze kiedy tylko występuje.

Dokładne informacje na temat działa priorytetów i konkretnych parametrów znajdują
się w README twórcy akcji - [docker/metadata-action](https://github.com/docker/metadata-action).

Powodem takiego skonfigurowania tagowania obrazów jest to, iż tag latest jest
przydatnym tagiem, którego obecność przyspiesza wiele procesów.

Otagowywanie poprzez numer wersji jest najbardziej konwencjonalnym sposobem
oznaczania obrazów, dlatego znalazł się również.

Otagowywanie poprzez sha1-id commita jest zabezpieczeniem, któro zapewnia istnienie
niepowtarzalnego tagu, gdyż same id commita w danym repozytorium, również jest
niepowtarzalne. Ponadto, posiadanie id commita jako tagu, ułatwi pobieranie obrazu,
gdyż wystarczy poznać id commita. Ponadto oznaczenie obrazu przy pomocy ID commita,
pozwoli na łatwe poznanie jakie zmiany zostały wprowadzone w danym obrazie, lub
który obraz jest obrazem, który chcemy pobrać bazując na rzeczywistej zawartości
obrazu.
