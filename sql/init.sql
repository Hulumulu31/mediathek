-- Медиатека / плейлисты
-- Применение: psql -U postgres -d mediathek -f sql/init.sql
-- или: createdb mediathek && psql -d mediathek -f sql/init.sql

DROP TABLE IF EXISTS playlist_tracks;
DROP TABLE IF EXISTS playlists;
DROP TABLE IF EXISTS tracks;

CREATE TABLE tracks (
    id            SERIAL PRIMARY KEY,
    title         VARCHAR(200) NOT NULL,
    artist        VARCHAR(200) NOT NULL,
    duration_sec  INTEGER NOT NULL CHECK (duration_sec > 0)
);

CREATE TABLE playlists (
    id         SERIAL PRIMARY KEY,
    name       VARCHAR(200) NOT NULL UNIQUE,
    capacity   INTEGER NOT NULL CHECK (capacity > 0),
    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE playlist_tracks (
    playlist_id INTEGER NOT NULL REFERENCES playlists(id) ON DELETE CASCADE,
    track_id    INTEGER NOT NULL REFERENCES tracks(id) ON DELETE CASCADE,
    position    INTEGER NOT NULL CHECK (position >= 1),
    PRIMARY KEY (playlist_id, track_id),
    UNIQUE (playlist_id, position)
);

-- Демо-данные
INSERT INTO tracks (title, artist, duration_sec) VALUES
    ('Bohemian Rhapsody', 'Queen', 354),
    ('Imagine', 'John Lennon', 183),
    ('Billie Jean', 'Michael Jackson', 294),
    ('Smells Like Teen Spirit', 'Nirvana', 301),
    ('Hotel California', 'Eagles', 390);

INSERT INTO playlists (name, capacity) VALUES
    ('Favorites', 20),
    ('Road Trip', 10);

INSERT INTO playlist_tracks (playlist_id, track_id, position) VALUES
    (1, 1, 1),
    (1, 2, 2),
    (1, 3, 3),
    (2, 4, 1),
    (2, 5, 2);
