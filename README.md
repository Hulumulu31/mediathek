# Mediathek / Playlists

Итоговый мини-проект: консольная медиатека на C++17 с PostgreSQL.

Слои: `domain` → `repository` → `service` → `cli`.

## Зависимости

- CMake ≥ 3.16
- MinGW (MSYS2) или другой C++17 компилятор
- PostgreSQL
- libpqxx (`pacman -S mingw-w64-x86_64-libpqxx` в MSYS2)

## База данных

1. Создать БД:

```bash
createdb mediathek
```

(или через `psql`: `CREATE DATABASE mediathek;`)

2. Применить схему и демо-данные:

```bash
psql -U postgres -d mediathek -f sql/init.sql
```

## Подключение

Скопируйте пример и заполните пароль:

```bash
copy config.example.txt config.txt
```

Либо задайте переменные окружения:

- `PGHOST` (по умолчанию `localhost`)
- `PGPORT` (по умолчанию `5432`)
- `PGUSER` (по умолчанию `postgres`)
- `PGPASSWORD`
- `PGDATABASE` (по умолчанию `mediathek`)

Пароль **не** зашит в код.

## Сборка

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
```

## Запуск

Из каталога, где лежит `config.txt` (обычно корень проекта или `build/`):

```bash
.\build\mediathek.exe
```

Если `config.txt` в корне проекта:

```bash
cd C:\tm_final
.\build\mediathek.exe
```

## Меню

1. Tracks — CRUD треков  
2. Playlists — создать / список / переименовать / удалить  
3. Playlist contents — состав плейлиста  
0. Exit  

## Структура

```text
src/domain/       — Track, Playlist, ошибки
src/repository/   — SQL / libpqxx
src/service/      — бизнес-правила
src/cli/          — консольное меню
sql/init.sql      — схема БД
```
