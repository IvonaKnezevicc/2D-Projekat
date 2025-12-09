# Bioskop - Upravljanje Sedištima

2D grafička aplikacija za upravljanje sedištima u bioskopu napravljena u C++ sa OpenGL.

## Zahtevi

- OpenGL 3.3+ (programabilni pipeline)
- GLFW 3.4.0
- GLEW 2.2.0
- Visual Studio 2019+ (ili kompatibilan kompajler)

## Funkcionalnosti

### Osnovne funkcionalnosti:
- ✅ Ptičja perspektiva bioskopske sale sa 50+ sedišta
- ✅ Ulaz u salu na gornjem levom delu
- ✅ Bioskopsko platno (beli pravougaonik) na gornjem delu
- ✅ Tamnosivi pravougaonik sa providnošću 0.5 preko celog ekrana

### Upravljanje sedištima:
- ✅ Sva sedišta su plava (Slobodno) na početku
- ✅ Levi klik miša: Rezervacija/otkazivanje sedišta (žuto = Rezervisano)
- ✅ Tasteri 1-9: Kupovina karata (crveno = Kupljeno)
  - Pronalazi N susednih slobodnih sedišta počevši od najdesnijeg u poslednjem redu

### Projekcija:
- ✅ Enter: Početak projekcije
  - Vrata se otvaraju
  - Tamnosivi pravougaonik nestaje
  - Ljudi ulaze u salu (slučajan broj, maksimalno koliko ima rezervisanih + kupljenih)
  - Svaka osoba se kreće vertikalno do svog reda, zatim horizontalno do sedišta

- ✅ Film traje 20 sekundi
  - Platno menja boju na nasumičnu svakih 20 frejmova

- ✅ Nakon filma:
  - Platno postaje belo
  - Vrata se otvaraju
  - Ljudi izlaze istom putanjom
  - Kada svi izađu, vrata se zatvaraju i program se vraća na početno stanje

### Tehnički zahtevi:
- ✅ Full screen mod
- ✅ Frame limiter na 75 FPS
- ✅ Escape taster za izlaz
- ✅ Kursor u obliku filmske kamere
- ✅ Poluprovidna tekstura sa informacijama o studentu (donji levi ugao)

## Struktura projekta

```
Kostur/
├── Header/
│   ├── Cinema.h          # Glavna logika bioskopa
│   ├── Person.h          # Klasa za osobe
│   ├── Renderer.h        # Renderovanje scene
│   ├── Seat.h            # Klasa za sedišta
│   ├── Util.h            # Pomoćne funkcije
│   └── stb_image.h       # Biblioteka za učitavanje slika
├── Source/
│   ├── Cinema.cpp        # Implementacija logike bioskopa
│   ├── Main.cpp          # Glavni fajl
│   ├── Renderer.cpp      # Implementacija renderovanja
│   └── Util.cpp          # Implementacija pomoćnih funkcija
├── Shaders/
│   ├── vertex_shader.glsl    # Vertex sejder
│   └── fragment_shader.glsl # Fragment sejder
└── Resources/            # Resursi (slike, teksture)
```

## Kontrole

- **Levi klik miša**: Rezervacija/otkazivanje sedišta
- **Tasteri 1-9**: Kupovina 1-9 karata
- **Enter**: Početak projekcije
- **Escape**: Izlaz iz programa

## Prilagođavanje informacija o studentu

Za dodavanje informacija o studentu (ime, prezime, indeks), izmenite funkciju `renderStudentInfo()` u fajlu `Source/Renderer.cpp`. 

Trenutno se koristi jednostavan placeholder. Za produkciju, preporučuje se:
1. Kreiranje teksture sa tekstom koristeći FreeType biblioteku, ili
2. Učitavanje prethodno kreirane teksture sa tekstom

## Kompajliranje

Projekat se kompajlira kroz Visual Studio ili kroz komandnu liniju:

```bash
# Visual Studio
msbuild Kostur.sln /p:Configuration=Debug /p:Platform=x64
```

## Napomene

- Projekat koristi programabilni OpenGL pipeline (GLSL sejderi)
- Svi zahtevi iz specifikacije su implementirani
- Kod je organizovan u više fajlova za bolju čitljivost i održivost

